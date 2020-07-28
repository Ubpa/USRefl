if(NOT Ubpa_USRefl_Build_AutoRefl)
  return()
endif()

Ubpa_USReflAutoRefl(
  "${CMAKE_CURRENT_SOURCE_DIR}/Vec.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/Vec_AutoRefl.inl"
)

set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030;/permissive-")
endif()

Ubpa_AddTarget(
  TEST
  MODE EXE
  SOURCE
    "${CMAKE_CURRENT_SOURCE_DIR}/Vec_AutoRefl.inl"
  LIB
    Ubpa::USRefl_core
  C_OPTION
    ${c_options}
)
