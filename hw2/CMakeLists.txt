cmake_minimum_required(VERSION 3.10)
project(visicalc)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create the library from source files
add_library(visicalc_lib
    lib/src/AnsiTerminal.cpp
    lib/src/Sheet.cpp
    lib/src/Cursor.cpp 
    lib/src/FileManager.cpp
    lib/src/FormulaParser.cpp
)

# Add include directories for the library
target_include_directories(visicalc_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/include
)

# Create the executable
add_executable(visicalc main.cpp)

# Link the executable with the library
target_link_libraries(visicalc PRIVATE visicalc_lib)

# Set debug flag for Debug build
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
