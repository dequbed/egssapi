#ifndef RESOURCE_GSS_NAME_H_B5V2R9AM
#define RESOURCE_GSS_NAME_H_B5V2R9AM

#include <gssapi.h>
#include <erl_nif.h>

typedef struct {
    gss_name_t inner;
} egssapi_gss_name;

typedef struct {
    gss_cred_id_t inner;
} egssapi_gss_cred_handle;

typedef struct {
    gss_ctx_id_t inner;
} egssapi_gss_ctx_handle;

ErlNifResourceType* load_gss_name(ErlNifEnv* caller_env);
ErlNifResourceType* load_cred_handle(ErlNifEnv* caller_env);
ErlNifResourceType* load_ctx_handle(ErlNifEnv* caller_env);

#endif /* end of include guard: RESOURCE_GSS_NAME_H_B5V2R9AM */
