cmake_minimum_required(VERSION 3.22)
project(projekt C)


set(CMAKE_C_STANDARD 99)
#set (CMAKE_SYSTEM_VERSION 10.0)
set(CMAKE_C_COMPILER gcc)
add_subdirectory(portaudio)



add_executable(projekt main.c synthesis.c synthesis.h)
target_link_libraries(projekt PUBLIC portaudio_static winmm.lib  )

