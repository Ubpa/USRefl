if(NOT Ubpa_USRefl_Build_Parser)
  return()
endif()

Ubpa_AddTarget(
  MODE EXE
  LIB
    Ubpa::UANTLR_ParserCpp14
)
