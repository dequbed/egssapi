-module(egssapi).

%% @headerfile "egssapi.hrl"
-include("src/egssapi.hrl").

-export(
    [ accept_context/2
    , accept_context/3
    , acquire_credentials/2
    ]).

-export_type(
    [ context/0
    , credentials/0
    , credentials_usage/0
    , gss_retcode/0
    , gss_string/0
    , gss_error/0
    ]).

-type accept_context_ret()
    :: {ok, context(), binary() }
     | {error, gss_error()}.

-spec accept_context(credentials(), binary()) -> accept_context_ret().
%% @doc Accept and create an security context.
%%
%% Maps to gss_accept_sec_context with the context set to GSS_C_NO_CONTEXT.
%% This is the function you call when receiving the first Token from an initiator.
accept_context(Credentials, Token) ->
    accept_context(Credentials, egssapi_nif:empty_context(), Token).

-spec accept_context(credentials(), context(), binary()) -> accept_context_ret().
%% @doc Accept another token into the given security context.
%%
%% Maps to gss_accept_sec_context. This is the function you call for all Tokens received
%% after the first.
accept_context(Credentials, Context, Token) ->
    try egssapi_nif:accept_sec_context(Credentials, Context, Token)
    of
        { Context, Token } -> {ok, Context, Token}
    catch
        error:Error -> {error, egssapi_nif:show_error(Error) }
    end.

-spec acquire_credentials(integer(), credentials_usage())
    -> {ok, credentials(), integer()}
     | {error, gss_error()}.
%% @doc Acquire credentials for future GSSAPI operations.
%%
%% Maps to gss_acquire_credentials with desired_name and desired_mechs set to NULL
acquire_credentials(Lifetime, Usage) ->
    try
        UsageInt = case Usage of
            #credentials_usage.initiate_and_accept -> 0;
            #credentials_usage.initiate_only -> 1;
            #credentials_usage.accept_only -> 2
        end,
        egssapi_nif:acquire_cred(Lifetime, UsageInt)
    of
        { Credentials, Lifetime } -> {ok, Credentials, Lifetime}
    catch
        % Makes sure errors from a bad cred usage don't make it into show_error()
        error:{case_clause, _} -> erlang:error(badarg);
        error:Error -> {error, egssapi_nif:show_error(Error)}
    end.
