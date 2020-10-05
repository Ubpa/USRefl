cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(USRefl VERSION 0.5.5)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
include(cmake/UbpaUSReflAutoRefl.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

option(Ubpa_USRefl_Build_AutoRefl "build AutoRefl, need Java" OFF)
if(Ubpa_USRefl_Build_AutoRefl)
  Ubpa_AddDep(UANTLR 0.1.4)
endif()

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

Ubpa_Export(
  TARGET
  DIRECTORIES
    "include"
    "cmake"
)
