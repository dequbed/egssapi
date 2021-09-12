#ifndef EGSSAPI_ERRORS_H_NADWTW1I
#define EGSSAPI_ERRORS_H_NADWTW1I

#include <erl_nif.h>
#include <stdint.h>

ERL_NIF_TERM
egssapi_raise_exception(ErlNifEnv* env, uint32_t major, uint32_t minor);

#endif /* end of include guard: EGSSAPI_ERRORS_H_NADWTW1I */
