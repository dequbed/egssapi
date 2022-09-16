-module(egssapi_nif).

-include("egssapi.hrl").

-export(
    [ accept_sec_context/3
    , acquire_cred/2
    , empty_context/0
    , show_error/1
    , inquire_context/1
    ]).

-on_load(init/0).

init() ->
  % erlang:load_nif(Path, LoadInfo)
  % Path is the path to the loadable object minus extension (i.e .so or .dll)
  % LoadInfo can be any term and will be passed as-is to load() & upgrade() in
  % the NIF lib.
  ok = erlang:load_nif("./target/priv/egssapi_nif", 0).


-spec accept_sec_context(credentials(), context(), Token :: binary())
    -> { Context :: context(), Token :: binary() }.
accept_sec_context(Creds, Context, Token) ->
  erlang:nif_error(nif_library_not_loaded, [Creds, Context, Token]).

-spec acquire_cred(integer(), integer()) -> { credentials(), integer() }.
acquire_cred(Lifetime, Usage) ->
  erlang:nif_error(nif_library_not_loaded, [Lifetime, Usage]).

-spec empty_context() -> context().
empty_context() ->
  erlang:nif_error(nif_library_not_loaded).

-spec show_error(gss_retcode()) -> gss_error().
show_error(Error) ->
  erlang:nif_error(nif_library_not_loaded, [Error]).

-spec inquire_context(context())
    -> { SourceName :: name()
       , TargetName :: name()
       , Lifetime :: integer()
       , Flags :: contextflags()
       , LocallyInitiated :: boolean()
       , Open :: boolean()
       }.
inquire_context(Context) ->
  erlang:nif_error(nif_library_not_loaded, [Context]).