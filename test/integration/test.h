#include "sapi/tpm20.h"
#include "test-options.h"

/*
 * This is the prototype for all integration tests in the TPM2.0-TSS
 * project. Integration tests are intended to exercise the combined
 * components in the software stack. This typically means executing some
 * SAPI function using the socket TCTI to communicate with a software
 * TPM2 simulator.
 * Return values:
 * A successful test will return 0, any other value indicates failure.
 */
int test_invoke (TSS2_SYS_CONTEXT *sapi_context, test_opts_t *opts);
