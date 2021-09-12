#include "egssapi_credentials.h"
#include "egssapi_resources.h"
#include "egssapi_errors.h"
#include "egssapi.h"

ERL_NIF_TERM
egssapi_acquire_cred(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    egssapi_privdata_t privdata;

    if (!(privdata = (egssapi_privdata_t) enif_priv_data(env))) {
        return egssapi_raise_is_unloaded(env);
    }

    uint32_t cred_usage;

    if (!enif_get_uint(env, argv[2], &cred_usage)) {
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
                            enif_make_uint(env, lifetime)
                           );
}
