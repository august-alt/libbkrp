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

#ifndef BKRP_CLIENT_H
#define BKRP_CLIENT_H

#include <bkrp/bkrp.h>

#include "bkrp_client_error.h"

/**
    @brief get_client_rpc_binding Gets a binding handle to an RPC interface.
    @param[out] binding_handle Output parameter to receive the binding handle.
    @param[in] interface_spec DCE Interface handle for service.
    @param[in] hostname Internet hostname where server lives.
    @param[in] protocol Protocol such as "ncacn_ip_tcp".
    @param[in] endpoint Optional endpoint.
    @return
       - DCE_ERR_OK - on successful bind.
       - DCE_ERR_* - on error.
*/
dce_err
get_client_rpc_binding(
    rpc_binding_handle_t * binding_handle,
    rpc_if_handle_t interface_spec,
    char * hostname,
    char * protocol,
    char * endpoint);

/**
 * @brief create_rpc_identity Sets rpc to use kerberos for authentication and packet level security.
 * @param hostname Internet hostname where server lives.
 * @param rpc_binding_h the binding handle.
 * @return
 *      - 0 - on success.
 *      - code of the error - on error.
 */
int
create_rpc_identity(
    char * hostname,
    rpc_binding_handle_t rpc_binding_h);

#endif//BKRP_CLIENT_H
