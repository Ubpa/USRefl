cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(USRefl VERSION 0.8.0)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
include(cmake/UbpaUSReflAutoRefl.cmake)

Ubpa_InitUCMake()

Ubpa_InitProject()

option(Ubpa_USRefl_Build_AutoRefl "build AutoRefl" OFF)
if(Ubpa_USRefl_Build_AutoRefl)
  Ubpa_AddDep(UANTLR 0.2.0)
endif()

Ubpa_AddDep(UTemplate 0.5.3)

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
