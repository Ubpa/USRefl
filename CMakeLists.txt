cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(USRefl VERSION 0.6.0)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
include(cmake/UbpaUSReflAutoRefl.cmake)

Ubpa_InitUCMake()

Ubpa_InitProject()

option(Ubpa_USRefl_Build_AutoRefl "build AutoRefl" OFF)
if(Ubpa_USRefl_Build_AutoRefl)
  Ubpa_AddDep(UANTLR 0.2.0)
endif()

set(OptionCPM "")

option(Ubpa_USRefl_UseNameof "use nameof" OFF)
if(Ubpa_USRefl_UseNameof)
  set(OptionCPM "CPM")
  CPMAddPackage(
    NAME nameof
    GITHUB_REPOSITORY Neargye/nameof
    GIT_TAG 0e8bec0
  )
endif()

Ubpa_AddSubDirsRec(include)
Ubpa_AddSubDirsRec(src)

set(installFiles "cmake/InitUCMake.cmake")
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
  ${OptionCPM}
  DIRECTORIES
    "include"
    "cmake"
)
