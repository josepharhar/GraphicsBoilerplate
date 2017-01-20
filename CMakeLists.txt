# gcc -c
add_library(static_lib STATIC static_lib.cpp)
# gcc -shared
add_library(shared_lib SHARED shared_lib.cpp)

# gcc
add_executable(asdf asdf.cpp)

# ld
target_link_libraries(asdf shared_lib)
