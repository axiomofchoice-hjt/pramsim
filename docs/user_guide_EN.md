# User Guide

## 1. Basic Structure

1. Create a Machine using `pram::Machine machine{...};`.
2. Create several SharedArrays using `machine.allocate(...)`.
3. Start the parallel program using `machine.parallel(...)`.

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

## 2. Model Types

The PRAM model describes strategies for reading/writing the same address. You can use constants like `pram::CREW` and pass them as the second parameter to the Machine constructor.

pramsim supports the following models:

| Model | Read Strategy | Write Strategy | Description |
| :--- | :--- | :--- | :--- |
| EREW | Exclusive | Exclusive Write | Most restrictive model |
| CREW | Concurrent | Exclusive Write | Commonly used model |
| CRCW_Common | Concurrent | Common Write | Can only write the same value simultaneously |
| CRCW_Arbitrary | Concurrent | Arbitrary Write | Randomly writes one value |
| CRCW_Priority | Concurrent | Priority Write | Lower-numbered processor has priority |
| CRCW_Add | Concurrent | Merge Write (Add) | Values are summed |
| CRCW_Max | Concurrent | Merge Write (Max) | Maximum value is taken |
| CRCW_Min | Concurrent | Merge Write (Min) | Minimum value is taken |

## 3. Read/Write Operations

SharedArray provides read/write operations:

- `array[index]` reads memory at index position, taking effect immediately.
- `array.write(index, value)` writes value to memory at index position, but does not take effect immediately; it only takes effect at the synchronization point.

Note: SharedArray provides `array.debug_data()` which returns a vector. This is only for verification and debugging purposes; reading this vector bypasses PRAM checks.

## 4. Synchronization Point

C++ coroutines provide an elegant way to suspend functions using co_await:

```cpp
co_await pram::step();
```

After all processors reach the synchronization point:

1. Read-write conflict detection is performed. pramsim checks whether program behavior is legal according to the model type.
2. Write operations are committed. Calls to `array.write` do not write immediately; they only write at the synchronization point.

## 5. Debug Output

Example:

```cpp
machine.parallel([&](size_t pid) -> pram::Task {
    array.write(pid, pid);
    co_await pram::step();
    if (pid == 0) {
        std::println("debug: {}", str(array.debug_data()));
    }
});
```

As shown above, to view values after write operations take effect, it's recommended to print after the synchronization point `co_await pram::step();`. `if (pid == 0)` ensures printing only once.

The `str` implementation is located in `examples/str.hpp`.

## 6. Error Handling

pramsim may internally throw `pram::assertion_error` exceptions, which should be caught in the outer layer using try-catch. For example:

```cpp
int main() try {
    /* ... */
} catch (const pram::assertion_error& e) {
    std::println("Assertion error: {}", e.what());
    return 1;
}
```

Possible error messages:

- `Read-write conflict: read and write to the same address` - Simultaneous read and write to the same address.
- `Read conflict: exclusive read to the same address` - Exclusive to EREW model, different processors reading the same address simultaneously.
- `Write conflict: exclusive write to the same address` - EREW or CREW model, different processors writing to the same address simultaneously.
- `Write conflict: common write with different values` - Exclusive to CRCW_Common model, different processors writing different values to the same address simultaneously.
- `Read outside parallel region` - Reading shared memory outside the parallel region.
- `Write outside parallel region` - Writing shared memory outside the parallel region.
