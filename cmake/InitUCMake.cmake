macro(Ubpa_InitUCMake)
  include(FetchContent)
  set(UCMake_VERSION 0.6.3)
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
