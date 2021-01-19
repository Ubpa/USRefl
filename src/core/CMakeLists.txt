set(c_options "")
if(MSVC)
  list(APPEND c_options "/wd5030;/wd5222")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  list(APPEND c_options "-Wno-attributes")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  list(APPEND c_options "-Wno-attributes")
endif()

Ubpa_AddTarget(
  MODE INTERFACE
  INC
    "${PROJECT_SOURCE_DIR}/include"
  C_OPTION
    ${c_options}
  LIB
    Ubpa::UTemplate_core
)
