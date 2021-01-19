macro(Ubpa_InitUCMake)
  cmake_parse_arguments(
    "ARG" # prefix
    "" #<options> # TRUE / FALSE
    "VERSION" # <one_value_keywords>
    "" #<multi_value_keywords> # list
    ${ARGN}
  )
  # 结果为 ARG_*
  # - ARG_<option>
  # - ARG_<one_value_keyword>
  # - ARG_<multi_value_keyword>
  
  if("${ARG_VERSION}" STREQUAL "")
    message(FATAL_ERROR "not provide argument <VERSION>")
  endif()
  
  include(FetchContent)
  set(UCMake_VERSION ${ARG_VERSION})
  message(STATUS "find package: UCMake ${UCMake_VERSION}")
  find_package(UCMake ${UCMake_VERSION} EXACT QUIET)
  if(NOT UCMake_FOUND)
    message(STATUS "UCMake ${UCMake_VERSION} not found")
    set(_address "https://github.com/Ubpa/UCMake")
    message(STATUS "fetch: ${_address} with tag ${UCMake_VERSION}")
    FetchContent_Declare(
      UCMake
      GIT_REPOSITORY ${_address}
      GIT_TAG ${UCMake_VERSION}
    )
    FetchContent_MakeAvailable(UCMake)
    message(STATUS "UCMake ${UCMake_VERSION} build done")
  endif()
endmacro()
