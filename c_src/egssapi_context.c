#include "egssapi.h"
#include "egssapi_context.h"
#include "egssapi_resources.h"
#include "egssapi_errors.h"

#include "util.h"

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

        return enif_raise_exception(env, build_display_err(env, major, minor, mech));
    }

    ERL_NIF_TERM erl_output;
    void* output_ptr = enif_make_new_binary(env, output.length, &erl_output);
    memcpy(output_ptr, output.value, output.length);
    gss_release_buffer(NULL, &output);

    return enif_make_tuple2(env, argv[1], erl_output);
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

ERL_NIF_TERM
egssapi_context_time(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    egssapi_privdata_t privdata;
    if (!(privdata = (egssapi_privdata_t) enif_priv_data(env))) {
        return egssapi_raise_is_unloaded(env);
    }


    egssapi_gss_ctx_handle* context_handle;
    if (!enif_get_resource(env, argv[1], privdata->gss_ctx_handle_type, (void*)&context_handle)) {
        return enif_make_badarg(env);
    }

    uint32_t major, minor, lifetime;

    major = gss_context_time(
                &minor,
                context_handle->inner,
                &lifetime
            );

    if (major != GSS_S_COMPLETE && major != GSS_S_CONTINUE_NEEDED) {
        return egssapi_raise_exception(env, major, minor);
    }

    return enif_make_uint(env, lifetime);
}

ERL_NIF_TERM
egssapi_inquire_context(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    egssapi_privdata_t privdata;
    if (!(privdata = (egssapi_privdata_t) enif_priv_data(env))) {
        return egssapi_raise_is_unloaded(env);
    }

    egssapi_gss_ctx_handle* context_handle;
    if (!enif_get_resource(env, argv[0], privdata->gss_ctx_handle_type, (void*)&context_handle)) {
        return enif_make_badarg(env);
    }

    uint32_t major, minor, lifetime, flags;
    int32_t locally_initiated, open;

    egssapi_gss_name* src_name = enif_alloc_resource(privdata->gss_name_type,
            sizeof(egssapi_gss_name));
    ERL_NIF_TERM erl_src_name = enif_make_resource(env, src_name);

    egssapi_gss_name* target_name = enif_alloc_resource(privdata->gss_name_type,
            sizeof(egssapi_gss_name));
    ERL_NIF_TERM erl_target_name = enif_make_resource(env, target_name);

    gss_OID mech_type;

    major = gss_inquire_context(
                &minor,
                context_handle->inner,
                &src_name->inner,
                &target_name->inner,
                &lifetime,
                &mech_type,
                &flags,
                &locally_initiated,
                &open
            );

    if (major != GSS_S_COMPLETE && major != GSS_S_CONTINUE_NEEDED) {
        return egssapi_raise_exception(env, major, minor);
    }

    return enif_make_tuple6(env, 
        erl_src_name,
        erl_target_name,
        enif_make_uint(env, lifetime),
        enif_make_tuple9(env,
            make_boolean(env, 0 != (flags & GSS_C_DELEG_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_MUTUAL_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_REPLAY_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_SEQUENCE_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_CONF_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_INTEG_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_ANON_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_PROT_READY_FLAG)),
            make_boolean(env, 0 != (flags & GSS_C_TRANS_FLAG))
        ),
        make_boolean(env, locally_initiated),
        make_boolean(env, open)
    );
}
