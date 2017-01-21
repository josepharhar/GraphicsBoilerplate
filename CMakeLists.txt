# gcc
add_executable(main main.cpp)

# ld
target_link_libraries(main shared_lib)

# -I./hello_library
include_directories(hello_library)
