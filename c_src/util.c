#include <string.h>
#include "util.h"

ERL_NIF_TERM
buffer_into_binary(ErlNifEnv* env, gss_buffer_desc buffer)
{
    ERL_NIF_TERM string;
    void* string_ptr = enif_make_new_binary(env, buffer.length, &string);
    memcpy(string_ptr, buffer.value, buffer.length);
    return string;
}

ERL_NIF_TERM
build_retcode(ErlNifEnv* env, uint32_t major, uint32_t minor)
{
    ERL_NIF_TERM erl_major = enif_make_uint(env, major);
    ERL_NIF_TERM erl_minor = enif_make_uint(env, minor);

    return enif_make_tuple2(env, erl_major, erl_minor);
}

ERL_NIF_TERM
make_boolean(ErlNifEnv* env, bool input)
{
    return enif_make_atom_len(env,
        input ? "true" : "false",
        input ? 4      : 5
    );
}
