# uProtocol C++ Zenoh Transport (up-transport-zenoh-cpp)

## Welcome!

This library provides a Zenoh-based uProtocol transport for  C++ uEntities.

*_IMPORTANT NOTE:_ This project is under active development*

This module contains the implementation for the UTransport API in up-cpp.

## Getting Started

### Requirements:
- Compiler: GCC/G++ 11 or Clang 13
- Conan : 1.59 or latest 2.X

#### Conan packages

Using the recipes found in [up-conan-recipes](/gregmedd/up-conan-recipes), build
these Conan packages:

1. [up-core-api](/eclipse-uprotocol/up-spec) - `conan create --version 1.5.8 --build=missing up-core-api/developer`
1. [up-cpp](/eclipse-uprotocol/up-cpp) - `conan create --version 0.2.0 --build=missing up-cpp/developer`
2. [zenoh-c](/eclipse-zenoh/zenoh-c) - `conan create --version 0.11.0.3 zenoh-tmp/developer`

## How to Use the Library

To add up-transport-zenoh-cpp to your conan build dependencies, place following
in your conanfile.txt:

```
[requires]
up-transport-zenoh-cpp/[>=0.2.0]

[generators]
CMakeDeps
CMakeToolchain

[layout]
cmake_layout

```

**NOTE:** If using conan version 1.59 Ensure that the conan profile is
configured to use ABI 11 (libstdc++11: New ABI) standards according to
https://docs.conan.io/en/1.60/howtos/manage_gcc_abi.html

## Building locally

The following steps are only required for developers to locally build and test
up-transport-zenoh-cpp, If you are making a project that uses
up-transport-zenoh-cpp, follow the steps in the _How to Use the Library_ section
above.

### With Conan for dependencies

```
cd up-client-zenoh-cpp
conan install .
cd build
cmake ../ -DCMAKE_TOOLCHAIN_FILE=Release/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . -- -j
```

Once the build completes, tests can be run with `ctest`.

### With dependencies installed as system libraries

**TODO** Verify steps for pure cmake build without Conan.

### Creating the Conan package

See: [up-conan-recipes](/gregmedd/up-conan-recipes)

## Show your support

Give a ⭐️ if this project helped you!
