cmake_minimum_required(VERSION 3.16)
project(WINELIB_DEMO LANGUAGES CXX)

# Setting Wine compilator (target is Linux)
set(CMAKE_CXX_COMPILER wineg++-stable)
set(CMAKE_C_COMPILER winegcc-stable)
set(CMAKE_CXX_FLAGS "-m64")

include_directories(/usr/include/wine)
link_directories(/usr/lib/wine)

# Setting the output file
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Adding source code files
add_executable(WINELIB_DEMO main.cpp)

# Adding Wine libraries
target_link_libraries(WINELIB_DEMO PRIVATE uuid user32 gdi32 ole32 oleaut32 shell32)
