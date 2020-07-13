set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030;/permissive-")
endif()

Ubpa_AddTarget(
  TEST
  MODE EXE
  SOURCE
    "${PROJECT_SOURCE_DIR}/include/USRefl_99.h"
  INC
    "${PROJECT_SOURCE_DIR}/include"
  C_OPTION
    ${c_options}
)
