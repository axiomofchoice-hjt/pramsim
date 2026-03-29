#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

#include "memory.hpp"
#include "pramsim/base/assert.hpp"
#include "pramsim/machine/context.hpp"
#include "pramsim/machine/model.hpp"
#include "resolver.hpp"

namespace pram {
template <typename T>
class SharedArray : public Memory {
   public:
    SharedArray(size_t length, T value, Model model, Context* context)
        : data_(length, value), model_(model), context_(context) {}

    SharedArray(std::vector<T> data, Model model, Context* context)
        : data_(std::move(data)), model_(model), context_(context) {}

    size_t size() const { return data_.size(); }

    const std::vector<T>& debug_data() const { return data_; }

    T operator[](size_t index) {
        assert_or_throw(context_->current_pid.has_value(), "Read outside parallel region");
        read_requests_.push_back({.internal_ref = &data_[index], .pid = *context_->current_pid});
        return data_[index];
    }

    void write(size_t index, T value) {
        assert_or_throw(context_->current_pid.has_value(), "Write outside parallel region");
        write_requests_.push_back({.internal_ref = &data_[index], .value = value, .pid = *context_->current_pid});
    }

    void commit_round() override {
        auto key = [](const auto& req) { return std::pair{req.internal_ref, req.pid}; };

        // Sort read requests and deduplicate - a processor reading the same address only counts once
        std::ranges::sort(read_requests_, {}, key);
        read_requests_.erase(std::ranges::unique(read_requests_, {}, key).begin(), read_requests_.end());

        // Sort write requests
        std::ranges::sort(write_requests_, {}, key);

        // Check read-write conflicts
        check_read_write_conflict(read_requests_, write_requests_);

        switch (model_.read_policy) {
            case impl::ReadPolicy::Exclusive:  // Handle exclusive read
                impl::check_exclusive_read(read_requests_);
                break;
            case impl::ReadPolicy::Concurrent:  // Handle concurrent read
                break;
        }

        switch (model_.write_policy) {
            case impl::WritePolicy::Exclusive:  // Handle exclusive write
                impl::check_exclusive_write(write_requests_);
                impl::apply_write(write_requests_);
                break;
            case impl::WritePolicy::Common:  // Handle common write
                impl::check_common_write(write_requests_);
                impl::apply_write(write_requests_);
                break;
            case impl::WritePolicy::Arbitrary:  // Handle arbitrary write
                impl::apply_arbitrary_write(write_requests_, context_);
                break;
            case impl::WritePolicy::Priority:  // Handle priority write
                impl::apply_priority_write(write_requests_);
                break;
            case impl::WritePolicy::Add:  // Handle combining write (addition)
                impl::apply_combining_write(write_requests_, std::plus<T>{});
                break;
            case impl::WritePolicy::Max:  // Handle combining write (maximum)
                impl::apply_combining_write(write_requests_, [](const T& a, const T& b) { return std::max(a, b); });
                break;
            case impl::WritePolicy::Min:  // Handle combining write (minimum)
                impl::apply_combining_write(write_requests_, [](const T& a, const T& b) { return std::min(a, b); });
                break;
        }

        n_reads_ += read_requests_.size();
        n_writes_ += write_requests_.size();

        read_requests_.clear();
        write_requests_.clear();
    }

    size_t n_reads() const override { return n_reads_; }
    size_t n_writes() const override { return n_writes_; }

   private:
    std::vector<T> data_;
    Model model_;
    Context* context_;

    std::vector<impl::ReadRequest<T>> read_requests_;
    std::vector<impl::WriteRequest<T>> write_requests_;

    size_t n_reads_ = 0;
    size_t n_writes_ = 0;
};
}  // namespace pram
