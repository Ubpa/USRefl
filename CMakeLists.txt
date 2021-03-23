cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(USRefl VERSION 0.8.1)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
include(cmake/UbpaUSReflAutoRefl.cmake)

Ubpa_InitUCMake(VERSION 0.6.4)

Ubpa_InitProject()

option(Ubpa_USRefl_Build_AutoRefl "build AutoRefl" OFF)
if(Ubpa_USRefl_Build_AutoRefl)
  message(STATUS "USRefl build AutoRefl")
  Ubpa_AddDep(UANTLR 0.2.1)
else()
  message(STATUS "USRefl doesn't build AutoRefl")
endif()

Ubpa_AddDep(UTemplate 0.7.2)

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

set(installFiles "")
if(Ubpa_USRefl_Build_AutoRefl)
  list(APPEND installFiles "${PROJECT_SOURCE_DIR}/cmake/UbpaUSReflAutoRefl.cmake")
endif()

Ubpa_PackageName(package_name)
install(
  FILES
    ${installFiles}
  DESTINATION "${CMAKE_INSTALL_PREFIX}/${package_name}/cmake"
)

Ubpa_Export(
  TARGET
  DIRECTORIES
    "include"
)
