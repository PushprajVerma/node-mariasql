/* Copyright (c) 2012, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */


/**
  @file

  @brief
  Wrapper functions for OpenSSL and YaSSL. Also provides a Compatibility layer
  to make available YaSSL's MD5 implementation.
*/

#include <my_global.h>
#include <my_md5.h>

#if defined(HAVE_YASSL)
// simplify config -bw
#include "config.h"
#include "md5.hpp"

static void my_md5_hash(char *digest, const char *buf, int len)
{
  TaoCrypt::MD5 hasher;
  hasher.Update((TaoCrypt::byte *) buf, len);
  hasher.Final((TaoCrypt::byte *) digest);
}

#elif defined(HAVE_OPENSSL)
#include <openssl/evp.h>

static void my_md5_hash(uchar* digest, const uchar *buf, uint len)
{
  EVP_MD_CTX ctx;
  EVP_MD_CTX_init(&ctx);
#ifdef EVP_MD_CTX_FLAG_NON_FIPS_ALLOW
  /* Ok to ignore FIPS: MD5 is not used for crypto here */
  EVP_MD_CTX_set_flags(&ctx, EVP_MD_CTX_FLAG_NON_FIPS_ALLOW);
#endif
  EVP_DigestInit_ex(&ctx, EVP_md5(), NULL);
  EVP_DigestUpdate(&ctx, buf, len);
  EVP_DigestFinal(&ctx, digest, &len);
  EVP_MD_CTX_cleanup(&ctx);
}

#endif /* HAVE_YASSL */

/**
    Wrapper function to compute MD5 message digest.

    @param digest [out]  Computed MD5 digest
    @param buf    [in]   Message to be computed
    @param len    [in]   Length of the message

    @return              void
*/
void compute_md5_hash(char *digest, const char *buf, int len)
{
#if defined(HAVE_YASSL)
  my_md5_hash(digest, buf, len);
#elif defined(HAVE_OPENSSL)
  my_md5_hash((unsigned char*)digest, (unsigned const char*)buf, len);
#endif /* HAVE_YASSL */
}
