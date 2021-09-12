#ifndef UTIL_H_3QZRMFHX
#define UTIL_H_3QZRMFHX

#include <erl_nif.h>
#include <stdint.h>
#include <gssapi/gssapi.h>

ERL_NIF_TERM
buffer_to_str(ErlNifEnv* env, gss_buffer_desc buffer);

ERL_NIF_TERM
build_retcode(ErlNifEnv* env, uint32_t major, uint32_t minor);

#endif /* end of include guard: UTIL_H_3QZRMFHX */
