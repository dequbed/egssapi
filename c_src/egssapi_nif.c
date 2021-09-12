#include "egssapi.h"
#include "util.h"
#include "egssapi_errors.h"
#include "egssapi_resources.h"
#include "egssapi_context.h"
#include "egssapi_credentials.h"

int
egssapi_load (ErlNifEnv* caller_env, egssapi_privdata_t* priv_data_ptr, ERL_NIF_TERM load_info)
{
    *priv_data_ptr = enif_alloc(sizeof(egssapi_privdata_struct));
    egssapi_privdata_t priv_data = *priv_data_ptr;
    priv_data->version = 0;

    if (!(priv_data->gss_name_type = load_gss_name(caller_env))) {
        return 1;
    }

    if (!(priv_data->gss_cred_handle_type = load_cred_handle(caller_env))) {
        return 2;
    }

    if (!(priv_data->gss_ctx_handle_type = load_ctx_handle(caller_env))) {
        return 3;
    }

    return 0;
}


static ERL_NIF_TERM
oid_to_erl(ErlNifEnv* env, gss_OID oid)
{
    uint32_t major, minor;
    gss_buffer_desc buffer;
    major = gss_oid_to_str(&minor, oid, &buffer);

    if (major != GSS_S_COMPLETE) {
        return build_retcode(env, major, minor);
    }

    return buffer_to_str(env, buffer);
}

uint32_t
// Returns 0 on success, has errored otherwise
erl_to_oid(uint32_t* minor, ErlNifEnv* env, ERL_NIF_TERM str, gss_OID* out)
{
    uint32_t len;

    if (!enif_get_list_length(env, str, &len)) {
        enif_raise_exception(env, enif_make_badarg(env));
        return 1;
    }

    gss_buffer_desc buffer = {
        .value = malloc(len+1),
        .length = len+1,
    };

    int ret;

    if ((ret = enif_get_string(env, str, buffer.value, len+1, ERL_NIF_LATIN1)) < 1) {
        buffer.length = ret;
        ret = gss_str_to_oid(minor, &buffer, out);
        free(buffer.value);
        return ret;
    }

    enif_raise_exception(env, enif_make_badarg(env));
    return -2;
}


// Almost all GSSAPI calls have to be treated as dirty nifs that are IO bound
// since they may open files, do network calls and so on.
static ErlNifFunc nif_funcs[] = {
    {"accept_sec_context", 3, egssapi_accept_sec_context, ERL_NIF_DIRTY_JOB_IO_BOUND},
    {"acquire_cred", 2, egssapi_acquire_cred, ERL_NIF_DIRTY_JOB_IO_BOUND},
    {"empty_context", 0, egssapi_empty_context},
    {"show_error", 1, egssapi_show_error},
};

typedef int (*load)(ErlNifEnv* caller_env, void** priv_data, ERL_NIF_TERM load_info) ;

ERL_NIF_INIT(
    /* MODULE  */ egssapi_nif,
    /* funcs   */ nif_funcs,
    /* load    */ (load)&egssapi_load,
    /* reload  */ NULL, // Not supported since OTP 20
    /* upgrade */ NULL,
    /* unload  */ NULL
)

// vim: set tw=100 ts=4 sts=4 sw=4 expandtab
