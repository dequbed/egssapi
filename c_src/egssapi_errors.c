#include "util.h"
#include "egssapi_errors.h"

ERL_NIF_TERM
egssapi_raise_exception(ErlNifEnv* env, uint32_t major, uint32_t minor)
{
    return enif_raise_exception(env, build_retcode(env, major, minor));
}
