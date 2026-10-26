rm -rf build

cmake -S . -B build
cmake --build build --target afrm_unit_tests

ctest --test-dir build --output-on-failure