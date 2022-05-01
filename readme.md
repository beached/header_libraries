# Header Libraries ![](https://github.com/beached/header_libraries/workflows/MacOS/badge.svg) ![](https://github.com/beached/header_libraries/workflows/Ubuntu/badge.svg) ![](https://github.com/beached/header_libraries/workflows/Windows/badge.svg)

## Description
A set of header only algorithms, data structures, and utilities I use in many of my other projects.  Most should have tests located in the [tests](tests/) folder

## Cmake
To use in your projects
```cmake
find_package(daw-header-libraries REQUIRED)
#... 
target_link_libraries( Foo daw::daw-header-libraries )
```

## Cmake and VCPKG
The port name in vcpkg is `daw-header-libraries`.  Once it is installed via vcpkg(system wide/vcpkg.json manifest) it can be used like the cmake section above

## Cmake FetchContent
The library can be used via `FetchContent` in cmake
```cmake
FetchContent_Declare(
        daw_header_libraries
        GIT_REPOSITORY https://github.com/beached/header_libraries.git
        GIT_TAG master
)
#... 
target_link_libraries( Foo daw::daw-header-libraries )
```

## Building
to build directly from git
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
