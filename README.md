# winelib-demo

## How to install WineLib (Target=native Linux)

```bash
sudo apt install wine wine-dev
apt install libwine-dev libjack-dev

```

## How to build via console

```bash
cmake -B build && cmake --build build
```

## How to set CLION

File > Settings > Build, Execution, Deployment > CMake

Add CMake profile and add:

```
-DCMAKE_CXX_COMPILER=wineg++-stable
-DCMAKE_C_COMPILER=winegcc-stable
-DCMAKE_CXX_FLAGS="-m64"
```

#### Edit Run configuration:

Executable=/usr/bin/wine
Arguments={PATH}/winelib-demo/cmake-build-debug/bin/WINELIB_DEMO.exe 
