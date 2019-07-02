cmake_minimum_required (VERSION 2.7)
project(ServerDump)
aux_source_directory(. fileList)
add_executable(server ${fileList})