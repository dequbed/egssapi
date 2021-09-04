#include "egssapi_errors.h"

void egssapi_raise_exception(ErlNifEnv* env, int major, int minor)
{
    enif_raise_exception(
        env,
        enif_make_tuple2(
            env,
            enif_make_int(env, major),
            enif_make_int(env, minor)
        )
    );
}
