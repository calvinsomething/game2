cmake_minimum_required(VERSION 3.12)

project(game2)

if(CMAKE_BUILD_TYPE)
	set(build_type)
	string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
	set(CMAKE_BUILD_TYPE "${build_type}")
else()
	set(CMAKE_BUILD_TYPE debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL debug)
	set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
endif()

function(set_src_list)
	foreach(path ${ARGN})
		list(APPEND src_list "${PROJECT_SOURCE_DIR}/src/${path}")
	endforeach()
	set(src_list "${src_list}" PARENT_SCOPE)
endfunction()

set_src_list(main.cpp)

add_executable(${PROJECT_NAME} ${src_list})

if(CMAKE_BUILD_TYPE STREQUAL debug)
	execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/compile_commands.json ${PROJECT_SOURCE_DIR}/compile_commands.json)
endif()
