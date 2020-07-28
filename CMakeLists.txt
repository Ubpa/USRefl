cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(USRefl VERSION 0.4.3)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

option(Ubpa_USRefl_Build_Parser "build parser" OFF)
if(Ubpa_USRefl_Build_Parser)
  Ubpa_AddDep(UANTLR 0.1.1)
endif()

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

Ubpa_Export(DIRECTORIES "include")
