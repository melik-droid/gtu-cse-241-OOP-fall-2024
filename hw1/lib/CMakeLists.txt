
set(LIB_SOURCES
    lib/src/AnsiTerminal.cpp
    lib/src/Cell.cpp
    lib/src/Sheet.cpp
    lib/src/Cursor.cpp
    lib/src/FileManager.cpp
    lib/src/FormulaParser.cpp
)

add_library(visicalc_lib ${LIB_SOURCES})

target_include_directories(visicalc_lib PUBLIC 
    CMAKE_CURRENT_SOURCE_DIR/../include
    )