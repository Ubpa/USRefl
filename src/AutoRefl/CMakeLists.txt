if(NOT Ubpa_USRefl_Build_AutoRefl)
  return()
endif()

Ubpa_AddTarget(
  CXX_STANDARD 17
  MODE EXE
  OUTPUT_NAME AutoRefl
  LIB
    Ubpa::UANTLR_ParserCpp14
  INC
    "${PROJECT_SOURCE_DIR}/include"
)
