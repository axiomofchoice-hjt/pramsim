# API Reference

## 1. Machine

Machine is the PRAM machine, used for managing shared memory, context, and scheduling.

### 1.1. Machine::Machine

```cpp
Machine::Machine(size_t n_processors);               // (1)
Machine::Machine(size_t n_processors, Model model);  // (2)
```

(1) Constructs a PRAM machine with n_processors processors and model type CREW.

(2) Constructs a PRAM machine with n_processors processors and the specified model type.

Model types are described in user_guide.md.

### 1.2. Machine::allocate

```cpp
SharedArray<T>& Machine::allocate(size_t length, T value);  // (1)
SharedArray<T>& Machine::allocate(std::vector<T> data);     // (2)
```

(1) Allocates a shared array of length `length` with initial value `value`.

(2) Allocates a shared array with initial values from `data`.

See [SharedArray](#2-sharedarray) for details.

### 1.3. Machine::parallel

```cpp
template <std::invocable<size_t> F>
void Machine::parallel(F&& func);
```

Simulates PRAM parallel programs. `func` is a coroutine that takes a processor id as parameter and returns `pram::Task`.

The PRAM machine starts n_processors processors with ids ranging from 0 to n_processors - 1.

Inside `func`, you can use `co_await pram::step();` for synchronization.

### 1.4. Machine::stat

```cpp
Stat Machine::stat() const;
```

Returns statistical information including number of processors (n_processors), number of rounds (n_rounds), number of memory reads (n_reads), and number of memory writes (n_writes).

Stat is defined as:

```cpp
struct Stat {
    size_t n_processors;
    size_t n_rounds;
    size_t n_reads;
    size_t n_writes;
};
```

## 2. SharedArray

SharedArray is a shared array used for detecting read-write conflicts and committing write operations.

### 2.1. operator[]

```cpp
template <typename T>
T SharedArray<T>::operator[](size_t index);
```

Reads memory at position `index`.

### 2.2. SharedArray::write

```cpp
template <typename T>
void SharedArray<T>::write(size_t index, T value);
```

Writes `value` to memory at position `index`.

### 2.3. SharedArray::size

```cpp
template <typename T>
size_t SharedArray<T>::size() const;
```

Returns the size of the shared array.

### 2.4. SharedArray::debug_data()

```cpp
template <typename T>
const std::vector<T>& SharedArray<T>::debug_data() const;
```

For verification and debugging purposes only. Reading this vector bypasses PRAM checks.
