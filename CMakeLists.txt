cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(USRefl VERSION 0.2.5)
message(STATUS "[Project] ${PROJECT_NAME}")

include(cmake/InitUCMake.cmake)
Ubpa_InitUCMake()

Ubpa_InitProject()

Ubpa_AddSubDirsRec(src)

Ubpa_Export(DIRECTORIES "include")
