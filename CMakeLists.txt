cmake_minimum_required(VERSION 3.21)
project(bbb-sht21-multithreaded-http-server VERSION 1.0)
add_compile_definitions(CMAKE_PROJECT_VERSION=${CMAKE_PROJECT_VERSION}) # make version available in c++ files

if(NOT DEFINED BUILD_LOCAL)
    add_custom_target(print-target ALL COMMAND ${CMAKE_COMMAND} -E echo Building for the BBB) # print message when running `cmake --build .`
    set(CMAKE_C_COMPILER arm-linux-gnueabihf-gcc)
    set(CMAKE_CXX_COMPILER arm-linux-gnueabihf-g++)
else()
    add_custom_target(print-target ALL COMMAND ${CMAKE_COMMAND} -E echo Building for the VM) # print message when running `cmake --build .`
    set(ignoreme "${BUILD_LOCAL}") # remove warning "unused variable"
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# See: https://github.com/cpp-best-practices/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
add_compile_options(
    -g3
    -O0
    $<$<CONFIG:RELEASE>:-g0> # overwrites -g3
    $<$<CONFIG:RELEASE>:-O3> # overwrites -O0

    -Wall -Wextra             # reasonable and standard
    -Wshadow                  # warn the user if a variable declaration shadows one from a parent context
    -Wnon-virtual-dtor        # warn the user if a class with virtual functions has a non-virtual destructor. This helps catch hard to track down memory errors
    -Wold-style-cast          # warn for c-style casts
    -Wcast-align              # warn for potential performance problem casts
    -Wunused                  # warn on anything being unused
    -Woverloaded-virtual      # warn if you overload (not override) a virtual function
    -Wconversion              # warn on type conversions that may lose data
    -Wsign-conversion         # warn on sign conversions
    -Wmisleading-indentation  # warn if indentation implies blocks where blocks do not exist
    -Wnull-dereference        # warn if a null dereference is detected
    -Wdouble-promotion        # warn if float is implicit promoted to double
    -Wformat=2                # warn on security issues around functions that format output (ie printf)
    -Weffc++                  # warning mode can be too noisy, but if it works for your project, use it also
    -Wduplicated-cond         # warn if if / else chain has duplicated conditions
    -Wduplicated-branches     # warn if if / else branches have duplicated code
    -Wlogical-op              # warn about logical operations being used where bitwise were probably wanted
    -Wuseless-cast            # warn if you perform a cast to the same type
#   -pedantic                 # warn on language extensions
#   -Werror                   # treat warnings as errors
  )

add_compile_definitions($<$<NOT:$<CONFIG:RELEASE>>:DEBUG>)

# Conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

# Set up VSCode's auto-completion with Conan, see: https://stackoverflow.com/a/70616885/7168774
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Executable
add_executable(http-server src/main.cpp src/RequestHandlerFactory.cpp src/SHT21.cpp)
target_include_directories(http-server PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(http-server CONAN_PKG::poco stdc++fs) # stdc++fs links the <filesystem> library
