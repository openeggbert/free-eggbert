cmake_minimum_required(VERSION 3.16)
project(SPEEDY_BLUPI_WINDOWS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Setting Wine compilator (target is Linux)
set(CMAKE_CXX_COMPILER wineg++-stable)
set(CMAKE_C_COMPILER winegcc-stable)
set(CMAKE_CXX_FLAGS "-m64")
#optimizations
set(CMAKE_CXX_FLAGS "-g -O0")
set(CMAKE_CXX_FLAGS "-fpermissive -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

#Disable warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")

include_directories(/usr/include/wine)
link_directories(/usr/lib/wine)

# Setting the output file
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

# Adding source code files
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(SPEEDY_BLUPI_WINDOWS ${SOURCES})

target_include_directories(SPEEDY_BLUPI_WINDOWS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#find_library(DPLAYX_LIBRARY NAMES dplayx PATHS /home/robertvokac/.wine/drive_c/windows/system32)

# Adding Wine libraries
target_link_libraries(SPEEDY_BLUPI_WINDOWS PRIVATE uuid user32 gdi32 ole32 oleaut32 shell32

        ddraw
        dsound
#        /home/robertvokac/.wine/drive_c/windows/system32/dplayx.dll
#        /home/robertvokac/.wine/drive_c/windows/system32/dpnet.dll
        #/home/robertvokac/.wine/drive_c/windows/system32/dpnlobby.dll
)

