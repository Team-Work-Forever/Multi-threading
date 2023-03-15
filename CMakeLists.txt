cmake_minimum_required(VERSION 3.0.0)
project(main VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(src)

set(SOURCES
    ${WORKING_DIRECTORY}src/main.c 
    ${WORKING_DIRECTORY}src/linkedlists.c
    ${WORKING_DIRECTORY}src/default_func.c
    ${WORKING_DIRECTORY}src/linkedlists.h 
    ${WORKING_DIRECTORY}src/default_func.h
    ${WORKING_DIRECTORY}src/log_func.h
    ${WORKING_DIRECTORY}src/log_func.c
    ${WORKING_DIRECTORY}src/queue.h
    ${WORKING_DIRECTORY}src/queue.c
)   

add_executable(main ${SOURCES})

if ( MSVC )
    target_compile_options(main PRIVATE /W4)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
