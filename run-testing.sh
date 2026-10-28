rm -rf build-macos

cmake -S . -B build-macos
cmake --build build-macos --target afrm_tests

ctest --test-dir build-macos --output-on-failure