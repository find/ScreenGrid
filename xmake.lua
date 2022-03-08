target("grid")
  set_kind("binary")
  set_optimize("smallest")
  set_runtimes("MT")
  add_defines("UNICODE")
  add_files("ScreenGrid/*.cpp")
  add_files("ScreenGrid/*.rc")
  add_syslinks("user32","gdi32","comdlg32")
  if is_subhost('msys') then
    add_ldflags('-static')
    add_ldflags('-Wl,--subsystem,windows')
  else
    add_rules("win.sdk.application")
  end
