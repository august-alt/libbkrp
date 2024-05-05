/***********************************************************************************************************************
**
** Copyright (C) 2024 BaseALT Ltd. <org@basealt.ru>
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public License
** as published by the Free Software Foundation; either version 2
** of the License, or (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**
***********************************************************************************************************************/

#include <iostream>
#include <fstream>
#include <memory>
#include <vector>

extern "C"
{
#include <bkrp/bkrp_client.h>
#include <bkrp/ndr_bkrp_c.h>
}

class RpcWrapper
{
private:
    TALLOC_CTX* mem_ctx;
    dcerpc_pipe* pipe;

public:
    RpcWrapper(const std::string &host, const std::string& domain, const std::string& username)
        : mem_ctx(nullptr),
          pipe(nullptr)
    {
        mem_ctx = talloc_named(NULL, 0, "create_rpc_client");

        if (!NT_STATUS_IS_OK(get_client_rpc_binding(mem_ctx,
                                   &pipe,
                                   const_cast<char*>(host.c_str()),
                                   const_cast<char*>(domain.c_str()),
                                   const_cast<char*>(username.c_str()))))
        {
            std::cout << "Couldnt obtain RPC server binding. exiting.\n" << std::endl;
            return;
        }
    }

    ~RpcWrapper()
    {
        TALLOC_FREE(pipe);
        TALLOC_FREE(mem_ctx);
    }

    uint32_t BackupKey(GUID &pguidActionAgent,
                uint8_t *pDataIn,
                uint32_t cbDataIn,
                uint8_t **ppDataOut,
                uint32_t *pcbDataOut)
    {
        uint32_t result = 0;
        dcerpc_bkrp_BackupKey(
                          pipe->binding_handle,
                          mem_ctx,
                          &pguidActionAgent,
                          pDataIn,
                          cbDataIn,
                          ppDataOut,
                          pcbDataOut,
                          0,
                          &result);

        return result;
    }
};

int main(int argc, char **argv)
{
    std::vector<std::string> args(argv, argv+argc);
    if(args.size() < 4)
    {
        std::cerr << "Usage: " << args[0] << " [host] [domain] [username]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string hostname = args[1];
    std::string domain = args[2];
    std::string username = args[3];

    std::unique_ptr<RpcWrapper> wrapper { new  RpcWrapper(hostname, domain, username) };

    uint8_t pDataIn[] = "cleartext";
    uint32_t cbDataIn = sizeof(pDataIn);
    uint8_t *ppDataOut = 0;
    uint32_t pcbDataOut = 0;
    GUID guid;
    NTSTATUS status;

    status = GUID_from_string(BACKUPKEY_BACKUP_GUID, &guid);

    if (!NT_STATUS_IS_OK(status)) {
        printf("Failed to convert guid: %u\n", status.v);
        exit(EXIT_FAILURE);
    }

    uint32_t result = 0;
    wrapper->BackupKey(guid,
                       pDataIn,
                       cbDataIn,
                       &ppDataOut,
                       &pcbDataOut);

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

    wrapper->BackupKey(guid,
                       ppDataOut,
                       pcbDataOut,
                       &ppDataOut2,
                       &pcbDataOut2);

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

    return 0;
}
