#include "util.h"
#include "egssapi_errors.h"

ERL_NIF_TERM
egssapi_raise_exception(ErlNifEnv* env, uint32_t major, uint32_t minor)
{
    return enif_raise_exception(env, build_retcode(env, major, minor));
}

ERL_NIF_TERM
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
    ERL_NIF_TERM erl_major_str = buffer_into_binary(env, bufmaj);
    ERL_NIF_TERM erl_minor_str = buffer_into_binary(env, bufmin);

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

ERL_NIF_TERM
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

ERL_NIF_TERM
egssapi_raise_is_unloaded(ErlNifEnv* env)
{
    return enif_raise_exception(env, enif_make_atom(env, "egssapi_is_unloaded"));
}
