cmake_minimum_required(VERSION 3.10)
project(chip8)

add_executable(chip8
    "src/main.c"
    "src/chip8.c"
    "src/chip8_op.c"
)

target_include_directories(chip8 PRIVATE "include/")
