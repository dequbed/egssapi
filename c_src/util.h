#ifndef UTIL_H_3QZRMFHX
#define UTIL_H_3QZRMFHX

#include <erl_nif.h>
#include <stdint.h>
#include <stdbool.h>
#include <gssapi/gssapi.h>

ERL_NIF_TERM
buffer_into_binary(ErlNifEnv* env, gss_buffer_desc buffer);

ERL_NIF_TERM
build_retcode(ErlNifEnv* env, uint32_t major, uint32_t minor);

ERL_NIF_TERM
make_boolean(ErlNifEnv* env, bool input);

#endif /* end of include guard: UTIL_H_3QZRMFHX */
