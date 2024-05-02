/* SPDX-License-Identifier: BSD-2-Clause */
/***********************************************************************;
 * Copyright (c) 2015 - 2017, Intel Corporation
 * All rights reserved.
 ***********************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h" // IWYU pragma: keep
#endif

#include "sysapi_util.h"      // for _TSS2_SYS_CONTEXT_BLOB, syscontext_cast
#include "tss2_common.h"      // for TSS2_RC, TSS2_SYS_RC_BAD_REFERENCE
#include "tss2_mu.h"          // for Tss2_MU_UINT32_Marshal, Tss2_MU_TPM2B_A...
#include "tss2_sys.h"         // for TSS2_SYS_CONTEXT, TSS2L_SYS_AUTH_COMMAND
#include "tss2_tpm2_types.h"  // for TPM2B_ATTEST, TPM2B_DATA, TPMI_DH_OBJECT

TSS2_RC Tss2_Sys_GetCommandAuditDigest_Prepare(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_RH_ENDORSEMENT privacyHandle,
    TPMI_DH_OBJECT signHandle,
    const TPM2B_DATA *qualifyingData,
    const TPMT_SIG_SCHEME *inScheme)
{
    _TSS2_SYS_CONTEXT_BLOB *ctx = syscontext_cast(sysContext);
    TSS2_RC rval;

    if (!ctx || !inScheme)
        return TSS2_SYS_RC_BAD_REFERENCE;

    rval = CommonPreparePrologue(ctx, TPM2_CC_GetCommandAuditDigest);
    if (rval)
        return rval;

    rval = Tss2_MU_UINT32_Marshal(privacyHandle, ctx->cmdBuffer,
                                  ctx->maxCmdSize,
                                  &ctx->nextData);
    if (rval)
        return rval;

    rval = Tss2_MU_UINT32_Marshal(signHandle, ctx->cmdBuffer,
                                  ctx->maxCmdSize,
                                  &ctx->nextData);
    if (rval)
        return rval;

    if (!qualifyingData) {
        ctx->decryptNull = 1;

        rval = Tss2_MU_UINT16_Marshal(0, ctx->cmdBuffer,
                                      ctx->maxCmdSize,
                                      &ctx->nextData);
    } else {

        rval = Tss2_MU_TPM2B_DATA_Marshal(qualifyingData, ctx->cmdBuffer,
                                          ctx->maxCmdSize,
                                          &ctx->nextData);
    }

    if (rval)
        return rval;

    rval = Tss2_MU_TPMT_SIG_SCHEME_Marshal(inScheme, ctx->cmdBuffer,
                                           ctx->maxCmdSize,
                                           &ctx->nextData);
    if (rval)
        return rval;

    ctx->decryptAllowed = 1;
    ctx->encryptAllowed = 1;
    ctx->authAllowed = 1;

    return CommonPrepareEpilogue(ctx);
}

TSS2_RC Tss2_Sys_GetCommandAuditDigest_Complete(
    TSS2_SYS_CONTEXT *sysContext,
    TPM2B_ATTEST *auditInfo,
    TPMT_SIGNATURE *signature)
{
    _TSS2_SYS_CONTEXT_BLOB *ctx = syscontext_cast(sysContext);
    TSS2_RC rval;

    if (!ctx)
        return TSS2_SYS_RC_BAD_REFERENCE;

    rval = CommonComplete(ctx);
    if (rval)
        return rval;

    rval = Tss2_MU_TPM2B_ATTEST_Unmarshal(ctx->cmdBuffer,
                                          ctx->maxCmdSize,
                                          &ctx->nextData, auditInfo);
    if (rval)
        return rval;

    return Tss2_MU_TPMT_SIGNATURE_Unmarshal(ctx->cmdBuffer,
                                            ctx->maxCmdSize,
                                            &ctx->nextData, signature);
}

TSS2_RC Tss2_Sys_GetCommandAuditDigest(
    TSS2_SYS_CONTEXT *sysContext,
    TPMI_RH_ENDORSEMENT privacyHandle,
    TPMI_DH_OBJECT signHandle,
    TSS2L_SYS_AUTH_COMMAND const *cmdAuthsArray,
    const TPM2B_DATA *qualifyingData,
    const TPMT_SIG_SCHEME *inScheme,
    TPM2B_ATTEST *auditInfo,
    TPMT_SIGNATURE *signature,
    TSS2L_SYS_AUTH_RESPONSE *rspAuthsArray)
{
    _TSS2_SYS_CONTEXT_BLOB *ctx = syscontext_cast(sysContext);
    TSS2_RC rval;

    if (!inScheme)
        return TSS2_SYS_RC_BAD_REFERENCE;

    rval = Tss2_Sys_GetCommandAuditDigest_Prepare(sysContext, privacyHandle,
                                                  signHandle, qualifyingData,
                                                  inScheme);
    if (rval)
        return rval;

    rval = CommonOneCall(ctx, cmdAuthsArray, rspAuthsArray);
    if (rval)
        return rval;

    return Tss2_Sys_GetCommandAuditDigest_Complete(sysContext, auditInfo,
                                                   signature);
}
