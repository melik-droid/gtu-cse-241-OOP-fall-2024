cmake_minimum_required(VERSION 3.10)
project(visicalc)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")  # Add optimization flag


set(LIBRARY_NAME visicalc_lib)

#Add the lib/src directory as a subdirectory
include_directories(lib/include)



# Define sources for the library 
set(LIB_SOURCES
    lib/src/AnsiTerminal.cpp
    lib/src/Cell.cpp
    lib/src/Sheet.cpp
    lib/src/Cursor.cpp
    lib/src/FileManager.cpp
    lib/src/FormulaParser.cpp
)


#Define the main source file

set(MAIN_SOURCES
    main.cpp
)

#Create the library from the lib sources
add_library(${LIBRARY_NAME} ${LIB_SOURCES})


#Add the main executable
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})


#Link the library to the main executable
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBRARY_NAME})










