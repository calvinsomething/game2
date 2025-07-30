cmake_minimum_required(VERSION 3.12)

project(game2)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
endif()

function(set_src_list)
	foreach(path ${ARGN})
		list(APPEND src_list "${PROJECT_SOURCE_DIR}/src/${path}")
	endforeach()
	set(src_list "${src_list}" PARENT_SCOPE)
endfunction()

set_src_list(main.cpp
	Window.cpp
	Error.cpp
	models/Cube.cpp
	gfx/Gfx.cpp
	gfx/Camera.cpp
	gfx/Buffer.cpp
	gfx/VertexBuffer.cpp
	gfx/IndexBuffer.cpp
	gfx/ConstantBuffer.cpp
	gfx/Shader.cpp
	gfx/PixelShader.cpp
	gfx/VertexShader.cpp
	input/Input.cpp)

set(CMAKE_WIN32_EXECUTABLE TRUE)

add_executable(${PROJECT_NAME} ${src_list})

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		UNICODE
		WIN32_LEAN_AND_MEAN)

target_link_libraries(${PROJECT_NAME}
	d3d11
	dxgi
	dxguid)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	execute_process(COMMAND
		${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/compile_commands.json ${PROJECT_SOURCE_DIR}/compile_commands.json)
endif()

# Shaders
#
add_custom_target(shaders)

set(shaders_src ${PROJECT_SOURCE_DIR}/src/Gfx/shaders)
set(shaders_bin ${CMAKE_BINARY_DIR}/shaders)

target_compile_definitions(${PROJECT_NAME} PRIVATE SHADERS_BIN="${shaders_bin}")

file(MAKE_DIRECTORY ${shaders_bin})

set(shader_files
	vs.hlsl
	ps.hlsl)

set_source_files_properties(vs.hlsl PROPERTIES
	type vs
    version _5_0)

set_source_files_properties(ps.hlsl PROPERTIES
	type ps
    version _5_0)

foreach(file ${shader_files})
	get_filename_component(shader_name ${file} NAME_WE)
	get_source_file_property(shader_type ${file} type)
	get_source_file_property(shader_version ${file} version)

	add_custom_command(TARGET shaders
		COMMAND fxc.exe /T ${shader_type}${shader_version} /Fo ${shaders_bin}/${shader_name}.cso /Fd ${shaders_bin}/${shader_name}.pdb /Zi $<$<CONFIG:Debug>:/Od> ${file}
		MAIN_DEPENDENCY ${file}
		WORKING_DIRECTORY ${shaders_src}
		COMMENT "Compiling ${file}..."
		VERBATIM)
endforeach()

add_dependencies(${PROJECT_NAME} shaders)
