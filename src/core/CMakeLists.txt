set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030")
endif()

Ubpa_AddTarget(
  MODE INTERFACE
  INC
    "${PROJECT_SOURCE_DIR}/include"
  C_OPTION
    ${c_options}
)
