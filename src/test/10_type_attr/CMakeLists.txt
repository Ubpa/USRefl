set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030;/permissive-")
endif()

Ubpa_AddTarget(
  TEST
  MODE EXE
  LIB
    Ubpa::USRefl_core
  C_OPTION
    ${c_options}
)
