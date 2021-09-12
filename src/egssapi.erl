-module(egssapi).

%% @headerfile "egssapi.hrl"
-include("src/egssapi.hrl").

-export(
    [ accept_context/2
    , accept_context/3
    , inquire_context/1

    , acquire_credentials/2
    , initiate_and_accept/0
    , initiate_only/0
    , accept_only/0
    ]).

-export_type(
    [ context/0
    , credentials/0
    , gss_retcode/0
    , gss_string/0
    , gss_error/0
    ]).

-type accept_context_ret()
    :: {ok, Context :: context(), Token :: binary() }
     | {error, Error :: gss_error()}.

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
        { NewContext, OutputToken } -> {ok, NewContext, OutputToken}
    catch
        error:Error -> {error, egssapi_nif:show_error(Error) }
    end.

-spec inquire_context(context()) 
    -> {ok, 
        Source :: name(), 
        Target :: name(), 
        Lifetime :: integer(), 
        Flags :: contextflags(),
        LocallyInitiated :: boolean(), 
        Open :: boolean()}
    |  {error, gss_error() }.
inquire_context(Context) ->
    try egssapi_nif:inquire_context(Context)
    of
        { SourceName, TargetName, Lifetime, Flags, LocallyInitiated, Open } ->
            {ok, SourceName, TargetName, Lifetime, Flags, LocallyInitiated, Open }
    catch
        error:Error -> {error, egssapi_nif:show_error(Error) }
    end.


-spec acquire_credentials(integer(), credentials_usage)
    -> {ok, credentials(), integer()}
     | {error, gss_error()}.
%% @doc Acquire credentials for future GSSAPI operations.
%%
%% Maps to gss_acquire_credentials with desired_name and desired_mechs set to NULL
acquire_credentials(RequestedLifetime, Usage) ->
    try
        UsageInt = case Usage of
            #credentials_usage.initiate_and_accept -> 0;
            #credentials_usage.initiate_only -> 1;
            #credentials_usage.accept_only -> 2
        end,
        egssapi_nif:acquire_cred(RequestedLifetime, UsageInt)
    of
        {Cred, ActualLifetime} -> {ok, Cred, ActualLifetime}
    catch
        % Makes sure errors from a bad cred usage don't make it into show_error()
        error:{case_clause, _} -> erlang:error(badarg);
        error:Error -> {error, egssapi_nif:show_error(Error)}
    end.

initiate_and_accept() -> ?CREDS_INITIATE_AND_ACCEPT.
initiate_only() -> ?CREDS_INITIATE_ONLY.
accept_only() -> ?CREDS_ACCEPT_ONLY.
