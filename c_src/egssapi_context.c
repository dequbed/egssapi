#include "egssapi.h"
#include "egssapi_context.h"
#include "egssapi_resources.h"
#include "egssapi_errors.h"

ERL_NIF_TERM
egssapi_accept_sec_context(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    egssapi_privdata_t privdata;

    if (!(privdata = (egssapi_privdata_t) enif_priv_data(env))) {
        return egssapi_raise_is_unloaded(env);
    }

    egssapi_gss_cred_handle* acceptor_cred_handle;
    egssapi_gss_ctx_handle* context_handle;
    ErlNifBinary erl_token;

    if (!enif_get_resource(env, argv[0], privdata->gss_cred_handle_type,
                           (void*)&acceptor_cred_handle)) {
        return enif_make_badarg(env);
    }

    if (!enif_get_resource(env, argv[1], privdata->gss_ctx_handle_type, (void*)&context_handle)) {
        return enif_make_badarg(env);
    }

    // TODO: Allow providing channel binding data.

    if (!enif_inspect_binary(env, argv[2], &erl_token)) {
        return enif_make_badarg(env);
    }

    uint32_t major, minor, lifetime, ret_flags;
    gss_buffer_desc token, output;
    token.length = erl_token.size;
    token.value = erl_token.data;

    gss_cred_id_t delegated;
    gss_OID mech;
    gss_name_t src_name;

    major = gss_accept_sec_context(
                &minor,
                &context_handle->inner,
                acceptor_cred_handle->inner,
                &token,
                NULL,
                &src_name,
                &mech,
                &output,
                &ret_flags,
                &lifetime,
                &delegated
            );

    if (major != GSS_S_COMPLETE && major != GSS_S_CONTINUE_NEEDED) {
        if (output.value) {
            gss_release_buffer(NULL, &output);
        }

        return build_display_err(env, major, minor, mech);
    }

    ERL_NIF_TERM erl_output;
    void* output_ptr = enif_make_new_binary(env, output.length, &erl_output);
    memcpy(output_ptr, output.value, output.length);

    gss_release_buffer(NULL, &output);

    return enif_make_tuple2(env, erl_output, argv[1]);
}

ERL_NIF_TERM
egssapi_empty_context(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    egssapi_privdata_t privdata;

    if (!(privdata = (egssapi_privdata_t) enif_priv_data(env))) {
        return egssapi_raise_is_unloaded(env);
    }

    egssapi_gss_ctx_handle* context_handle = enif_alloc_resource(privdata->gss_ctx_handle_type,
            sizeof(egssapi_gss_ctx_handle));
    context_handle->inner = GSS_C_NO_CONTEXT;

    return enif_make_resource(env, (void*)context_handle);
}
