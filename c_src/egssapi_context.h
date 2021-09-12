#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <erl_nif.h>
#include <gssapi.h>

ERL_NIF_TERM
egssapi_accept_sec_context(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

ERL_NIF_TERM
egssapi_empty_context(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

ERL_NIF_TERM
egssapi_context_time(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

ERL_NIF_TERM
egssapi_inquire_context(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
