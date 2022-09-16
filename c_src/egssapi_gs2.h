#pragma once

#include <erl_nif.h>
#include <stdint.h>
#include <gssapi/gssapi.h>

ERL_NIF_TERM
egssapi_inquire_saslname_for_mech(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);

ERL_NIF_TERM
egssapi_inquire_mech_for_saslname(ErlNifEnv* env, int argc, const ERL_NIF_TERM argv[]);
