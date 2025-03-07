set(USE_PRECOMPILED_HEADERS FALSE)

add_library(
    bsc_controller
    include/GetRPiModel.h
    include/types.hpp
    include/bsc_i2c_interface.hpp
    src/bsc_i2c_interface.cpp
)

target_include_directories(bsc_controller PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(bsc_controller PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(USE_PRECOMPILED_HEADERS)
    target_precompile_headers(bsc_controller PRIVATE <math.h> <unordered_map> <fstream> <stdexcept> <iostream> <string> <vector> <chrono> <thread> <map> <sys/mman.h> <fcntl.h>)
endif()


set_target_properties(bsc_controller PROPERTIES LINKER_LANGUAGE CXX)


