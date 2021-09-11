#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <erl_nif.h>

#include <gssapi.h>

#include "egssapi_resources.h"

typedef struct {
    uint32_t version;
    ErlNifResourceType* gss_name_type;
    ErlNifResourceType* gss_cred_handle_type;
    ErlNifResourceType* gss_ctx_handle_type;
} egssapi_privdata_struct;

typedef egssapi_privdata_struct* egssapi_privdata_t;

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
buffer_to_str(ErlNifEnv* env, gss_buffer_desc buffer)
{
    ERL_NIF_TERM string;
    void* string_ptr = enif_make_new_binary(env, buffer.length, &string);
    memcpy(string_ptr, buffer.value, buffer.length);
    return string;
}

static ERL_NIF_TERM
egssapi_raise_is_unloaded(ErlNifEnv* env)
{
    return enif_raise_exception(env, enif_make_atom(env, "egssapi_is_unloaded"));
}

static ERL_NIF_TERM
build_retcode(ErlNifEnv* env, uint32_t major, uint32_t minor)
{
    ERL_NIF_TERM erl_major = enif_make_int(env, major);
    ERL_NIF_TERM erl_minor = enif_make_int(env, minor);

    return enif_make_tuple2(env, erl_major, erl_minor);
}

static ERL_NIF_TERM
egssapi_raise_exception(ErlNifEnv* env, uint32_t major, uint32_t minor)
{
    return enif_raise_exception(env, build_retcode(env, major, minor));
}

static ERL_NIF_TERM
build_display_err(ErlNifEnv* env, uint32_t major, uint32_t minor, gss_OID mech)
{
    uint32_t maj, min, context;
    gss_buffer_desc bufmaj, bufmin;

    if ((maj = gss_display_status(&min, major, 1, mech, &context, &bufmaj)) != GSS_S_COMPLETE) {
        return egssapi_raise_exception(env, maj, min);
    }

    if ((maj = gss_display_status(&min, minor, 2, mech, &context, &bufmin)) != GSS_S_COMPLETE) {
        return egssapi_raise_exception(env, maj, min);
    }

    ERL_NIF_TERM erl_context = enif_make_int(env, context);
    ERL_NIF_TERM erl_major_str = buffer_to_str(env, bufmaj);
    ERL_NIF_TERM erl_minor_str = buffer_to_str(env, bufmin);

    if ((maj = gss_release_buffer(&min, &bufmaj)) != GSS_S_COMPLETE) {
        return egssapi_raise_exception(env, maj, min);
    }
    if ((maj = gss_release_buffer(&min, &bufmin)) != GSS_S_COMPLETE) {
        return egssapi_raise_exception(env, maj, min);
    }

    return enif_make_tuple4(
               env,
               build_retcode(env, major, minor),
               erl_context,
               erl_major_str,
               erl_minor_str
           );
}

static ERL_NIF_TERM
egssapi_show_error(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    const ERL_NIF_TERM* errtpl;
    int arity;

    if (!enif_get_tuple(env, argv[0], &arity, &errtpl)) {
        return enif_make_badarg(env);
    }

    uint32_t major,minor;

    if (arity != 2
            || !enif_get_uint(env, errtpl[0], &major)
            || !enif_get_uint(env, errtpl[1], &minor)) {
        return enif_make_badarg(env);
    }

    return build_display_err(env, major, minor, NULL);
}

static ERL_NIF_TERM
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

static ERL_NIF_TERM
egssapi_acquire_cred(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    egssapi_privdata_t privdata;

    if (!(privdata = (egssapi_privdata_t) enif_priv_data(env))) {
        return egssapi_raise_is_unloaded(env);
    }

    int cred_usage;

    if (!enif_get_int(env, argv[2], &cred_usage)) {
        return enif_make_badarg(env);
    }

    uint32_t major, minor;
    gss_OID_set mechs;
    uint32_t lifetime;
    egssapi_gss_cred_handle* cred_handle
        = enif_alloc_resource(privdata->gss_cred_handle_type, sizeof(egssapi_gss_cred_handle));

    major = gss_acquire_cred(
                &minor,
                GSS_C_NO_NAME,
                0,
                NULL,
                cred_usage,
                &cred_handle->inner,
                &mechs,
                &lifetime
            );

    if (major != GSS_S_COMPLETE) {
        return egssapi_raise_exception(env, major, minor);
    }

    ERL_NIF_TERM cred = enif_make_resource(env, (void*)cred_handle);
    return enif_make_tuple2(env,
                            cred,
                            enif_make_int(env, lifetime)
                           );
}

static ERL_NIF_TERM
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


static ERL_NIF_TERM
oid_to_erl(ErlNifEnv* env, gss_OID oid)
{
    uint32_t major, minor;
    gss_buffer_desc buffer;
    major = gss_oid_to_str(&minor, oid, &buffer);

    if (major != GSS_S_COMPLETE) {
        return build_retcode(env, major, minor);
    }

    return buffer_to_string(env, buffer);
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
