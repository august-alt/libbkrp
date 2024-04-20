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

#include "bkrp_client_error.h"

#include <dce/dcethread.h>
#include <compat/dcerpc.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <uchar.h>
#include "bkrp.h"

#include <locale.h>
#include <errno.h>

#include <openssl/md5.h>
#include <openssl/hmac.h>

#include <krb5.h>
#include <gssapi/gssapi.h>
#include <gssapi/gssapi_ext.h>
#include <gssapi/gssapi_generic.h>
#include <gssapi/gssapi_krb5.h>

int
create_rpc_identity(
    char * hostname,
    rpc_binding_handle_t rpc_binding_h)
{
    unsigned32 serr = 0;

    rpc_binding_set_auth_info(rpc_binding_h,
        (unsigned char *) hostname,
        rpc_c_authn_level_pkt_privacy,
        rpc_c_authn_gss_negotiate,
        NULL,
        rpc_c_authz_name,
        &serr);

    if (serr)
    {
        printf("rpc_binding_set_auth_info - error: 0x%08x!\n", serr);
        goto error;
    }

error:
    return serr;
}


dce_err
get_client_rpc_binding(
    rpc_binding_handle_t * binding_handle,
    rpc_if_handle_t interface_spec,
    char * hostname,
    char * protocol,
    char * endpoint
)
{
    char * string_binding = NULL;
    error_status_t status;

    rpc_string_binding_compose(NULL,
                               (unsigned char *) protocol,
                               (unsigned char *) hostname,
                               (unsigned char *) endpoint,
                               NULL,
                               (unsigned char **) &string_binding,
                               &status);
    if (status != error_status_ok)
    {
        return DCE_ERR_RPC_STRING_BINDING_COMPOSE;
    }

    rpc_binding_from_string_binding((unsigned char *)string_binding,
                                    binding_handle,
                                    &status);
    if (status != error_status_ok)
    {
        return DCE_ERR_RPC_BINDING_FROM_STRING_BINDING;
    }

    if (!endpoint)
    {
        rpc_ep_resolve_binding(*binding_handle, interface_spec, &status);
        if (status != error_status_ok)
        {
            return DCE_ERR_RPC_EP_RESOLVE_BINDING;
        }
    }

    rpc_string_free((unsigned char **) &string_binding, &status);
    if (status != error_status_ok)
    {
        return DCE_ERR_RPC_STRING_FREE;
    }

    return DCE_ERR_OK;
}
