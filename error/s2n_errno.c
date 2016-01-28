/*
 * Copyright 2014 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <strings.h>
#include <stdlib.h>

#include "error/s2n_errno.h"

#include <s2n.h>

__thread int s2n_errno;
__thread const char *s2n_debug_str;

struct s2n_error_translation
{
    int errno_value;
    const char * str;
};

struct s2n_error_translation EN[] = { 
    { S2N_ERR_OK, "no error" },
    { S2N_ERR_KEY_INIT, "error initializing encryption key" },
    { S2N_ERR_ENCRYPT, "error encrypting data" },
    { S2N_ERR_DECRYPT, "error decrypting data" },
    { S2N_ERR_MADVISE, "error calling madvise" },
    { S2N_ERR_ALLOC,   "error allocating memory" },
    { S2N_ERR_MLOCK,   "error calling mlock" },
    { S2N_ERR_FSTAT,   "error calling fstat" },
    { S2N_ERR_OPEN,    "error calling open" },
    { S2N_ERR_MMAP,    "error calling mmap" },
    { S2N_ERR_NULL,    "NULL pointer encountered" },
    { S2N_ERR_CLOSED,  "connection is closed" },
    { S2N_ERR_SAFETY,  "a safety check failed" },
    { S2N_ERR_NOT_INITIALIZED,  "s2n not initialized" },
    { S2N_ERR_RANDOM_UNITIALIZED, "s2n enctropy not initialized" },
    { S2N_ERR_OPEN_RANDOM, "error opening urandom" },
    { S2N_ERR_RESIZE_STATIC_STUFFER, "cannot resize a static stuffer" },
    { S2N_ERR_RESIZE_TAINTED_STUFFER, "cannot resize a tainted stuffer" },
    { S2N_ERR_STUFFER_OUT_OF_DATA, "stuffer is out of data" },
    { S2N_ERR_STUFFER_IS_FULL, "stuffer is full" },
    { S2N_ERR_INVALID_BASE64, "invalid base64 encountered" },
    { S2N_ERR_INVALID_PEM, "invalid PEM encountered" },
    { S2N_ERR_DH_COPYING_PARAMETERS, "error copying Diffie-Hellman parameters" },
    { S2N_ERR_DH_COPYING_PUBLIC_KEY, "error copying Diffie-Hellman public key" },
    { S2N_ERR_DH_GENERATING_PARAMETERS, "error generating Diffie-Hellman parameters" },
    { S2N_ERR_DH_PARAMS_CREATE, "error creating Diffie-Hellman parameters" },
    { S2N_ERR_DH_SERIAZING, "error serializing Diffie-Hellman parameters" },
    { S2N_ERR_DH_SHARED_SECRET, "error computing Diffie-Hellman shared secret" },
    { S2N_ERR_DH_WRITING_PUBLIC_KEY, "error writing Diffie-Hellman public key" },
    { S2N_ERR_DH_FAILED_SIGNING, "error signing Diffie-Hellman values" },
    { S2N_ERR_DH_TOO_SMALL, "Diffie-Hellman parameters are too small" },
    { S2N_ERR_DH_PARAMETER_CHECK, "Diffie-Hellman parameter check failed" },
    { S2N_ERR_INVALID_PKCS3, "invalid PKCS3 encountered" },
    { S2N_ERR_HASH_DIGEST_FAILED, "failed to create hash digest" },
    { S2N_ERR_HASH_INIT_FAILED, "error initializing hash" },
    { S2N_ERR_HASH_INVALID_ALGORITHM, "invalid hash algorithm" },
    { S2N_ERR_HASH_UPDATE_FAILED, "error updating hash" },
    { S2N_ERR_HMAC_INVALID_ALGORITHM, "invalid HMAC algorithm" },
    { S2N_ERR_SIZE_MISMATCH, "size mismatch" },
    { S2N_ERR_DECODE_CERTIFICATE, "error decoding certificate" },
    { S2N_ERR_DECODE_PRIVATE_KEY, "error decoding private key" },
    { S2N_ERR_KEY_MISMATCH, "public and private key do not match" },
    { S2N_ERR_NOMEM, "no memory" },
    { S2N_ERR_SIGN, "error signing data"  },
    { S2N_ERR_VERIFY_SIGNATURE, "error verifying signature" },
    { S2N_ERR_ALERT_PRESENT, "TLS alert is already pending" },
    { S2N_ERR_ALERT, "TLS alert received" },
    { S2N_ERR_CBC_VERIFY, "Failed CBC verification" },
    { S2N_ERR_CIPHER_NOT_SUPPORTED, "Cipher is not supported" },
    { S2N_ERR_BAD_MESSAGE, "Bad message encountered" },
    { S2N_ERR_INVALID_SIGNATURE_ALGORITHM, "Invalid signature algorithm" },
    { S2N_ERR_INVALID_KEY_EXCHANGE_ALGORITHM, "Invaid key exchange algorithm" },
    { S2N_ERR_NO_CERTIFICATE_IN_PEM, "No certificate in PEM" },
    { S2N_ERR_NO_ALERT, "No Alert present" },
    { S2N_ERR_CLIENT_MODE, "operation not allowed in client mode" },
    { S2N_ERR_SERVER_NAME_TOO_LONG, "server name is too long" },
    { S2N_ERR_CLIENT_MODE_DISABLED, "client connections not allowed" },
    { S2N_ERR_HANDSHAKE_STATE, "Invalid handshake state encountered" },
    { S2N_ERR_FALLBACK_DETECTED, "TLS fallback detected" },
    { S2N_ERR_INVALID_CIPHER_PREFERENCES, "Invalid Cipher Preferences version" },
    { S2N_ERR_APPLICATION_PROTOCOL_TOO_LONG, "Application protocol name is too long" },
    { S2N_ERR_NO_APPLICATION_PROTOCOL, "No supported application protocol to negotiate" },
    { S2N_ERR_DRBG, "Error using Determinstic Random Bit Generator" },
    { S2N_ERR_DRBG_REQUEST_SIZE, "Request for too much entropy" },
    { S2N_ERR_ECDHE_GEN_KEY, "Failed to generate an ECDHE key" },
    { S2N_ERR_ECDHE_SHARED_SECRET, "Error computing ECDHE shared secret" },
    { S2N_ERR_ECDHE_UNSUPPORTED_CURVE, "Unsupported EC curve was presented during an ECDHE handshake" },
    { S2N_ERR_ECDHE_SERIALIZING, "Error serializing ECDHE public" },
    { S2N_ERR_SHUTDOWN_PAUSED, "s2n_shutdown() called while paused" },
}; 

const char *s2n_strerror(int error, const char *lang)
{
    const char *no_such_language = "Language is not supported for error translation";
    const char *no_such_error = "Internal s2n error";
 
    if (lang == NULL) {
        lang = "EN"; 
    }

    if (strcasecmp(lang, "EN")) {
        return no_such_language;
    }

    for (int i = 0; i < (sizeof(EN) / sizeof(struct s2n_error_translation)); i++) {
        if (EN[i].errno_value == error) {
            return EN[i].str;
        }
    }

    return no_such_error;
}
