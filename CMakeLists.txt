cmake_minimum_required(VERSION 3.10)
project(chip8)

# Obtain raylib
include(FetchContent)
FetchContent_Declare(
	raylib
	GIT_REPOSITORY	https://github.com/raysan5/raylib.git
	GIT_TAG 		6.0
)
FetchContent_MakeAvailable(raylib)

add_executable(chip8
    "src/main.c"
    "src/chip8.c"
    "src/chip8_op.c"
 )

target_include_directories(chip8 PRIVATE "include/")

target_link_libraries(chip8 PRIVATE raylib)
