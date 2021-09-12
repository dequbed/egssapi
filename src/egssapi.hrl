-type name() :: egssapi_gss_name.
-type context() :: egssapi_ctx_handle.
-type credentials() :: egssapi_cred_handle.

-type gss_retcode() :: { Major :: integer(), Minor :: integer() }.
-type gss_string() :: binary().
-type gss_error() :: { gss_retcode(), Status_Context :: integer(), Status_Major :: gss_string(), Status_Minor :: gss_string() }.

-record(contextflags,
    { deleg :: boolean()
    , mutual :: boolean()
    , replay :: boolean()
    , sequence :: boolean()
    , confidental :: boolean()
    , integrity :: boolean()
    , anonymous :: boolean()
    , ready :: boolean()
    , transferable :: boolean()
    }).
-type contextflags() :: contextflags.

-record(credentials_usage, {
    initiate_and_accept,
    initiate_only,
    accept_only
}).
-define(CREDS_INITIATE_AND_ACCEPT, #credentials_usage.initiate_and_accept).
-define(CREDS_INITIATE_ONLY, #credentials_usage.initiate_only).
-define(CREDS_ACCEPT_ONLY, #credentials_usage.accept_only).
