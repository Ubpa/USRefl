set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030")
endif()

Ubpa_AddTarget(
  TEST
  MODE EXE
  C_OPTION
    ${c_options}
)
