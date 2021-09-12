#ifndef EGSSAPI_ERRORS_H_NADWTW1I
#define EGSSAPI_ERRORS_H_NADWTW1I

#include <erl_nif.h>
#include <stdint.h>
#include <gssapi/gssapi.h>

ERL_NIF_TERM
egssapi_raise_exception(ErlNifEnv* env, uint32_t major, uint32_t minor);

ERL_NIF_TERM
build_display_err(ErlNifEnv* env, uint32_t major, uint32_t minor, gss_OID mech);

ERL_NIF_TERM
egssapi_show_error(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

ERL_NIF_TERM
egssapi_raise_is_unloaded(ErlNifEnv* env);

#endif /* end of include guard: EGSSAPI_ERRORS_H_NADWTW1I */
