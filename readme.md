# Header Libraries ![](https://github.com/beached/header_libraries/workflows/MacOS/badge.svg) ![](https://github.com/beached/header_libraries/workflows/Ubuntu/badge.svg) ![](https://github.com/beached/header_libraries/workflows/Windows/badge.svg)

## Description
A set of header only algorithms, data structures, and utilities I use in many of my other projects.  Most should have tests located in the [tests](tests/) folder

## Building
to build
```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=DEBUG ..
cmake --build . --target full --config Debug
```

## Testing
To run unit tests

```bash
ctest -C Debug
```

## Installing

```bash
cmake --install .
```
