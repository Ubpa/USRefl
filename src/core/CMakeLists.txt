set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030")
endif()

set(libs "")
set(defines "")
if(Ubpa_USRefl_UseNameof)
  list(APPEND libs nameof::nameof)
else()
  list(APPEND defines UBPA_USREFL_NOT_USE_NAMEOF)
endif()

Ubpa_AddTarget(
  MODE INTERFACE
  INC
    "${PROJECT_SOURCE_DIR}/include"
  C_OPTION
    ${c_options}
  LIB
    ${libs}
  DEFINE
    ${defines}
)
