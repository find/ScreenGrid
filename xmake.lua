target("grid")
  set_kind("binary")
  set_optimize("smallest")
  set_runtimes("MT")
  add_files("ScreenGrid/*.cpp")
  add_files("ScreenGrid/*.rc")
  add_syslinks("user32","gdi32","comdlg32")
