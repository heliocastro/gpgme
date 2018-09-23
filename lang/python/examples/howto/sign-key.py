#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from __future__ import absolute_import, division, unicode_literals

# Copyright (C) 2018 Ben McGinnes <ben@gnupg.org>
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 2.1 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License and the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU General Public License and the GNU
# Lesser General Public along with this program; if not, see
# <http://www.gnu.org/licenses/>.

import gpg
import os.path

print("""
This script signs or certifies a key.

The gpg-agent and pinentry are invoked to enter the passphrase.
""")

c = gpg.Context()

homedir = input("Enter the GPG configuration directory path (optional): ")
fpr0 = input("Enter the fingerprint of the key to sign: ")
userid = input("Enter the UID to sign (case sensitive, optional): ")
sig_type = input("Enter the certification type (local or normal): ")

if homedir.startswith("~"):
    if os.path.exists(os.path.expanduser(homedir)) is True:
        c.home_dir = os.path.expanduser(homedir)
    else:
        pass
elif os.path.exists(homedir) is True:
    c.home_dir = homedir
else:
    pass

fpr = "".join(fpr0.split())
key = c.get_key(fpr, secret=False)

if len(userid) > 0 and sig_type.lower() == "local":
    c.key_sign(key, uids=userid, local=True)
elif len(userid) > 0 and sig_type.lower() != "local":
    c.key_sign(key, uids=userid)
elif len(userid) == 0 and sig_type.lower() == "local":
    c.key_sign(key, local=True)
else:
    c.key_sign(key)
