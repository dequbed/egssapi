-module(egssapi_nif).

-export(
    [ accept_sec_context/3
    , acquire_cred/2
    , empty_context/0
    , show_error/1
    ]).

-on_load(init/0).

init() ->
  % erlang:load_nif(Path, LoadInfo)
  % Path is the path to the loadable object minus extension (i.e .so or .dll)
  % LoadInfo can be any term and will be passed as-is to load() & upgrade() in
  % the NIF lib.
  ok = erlang:load_nif("./priv/egssapi_nif", 0).


-spec accept_sec_context(egssapi_cred_handle, egssapi_ctx_handle, binary()) 
    -> { egssapi_ctx_handle, binary() }.
accept_sec_context(Creds, Context, Token) ->
  erlang:nif_error(nif_library_not_loaded, [Creds, Context, Token]).

-spec acquire_cred(integer(), integer()) -> { egssapi_cred_handle , integer() }.
acquire_cred(Lifetime, Usage) ->
  erlang:nif_error(nif_library_not_loaded, [Lifetime, Usage]).

-spec empty_context() -> egssapi_ctx_handle.
empty_context() ->
  erlang:nif_error(nif_library_not_loaded).

-spec show_error(egssapi:gss_retcode()) -> egssapi:gss_error().
show_error(Error) ->
  erlang:nif_error(nif_library_not_loaded, [Error]).
