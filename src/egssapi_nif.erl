%
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

-record(gss_accept_sec_context_output, {
          major :: integer(),
          minor :: integer()
}).

%-spec accept_sec_context(egssapi_cred_handle, egssapi_ctx_handle, binary) -> gss_accept_sec_context_output.
accept_sec_context(_Creds, _Context, _Token) ->
  erlang:exit(nif_library_not_loaded).

-record(gss_acquire_cred_output, {
          cred :: egssapi_cred_handle,
          lifetime :: integer()
}).
-spec acquire_cred(integer(), integer()) -> gss_acquire_cred_output.
acquire_cred(_Lifetime, _Usage) ->
  erlang:exit(nif_library_not_loaded).

empty_context() ->
  erlang:exit(nif_library_not_loaded).

show_error(_Error) ->
  erlang:exit(nif_library_not_loaded).
