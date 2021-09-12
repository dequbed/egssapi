#ifndef EGSSAPI_H_V9JCYORN
#define EGSSAPI_H_V9JCYORN

#include <stdint.h>
#include <gssapi.h>
#include <erl_nif.h>

typedef struct {
    uint32_t version;
    ErlNifResourceType* gss_name_type;
    ErlNifResourceType* gss_cred_handle_type;
    ErlNifResourceType* gss_ctx_handle_type;
} egssapi_privdata_struct;

typedef egssapi_privdata_struct* egssapi_privdata_t;

#endif /* end of include guard: EGSSAPI_H_V9JCYORN */
