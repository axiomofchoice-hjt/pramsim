#pragma once

#include <cstdint>

namespace pram {
namespace impl {
enum class ReadPolicy : uint8_t {
    Exclusive,   // Exclusive read
    Concurrent,  // Concurrent read
};

enum class WritePolicy : uint8_t {
    Exclusive,  // Exclusive write
    Common,     // Common write
    Arbitrary,  // Arbitrary write
    Priority,   // Priority write
    Add,        // Combining write (addition)
    Max,        // Combining write (maximum)
    Min,        // Combining write (minimum)
};
}  // namespace impl

struct Model {
    impl::ReadPolicy read_policy;
    impl::WritePolicy write_policy;
};

constexpr Model EREW = {.read_policy = impl::ReadPolicy::Exclusive, .write_policy = impl::WritePolicy::Exclusive};
constexpr Model CREW = {.read_policy = impl::ReadPolicy::Concurrent, .write_policy = impl::WritePolicy::Exclusive};
constexpr Model CRCW_Common = {.read_policy = impl::ReadPolicy::Concurrent, .write_policy = impl::WritePolicy::Common};
constexpr Model CRCW_Arbitrary = {
    .read_policy = impl::ReadPolicy::Concurrent, .write_policy = impl::WritePolicy::Arbitrary};
constexpr Model CRCW_Priority = {
    .read_policy = impl::ReadPolicy::Concurrent, .write_policy = impl::WritePolicy::Priority};
constexpr Model CRCW_Add = {.read_policy = impl::ReadPolicy::Concurrent, .write_policy = impl::WritePolicy::Add};
constexpr Model CRCW_Max = {.read_policy = impl::ReadPolicy::Concurrent, .write_policy = impl::WritePolicy::Max};
constexpr Model CRCW_Min = {.read_policy = impl::ReadPolicy::Concurrent, .write_policy = impl::WritePolicy::Min};
}  // namespace pram
