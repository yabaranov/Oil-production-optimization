cmake_minimum_required(VERSION 3.27 FATAL_ERROR)

set(PROJECT_NAME Oil-production-optimization)
project(${PROJECT_NAME})

file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
source_group(TREE ${CMAKE_SOURCE_DIR}  FILES ${SOURCES} ${HEADERS})

include_directories(external/rapidjson/include)
include_directories(external/eigen)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
if (MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC /MP)
	target_compile_options(${PROJECT_NAME} PUBLIC /Za)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>/res)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				   COMMAND ${CMAKE_COMMAND} -E copy_directory
				   ${CMAKE_SOURCE_DIR}/visualization $<TARGET_FILE_DIR:${PROJECT_NAME}>/visualization)