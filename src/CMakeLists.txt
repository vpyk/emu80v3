project(Emu80SDL)

# CMAKE_ASM_NASM_FLAGS since v. 2.8.4
cmake_minimum_required(VERSION 2.8.4)

set(CMAKE_CXX_FLAGS "-O0")

SET (CMAKE_ASM_NASM_OBJECT_FORMAT "elf")
ENABLE_LANGUAGE (ASM_NASM)
# SET (CMAKE_ASM_NASM_FLAGS "-Dlinux")

# Check for SDL
find_package(SDL REQUIRED)
if(NOT SDL_FOUND)
    message(FATAL_ERROR "SDL required")
endif()

include_directories(${SDL_INCLUDE_DIR})

SET (emu80_SOURCES
  emuinit.cpp
  io_vi53.cpp
  main.cpp
  menus.cpp
  misc.cpp
  scrdraw.cpp
  sdllayer.cpp
  sound.cpp
  i8080.cpp
  i8080_hal.cpp
  emucore.asm
  emuio.asm
  emukeyb.asm
  emumisc.asm
  emurom.asm
  interf.asm
  )

add_executable(Emu80SDL ${emu80_SOURCES})
target_link_libraries(Emu80SDL ${SDL_LIBRARY})
