%
-module(egssapi_nif).

-export(
   [ accept_sec_context/1
   , init_sec_context/1
   , acquire_cred/4
]).
-on_load(init/0).

init() ->
  % erlang:load_nif(Path, LoadInfo)
  % Path is the path to the loadable object minus extension (i.e .so or .dll)
  % LoadInfo can be any term and will be passed as-is to load() & upgrade() in
  % the NIF lib.
  ok = erlang:load_nif("./priv/egssapi_nif", 0).

accept_sec_context(_Token) ->
  erlang:exit(nif_library_not_loaded).

init_sec_context(_Token) ->
  erlang:exit(nif_library_not_loaded).

acquire_cred(_Name, _Lifetime, _Mechs, _Usage) ->
  erlang:exit(nif_library_not_loaded).

