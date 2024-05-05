#include "bkrp/bkrp_client.h"
#include "bkrp/ndr_bkrp_c.h"

int main(int argc, char ** argv)
{
    (void)argc;
    (void)argv;

    TALLOC_CTX *mem_ctx = talloc_named(NULL, 0, "create_rpc_client");
    struct dcerpc_pipe* pipe = NULL;
    NTSTATUS status;

    status = get_client_rpc_binding(
                                    mem_ctx,
                                    &pipe,
                                    "dc0.domain.alt",
                                    "DOMAIN.ALT",
                                    "administrator");

    if (!NT_STATUS_IS_OK(status)) {
        printf("Failed to establish RPC connection: %u\n", status.v);
        exit(EXIT_FAILURE);
    }

    uint8_t pDataIn[] = "cleartext";
    uint32_t cbDataIn = sizeof(pDataIn);
    uint8_t *ppDataOut = 0;
    uint32_t pcbDataOut = 0;
    struct GUID guid;

    status = GUID_from_string(BACKUPKEY_BACKUP_GUID, &guid);

    if (!NT_STATUS_IS_OK(status)) {
        printf("Failed to convert guid: %u\n", status.v);
        exit(EXIT_FAILURE);
    }

    uint32_t result = 0;
    dcerpc_bkrp_BackupKey(
                      pipe->binding_handle,
                      mem_ctx,
                      &guid,
                      pDataIn,
                      cbDataIn,
                      &ppDataOut,
                      &pcbDataOut,
                      0,
                      &result);

    if (result)
    {
        printf("Failed to backup key! %d\n", result);
        exit(EXIT_FAILURE);
    }

    status = GUID_from_string(BACKUPKEY_RESTORE_GUID, &guid);

    if (!NT_STATUS_IS_OK(status)) {
        printf("Failed to convert guid: %u\n", status.v);
        exit(EXIT_FAILURE);
    }

    uint8_t *ppDataOut2 = 0;
    uint32_t pcbDataOut2 = 0;

    dcerpc_bkrp_BackupKey(
                      pipe->binding_handle,
                      mem_ctx,
                      &guid,
                      ppDataOut,
                      pcbDataOut,
                      &ppDataOut2,
                      &pcbDataOut2,
                      0,
                      &result);

    if (result)
    {
        printf("Failed to restore key! %d\n", result);
        exit(EXIT_FAILURE);
    }

    if (cbDataIn != pcbDataOut2)
    {
        printf("Backuped and restored keys do not match!%s, %s\n", ppDataOut2, pDataIn);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Backuped and restored keys match! %s, %s\n", ppDataOut2, pDataIn);
    }

    // Cleanup
    TALLOC_FREE(pipe);
    TALLOC_FREE(mem_ctx);

    return 0;
}
