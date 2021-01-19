cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(USRefl VERSION 0.9.0)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
include(cmake/UbpaUSReflAutoRefl.cmake)

Ubpa_InitUCMake(VERSION 0.6.4)

Ubpa_InitProject()

option(Ubpa_USRefl_Build_AutoRefl "build AutoRefl" OFF)
if(Ubpa_USRefl_Build_AutoRefl)
  Ubpa_AddDep(UANTLR 0.2.1)
endif()

Ubpa_AddDep(UTemplate 0.6.0)

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

set(installFiles "")
if(Ubpa_USRefl_Build_AutoRefl)
  list(APPEND installFiles "cmake/UbpaUSReflAutoRefl.cmake")
endif()

install(
  FILES
    ${installFiles}
  DESTINATION "${package_name}/cmake"
)

Ubpa_Export(
  TARGET
  DIRECTORIES
    "include"
)
