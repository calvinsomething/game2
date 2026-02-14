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
	debug.cpp
	Error.cpp
	game/Camera.cpp
	game/Character.cpp
	game/Clock.cpp
	game/Controller.cpp
	game/Skybox.cpp
	gfx/Buffer.cpp
	gfx/ComputeShader.cpp
	gfx/ConstantBuffer.cpp
	gfx/DepthStencil.cpp
	gfx/Gfx.cpp
	gfx/IndexBuffer.cpp
	gfx/PixelShader.cpp
	gfx/Shader.cpp
	gfx/StaticBuffer.cpp
	gfx/Texture.cpp
	gfx/VertexBuffer.cpp
	gfx/VertexShader.cpp
	Global.cpp
	input/Input.cpp
	models/Animations.cpp
	models/Cube.cpp
	models/Material.cpp
	models/Mesh.cpp
	util.cpp
	Window.cpp)

set(CMAKE_WIN32_EXECUTABLE TRUE)

add_executable(${PROJECT_NAME} ${src_list})

target_compile_definitions(${PROJECT_NAME}
	PRIVATE
		UNICODE
		WIN32_LEAN_AND_MEAN)

# Libraries
#
add_subdirectory(vendor/include)

target_link_directories(${PROJECT_NAME}
	PRIVATE
		C:/Lib)

target_link_libraries(${PROJECT_NAME}
	d3d11
	dxgi
	dxguid

	assimp/unit
	assimp/assimp-vc143-mtd

	DDSTextureLoader
	memory_allocator)

target_include_directories(${PROJECT_NAME}
	PRIVATE
		C:/Include
		vendor/include)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	execute_process(COMMAND
		${CMAKE_COMMAND} -E create_symlink ${CMAKE_BINARY_DIR}/compile_commands.json ${PROJECT_SOURCE_DIR}/compile_commands.json)
endif()

add_custom_target(runtime_deps)

function(copy_runtime_deps)
	file(GLOB_RECURSE file_names vendor/bin/*)

	add_custom_command(TARGET runtime_deps
		COMMAND ${CMAKE_COMMAND} -E copy ${file_names} ${CMAKE_BINARY_DIR}
		COMMENT "Updating dependency binaries..."
		DEPENDS vendor/bin)
endfunction()

copy_runtime_deps()

# Shaders
#
add_custom_target(shaders)

set(shaders_src ${PROJECT_SOURCE_DIR}/src/gfx/shaders)
set(shaders_bin ${CMAKE_BINARY_DIR}/shaders)

target_compile_definitions(${PROJECT_NAME} PRIVATE SHADERS_BIN="${shaders_bin}")

file(MAKE_DIRECTORY ${shaders_bin})

set(vs_files
	vs.hlsl
	vs_tex.hlsl
	vs_skybox.hlsl)

set_source_files_properties(${vs_files}
	PROPERTIES
		profile vs_5_0)

set(ps_files
	ps.hlsl
	ps_tex.hlsl
	ps_skybox.hlsl)

set_source_files_properties(${ps_files}
	PROPERTIES
		profile ps_5_0)

set(cs_files
	cs_skybox.hlsl)

set_source_files_properties(${cs_files}
	PROPERTIES
		profile cs_5_0)

set(shader_files "${vs_files};${ps_files};${cs_files}")

foreach(file ${shader_files})
	get_filename_component(shader_name ${file} NAME_WE)
	get_source_file_property(shader_profile ${file} profile)

	add_custom_command(TARGET shaders
		COMMAND fxc.exe /T ${shader_profile} /Fo ${shaders_bin}/${shader_name}.cso /Fd ${shaders_bin}/${shader_name}.pdb /Zi $<$<CONFIG:Debug>:/Od> ${file}
		MAIN_DEPENDENCY ${file}
		WORKING_DIRECTORY ${shaders_src}
		COMMENT "Compiling ${file}...")
endforeach()

# Assets
#
add_custom_target(assets)

add_custom_command(TARGET assets
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
	DEPENDS ${PROJECT_SOURCE_DIR}/assets/*
	COMMENT "Updating assets...")

# Dependency Targets
#
add_dependencies(${PROJECT_NAME}
	shaders
	assets
	runtime_deps)
