#include "egssapi_resources.h"
#include "egssapi_atoms.h"
#include "egssapi_errors.h"

static void gss_name_dtor(ErlNifEnv* caller_env, void* obj)
{
    egssapi_gss_name* gss_name = (egssapi_gss_name*)obj;
    uint32_t major, minor;
    major = gss_release_name(&minor, gss_name->inner);

    if (major != GSS_S_COMPLETE) {
        egssapi_raise_exception(caller_env, major, minor);
    }
}

ErlNifResourceType* load_gss_name(ErlNifEnv* caller_env)
{
    return enif_open_resource_type(caller_env,
                                   NULL,
                                   "egssapi_gss_name",
                                   gss_name_dtor,
                                   ERL_NIF_RT_CREATE,
                                   NULL);
}

static void gss_cred_handle_dtor(ErlNifEnv* caller_env, void* obj)
{
    egssapi_gss_cred_handle* gss_cred_handle= (egssapi_gss_cred_handle*)obj;
    uint32_t major, minor;
    major = gss_release_cred(&minor, gss_cred_handle->inner);

    if (major != GSS_S_COMPLETE) {
        egssapi_raise_exception(caller_env, major, minor);
    }
}

ErlNifResourceType* load_cred_handle(ErlNifEnv* caller_env)
{
    return enif_open_resource_type(caller_env,
                                   NULL,
                                   "egssapi_cred_handle",
                                   gss_cred_handle_dtor,
                                   ERL_NIF_RT_CREATE,
                                   NULL);
}

static void gss_ctx_handle_dtor(ErlNifEnv* caller_env, void* obj)
{
    egssapi_gss_ctx_handle* gss_ctx_handle= (egssapi_gss_ctx_handle*)obj;
    uint32_t major, minor;
    major = gss_delete_sec_context(&minor, gss_ctx_handle->inner, NULL);

    if (major != GSS_S_COMPLETE) {
        egssapi_raise_exception(caller_env, major, minor);
    }
}

ErlNifResourceType* load_ctx_handle(ErlNifEnv* caller_env)
{
    return enif_open_resource_type(caller_env,
                                   NULL,
                                   "egssapi_ctx_handle",
                                   gss_ctx_handle_dtor,
                                   ERL_NIF_RT_CREATE,
                                   NULL);
}
