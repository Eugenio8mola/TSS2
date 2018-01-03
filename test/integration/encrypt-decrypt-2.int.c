#include <string.h>
#include <inttypes.h>

#define LOGMODULE test
#include "log/log.h"
#include "sapi-util.h"
#include "test.h"

#define ENC_STR "test-data-test-data-test-data"

/*
 * This test is inteded to exercise the EncryptDecrypt2 command.
 */
int
test_invoke (TSS2_SYS_CONTEXT *sapi_context)
{
    TSS2_RC rc;
    TPM2_HANDLE handle_parent, handle;
    TPM2B_MAX_BUFFER data_in = { 0 };
    TPM2B_MAX_BUFFER data_encrypted = TPM2B_MAX_BUFFER_INIT;
    TPM2B_MAX_BUFFER data_decrypted = TPM2B_MAX_BUFFER_INIT;

    data_in.size = strlen (ENC_STR);
    strcpy ((char*)data_in.buffer, ENC_STR);

    rc = create_primary_rsa_2048_aes_128_cfb (sapi_context, &handle_parent);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR("Failed to create primary RSA 2048 key: 0x%" PRIx32 "",
                    rc);
        exit(1);
    }

    rc = create_aes_128_cfb (sapi_context, handle_parent, &handle);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR("Failed to create child AES 128 key: 0x%" PRIx32 "", rc);
        exit(1);
    }

    LOG_INFO("Encrypting data: \"%s\" with key handle: 0x%08" PRIx32,
               data_in.buffer, handle);
    rc = encrypt_2_cfb (sapi_context, handle, &data_in, &data_encrypted);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR("Failed to encrypt buffer: 0x%" PRIx32 "", rc);
        exit(1);
    }

    rc = decrypt_2_cfb (sapi_context, handle, &data_encrypted, &data_decrypted);
    if (rc != TSS2_RC_SUCCESS) {
        LOG_ERROR("Failed to encrypt buffer: 0x%" PRIx32 "", rc);
        exit(1);
    }
    LOG_INFO("Decrypted data: \"%s\" with key handle: 0x%08" PRIx32,
               data_decrypted.buffer, handle);

    if (strcmp ((char*)data_in.buffer, (char*)data_decrypted.buffer)) {
        LOG_ERROR("Decrypt succeeded but decrypted data != to input data");
        exit(1);
    }

    return 0;
}
