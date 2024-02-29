cmake_minimum_required(VERSION 3.8)
project(MiniCRT LANGUAGES C)

# Disable default CRT libraries linking on Windows
add_compile_options(
    "$<$<C_COMPILER_ID:MSVC>:/DWIN32>"
    "$<$<C_COMPILER_ID:MSVC>:/GS->"
    "$<$<C_COMPILER_ID:MSVC>:/RTC->" # 禁用运行时错误检查
)

# Create the minicrt static library
add_library(minicrt STATIC
    entry.c
    malloc.c
    printf.c
    stdio.c
    string.c
 
    rtc_stubs.c # 诡异的RTC运行时检查
)

# Create the test executable
add_executable(test
    test.c
)

# Link the test executable with minicrt.lib and kernel32.lib
# Disable default libraries and set the entry point
target_link_options(test PRIVATE
    "$<$<C_COMPILER_ID:MSVC>:/NODEFAULTLIB>"
    "$<$<C_COMPILER_ID:MSVC>:/entry:mini_crt_entry>"
)
target_link_libraries(test PRIVATE
    minicrt
    kernel32.lib
)
