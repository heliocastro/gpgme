/* encrypt-sign.c -  encrypt and verify functions
 *	Copyright (C) 2000 Werner Koch (dd9jn)
 *      Copyright (C) 2001, 2002 g10 Code GmbH
 *
 * This file is part of GPGME.
 *
 * GPGME is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GPGME is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "util.h"
#include "context.h"
#include "ops.h"


static void
encrypt_sign_status_handler (GpgmeCtx ctx, GpgStatusCode code, char *args)
{
  char *encrypt_info = 0;
  int encrypt_info_len;

  _gpgme_encrypt_status_handler (ctx, code, args);

  if (code == STATUS_EOF)
    {
      encrypt_info = gpgme_data_release_and_get_mem (ctx->op_info,
						     &encrypt_info_len);
      ctx->op_info = NULL;
    }
  _gpgme_sign_status_handler (ctx, code, args);
  if (code == STATUS_EOF && encrypt_info)
    _gpgme_data_append (ctx->op_info, encrypt_info, encrypt_info_len);
}


GpgmeError
gpgme_op_encrypt_sign_start (GpgmeCtx ctx, GpgmeRecipients recp,
			     GpgmeData plain, GpgmeData cipher)
{
  int err = 0;

  fail_on_pending_request (ctx);
  ctx->pending = 1;

  _gpgme_release_result (ctx);

  /* Do some checks.  */
  if (!gpgme_recipients_count (recp))
    {
      /* FIXME: In this case we should do symmetric encryption.  */
      err = mk_error (No_Recipients);
      goto leave;
    }

  /* Create an engine object.  */
  _gpgme_engine_release (ctx->engine);
  ctx->engine = NULL;
  err = _gpgme_engine_new (ctx->use_cms ? GPGME_PROTOCOL_CMS
			   : GPGME_PROTOCOL_OpenPGP, &ctx->engine);
  if (err)
    goto leave;

  err = _gpgme_passphrase_start (ctx);
  if (err)
    goto leave;

  _gpgme_engine_set_status_handler (ctx->engine,
				    encrypt_sign_status_handler, ctx);
  _gpgme_engine_set_verbosity (ctx->engine, ctx->verbosity);

  /* Check the supplied data */
  if (gpgme_data_get_type (plain) == GPGME_DATA_TYPE_NONE)
    {
      err = mk_error (No_Data);
      goto leave;
    }
  _gpgme_data_set_mode (plain, GPGME_DATA_MODE_OUT);
  if (!cipher || gpgme_data_get_type (cipher) != GPGME_DATA_TYPE_NONE)
    {
      err = mk_error (Invalid_Value);
      goto leave;
    }
  _gpgme_data_set_mode (cipher, GPGME_DATA_MODE_IN);

  err = _gpgme_engine_op_encrypt_sign (ctx->engine, recp, plain, cipher,
				       ctx->use_armor, ctx /* FIXME */);

  if (!err)	/* And kick off the process.  */
    err = _gpgme_engine_start (ctx->engine, ctx);

 leave:
  if (err)
    {
      ctx->pending = 0; 
      _gpgme_engine_release (ctx->engine);
      ctx->engine = NULL;
    }
  return err;
}


/**
 * gpgme_op_encrypt_sign:
 * @ctx: The context
 * @recp: The set of recipients
 * @plain: plaintext input
 * @cipher: signed ciphertext
 * 
 * This function encrypts @plain for all recipients in recp, signs it,
 * and returns the ciphertext in @out.  The function does wait for the
 * result.
 * 
 * Return value:  0 on success or an errorcode. 
 **/
GpgmeError
gpgme_op_encrypt_sign (GpgmeCtx ctx, GpgmeRecipients recp,
		       GpgmeData plain, GpgmeData cipher)
{
  GpgmeError err = gpgme_op_encrypt_sign_start (ctx, recp, plain, cipher);

  if (!err)
    {
      gpgme_wait (ctx, &err, 1);
      /* Old gpg versions don't return status info for invalid
         recipients, so we simply check whether we got any output at
         all, and if not we assume that we don't have valid
         recipients.  */
      if (!ctx->error && gpgme_data_get_type (cipher) == GPGME_DATA_TYPE_NONE)
        ctx->error = mk_error (No_Recipients);
      err = ctx->error;
    }
  return err;
}