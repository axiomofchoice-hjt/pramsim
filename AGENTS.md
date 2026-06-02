# AGENTS.md — pramsim

## Build & Run

```bash
xmake                         # configure + build (debug by default)
xmake config -m release && xmake   # release build
xmake run                     # build + run all examples
./run.sh                      # generate compile_commands.json, build, run (convenience wrapper)
```

Direct compilation (no xmake):

```bash
g++ examples/rank_sort.cpp -Iinclude -std=c++23 -Wall -Wextra -Wpedantic -Werror -o rank_sort
```

## Verify / CI

- No dedicated test suite — **examples are the verification**. Every `xmake run` exercises the examples.
- CI runs `clang-format --dry-run --Werror` on all `*.cpp` and `*.h` files. Commit formatted code.
- CI also runs `run-clang-tidy` against compile_commands.json in `build/`.
- Always build with `-Wall -Wextra -Wpedantic -Werror` (xmake enforces this for examples).

## Formatting & Style

- clang-format: Google style, **4-space indent** (not 2), 100-column limit, `AllowShortFunctionsOnASingleLine: All`
- clang-tidy: `bugprone-*`, `clang-analyzer-*`, `performance-*`, `readability-*`, plus `modernize-use-nullptr`/`override`/`using`. Excludes: `-bugprone-exception-escape`, `-readability-identifier-length`, `-readability-convert-member-functions-to-static`.
- Generate compile_commands.json for tooling: `xmake project -y -k compile_commands build`

## Architecture

- **Header-only** C++23 library. The whole library lives under `include/pramsim/`.
- Main entry point: `#include <pramsim/pramsim.hpp>` (re-exports all sub-headers).
- Key types: `pram::Machine` (the simulator), `pram::SharedArray<T>` (shared memory with conflict detection), `pram::Task` (coroutine return type).
- PRAM models: `pram::EREW`, `pram::CREW`, `pram::CRCW_Add` (and other CRCW variants). Default model is CREW.
- Processor code runs as C++23 coroutines. Round barriers are `co_await pram::step();`.
- Reading shared memory: `array[index]`. Writing: `array.write(index, value)`. Writes are buffered and committed at the next `pram::step()`.
- `pram::Stat` (from `machine.stat()`) tracks `n_processors`, `n_rounds`, `n_reads`, `n_writes`.
- `pram::assert_or_throw(cond, msg)` is used throughout examples; throws `pram::assertion_error`.

## Documentation

- `docs/user_guide*.md` and `docs/api_reference*.md` in both Chinese and English.
- Referenced from `README.md` / `README_EN.md`.
