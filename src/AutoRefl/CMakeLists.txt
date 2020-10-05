if(NOT Ubpa_USRefl_Build_AutoRefl)
  return()
endif()

Ubpa_AddTarget(
  MODE EXE
  OUTPUT_NAME AutoRefl
  LIB
    Ubpa::UANTLR_ParserCpp14
)
