cmake_minimum_required(VERSION 3.14)

project(VKEX)
set(CMAKE_C_STANDARD 99)
add_executable(${PROJECT_NAME} src/main.c)

#           DOWNLOAD ALL THE SUBMODULES
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()




target_include_directories(${PROJECT_NAME} 
        PUBLIC include
        PUBLIC ext/glfw/include
        PUBLIC ext/volk
)

target_link_directories(${PROJECT_NAME}
        PUBLIC ext/glfw/src
)

target_link_libraries(${PROJECT_NAME} glfw volk_headers)

add_subdirectory(ext/glfw)
add_subdirectory(ext/volk)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets/
        ${CMAKE_CURRENT_BINARY_DIR}/assets/)