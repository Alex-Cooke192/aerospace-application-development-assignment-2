rmdir coverage-profiles

cmake -S . -B build-coverage \
  -DCMAKE_CXX_COMPILER="$(brew --prefix llvm)/bin/clang++" \
  -DCMAKE_C_COMPILER="$(brew --prefix llvm)/bin/clang"

cmake --build build-coverage --target afrm_tests

mkdir coverage-profiles

LLVM_PROFILE_FILE="$PWD/coverage-profiles/coverage-%p.profraw" \
  ctest --test-dir build-coverage --output-on-failure 

$(brew --prefix llvm)/bin/llvm-profdata merge \
  -sparse coverage-profiles/coverage-*.profraw \
  -o coverage.profdata

  $(brew --prefix llvm)/bin/llvm-cov report \
  build-coverage/afrm_unit_tests \
  -instr-profile=coverage.profdata \
  -ignore-filename-regex='(/tests/|/build-coverage/|/googletest/|/spdlog/)'

  $(brew --prefix llvm)/bin/llvm-cov show \
  build-coverage/afrm_unit_tests \
  -instr-profile=coverage.profdata \
  -ignore-filename-regex='(/tests/|/build-coverage/|/googletest/|/spdlog/)' \
  -format=html \
  -show-branches=count \
  -output-dir=coverage-report

open coverage-report/index.html