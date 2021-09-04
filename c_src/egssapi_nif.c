#include <erl_nif.h>

#include <krb5.h>

extern int foo(int x);
extern int bar(int y);

static ERL_NIF_TERM foo_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int x, ret;
    if (!enif_get_int(env, argv[0], &x)) {
        return enif_make_badarg(env);
    }
    ret = foo(x);
    return enif_make_int(env, ret);
}

static ERL_NIF_TERM bar_nif(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{
    int y, ret;
    if (!enif_get_int(env, argv[0], &y)) {
        return enif_make_badarg(env);
    }
    ret = bar(y);
    return enif_make_int(env, ret);
}

static ERL_NIF_TERM egssapi_accept_sec_context(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[])
{

}

static ErlNifFunc nif_funcs[] = {
        {"accept_sec_context", 1, egssapi_accept_sec_context},
};

ERL_NIF_INIT(
    /* MODULE  */ egssapi_internal,
    /* funcs   */ nif_funcs,
    /* load    */ NULL,
    /* reload  */ NULL, // Not supported since OTP 20
    /* upgrade */ NULL,
    /* unload  */ NULL
)
