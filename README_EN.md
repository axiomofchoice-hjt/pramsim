# pramsim - A PRAM Simulator for Parallel Algorithm Experiments

pramsim is a PRAM (Parallel Random Access Machine) simulator designed for parallel complexity experiments, model verification, and educational demonstrations.

## Features

1. **Modern build system and syntax**: Xmake, C++23, Coroutines.
2. **Lightweight and header-only**: Easy to integrate into projects.
3. **Education-friendly**: Easy to use with comprehensive PRAM model support.

## 1. What is PRAM?

PRAM is a common theoretical model used in parallel algorithm research. In this model, a large number of processors execute synchronously and share a block of random-access memory.

Computation proceeds in rounds. In each round, processors can perform computations and read/write memory. At the end of each round, global synchronization occurs before proceeding to the next round. Thus, PRAM is inherently synchronous.

PRAM can be classified based on rules for reading/writing the same memory address:

1. **EREW (Exclusive Read Exclusive Write)**: The same address cannot be read or written simultaneously.
2. **CREW (Concurrent Read Exclusive Write)**: The same address can be read concurrently but cannot be written concurrently.
3. **CRCW (Concurrent Read Concurrent Write)**: The same address can be read and written concurrently. Additional strategies exist for write conflicts, such as CRCW_Add where concurrent writes to the same address are automatically summed.

Parallel algorithm analysis typically considers two metrics: time complexity and processor count. For example, $`O(\log n)`$ time with $`O(n)`$ processors.

## 2. Example

Array circular right shift: The program starts n processors, each reading one array element, then writing it to the right position in the next round.

`co_await pram::step();` represents a round barrier. At this point, read-write conflict detection is performed uniformly, and write operations are committed.

```cpp
#include <pramsim/pramsim.hpp>
#include <vector>

int main() {
    constexpr size_t n = 8;
    pram::Machine machine{n, pram::EREW};
    auto& array = machine.allocate<int>(std::vector<int>{0, 1, 2, 3, 4, 5, 6, 7});

    machine.parallel([&](size_t pid) -> pram::Task {
        int value = array[pid];
        co_await pram::step();
        array.write((pid + 1) % n, value);
    });
}
```

More examples:

1. `examples/tree_sum.cpp` - Array summation
2. `examples/prefix_sum.cpp` - Prefix sum
3. `examples/rank_sort.cpp` - Rank Sort
4. `examples/bitonic_sort.cpp` - Bitonic sort (variant)
5. `examples/list_ranking.cpp` - List ranking

## 3. Quick Start

### 3.1. Prerequisites

- A C++23 compatible compiler (gcc / clang)
- (Optional) Xmake build tool

### 3.2. Building with Xmake (Recommended)

Clone the repository:

```bash
git clone https://github.com/axiomofchoice-hjt/pramsim.git
```

Enter the project directory and build:

```bash
xmake

# You can also specify build mode
# xmake config -m release && xmake
```

After building, run all example programs:

```bash
xmake run
```

To install the pramsim library:

```bash
xmake install pramsim

# You can also specify installation path
# xmake install --installdir=/path/to/install pramsim
```

### 3.3. Direct Compilation

Clone the repository:

```bash
git clone https://github.com/axiomofchoice-hjt/pramsim.git
```

Enter the project directory and compile:

```bash
g++ examples/rank_sort.cpp -Iinclude -std=c++23 -o rank_sort
```

After compilation, run the program:

```bash
./rank_sort
```

## 4. Documentation

[User Guide](docs/user_guide_EN.md)

[API Reference](docs/api_reference_EN.md)

[Chinese Documentation](README.md)
