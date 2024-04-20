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
#include <dce/dcethread.h>
#include <bkrp/bkrp_client.h>
#include <bkrp/bkrp.h>
}

const char* PROTOCOL_TCP   = "ncacn_ip_tcp";
const int DEFAULT_RPC_PORT = 135;

GUID BACKUPKEY_BACKUP_GUID = { .Data1 = 0x7F752B10, .Data2 = 0x178E, .Data3 = 0x11D1, .Data4 = { 0xAB, 0x8F, 0x00, 0x80, 0x5F, 0x14, 0xDB, 0x40 } };
GUID BACKUPKEY_RESTORE_GUID_WIN2K = { .Data1 = 0x7FE94D50, .Data2 = 0x178E, .Data3 = 0x11D1, .Data4 = { 0xAB, 0x8F, 0x00, 0x80, 0x5F, 0x14, 0xDB, 0x40 } };
GUID BACKUPKEY_RETRIEVE_BACKUP_KEY_GUID = { .Data1 = 0x018FF48A, .Data2 = 0xEABA, .Data3 = 0x40C6, .Data4 = { 0x8F, 0x6D, 0x72, 0x37, 0x02, 0x40, 0xE9, 0x67 } };
GUID BACKUPKEY_RESTORE_GUID = { .Data1 = 0x47270C64, .Data2 = 0x2FC7, .Data3 = 0x499B, .Data4 = { 0xAC, 0x5B, 0x0E, 0x37, 0xCD, 0xCE, 0x89, 0x9A } };

class RpcWrapper
{
private:
    handle_t hBindingHandle;
    DWORD status;

public:
    RpcWrapper(const std::string &host, int port)
        : hBindingHandle(NULL),
          status(0)
    {
        if (get_client_rpc_binding(&hBindingHandle,
                                   BackupKey_v1_0_c_ifspec,
                                   const_cast<char*>(host.c_str()),
                                   const_cast<char*>(PROTOCOL_TCP),
                                   const_cast<char*>(std::to_string(port).c_str())) != DCE_ERR_OK)
        {
            std::cout << "Couldnt obtain RPC server binding. exiting.\n" << std::endl;
            return;
        }

        status = create_rpc_identity(const_cast<char*>(host.c_str()), hBindingHandle);
        if (status)
        {
            std::cout << "Couldn't set auth info" << status << " exiting.\n";
            return;
        }
    }

    ~RpcWrapper()
    {
        if (hBindingHandle)
        {
            rpc_binding_free(&hBindingHandle, &status);
        }
    }

    DWORD BackupKey(GUID &pguidActionAgent,
                void *pDataIn,
                DWORD cbDataIn,
                idl_byte **ppDataOut,
                DWORD *pcbDataOut)
    {
        dcethread_exc *exc;
        DCETHREAD_TRY
        {
          exc = NULL;
          (status) = BackuprKey(hBindingHandle, &pguidActionAgent, (idl_byte*)pDataIn, cbDataIn, ppDataOut, pcbDataOut, 0);
        }
        DCETHREAD_CATCH_ALL(exc)
        {
          (status) = dcethread_exc_getstatus(exc);
        }
        DCETHREAD_ENDTRY

        return status;
    }
};

int main(int argc, char **argv)
{
    std::vector<std::string> args(argv, argv+argc);
    if(args.size() < 2)
    {
        std::cerr << "Usage: " << args[0] << " [host] <port>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string hostname = args[1];
    unsigned int port = 135;

    if (args.size() >= 3)
    {
        try
        {
            port = std::stoi(args[2]);
        }
        catch(const std::invalid_argument& e)
        {
            std::cerr << "Error: Invalid port number. " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        catch(const std::out_of_range& e)
        {
            std::cerr << "Error: Port number out of range. " << e.what() << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::unique_ptr<RpcWrapper> wrapper { new  RpcWrapper(hostname, port) };

    wchar_t pDataIn[] = L"cleartext";
    DWORD cbDataIn = sizeof(pDataIn);
    idl_byte *ppDataOut = 0;
    DWORD pcbDataOut = 0;

    wrapper->BackupKey(BACKUPKEY_RETRIEVE_BACKUP_KEY_GUID, pDataIn, cbDataIn, &ppDataOut, &pcbDataOut);

    return 0;
}
