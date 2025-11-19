#pragma once

#include <algorithm>
#include <cstddef>
#include <memory_resource>
#include <new>
#include <vector>

namespace hw5 {

class vector_tracking_resource : public std::pmr::memory_resource {
public:
    vector_tracking_resource() = default;

    vector_tracking_resource(const vector_tracking_resource&) = delete;
    vector_tracking_resource& operator=(const vector_tracking_resource&) = delete;

    ~vector_tracking_resource() override {
        for (auto& block : blocks_) {
            if (block.ptr != nullptr) {
                ::operator delete(block.ptr, block.size, std::align_val_t(block.alignment));
                block.ptr = nullptr;
            }
        }
    }

protected:
    void* do_allocate(std::size_t bytes, std::size_t alignment) override {
        auto it = std::find_if(blocks_.begin(), blocks_.end(), [&](const block_info& block) {
            return block.free && block.size >= bytes && block.alignment >= alignment;
        });

        if (it != blocks_.end()) {
            it->free = false;
            return it->ptr;
        }

        void* ptr = ::operator new(bytes, std::align_val_t(alignment));
        blocks_.push_back(block_info{ptr, bytes, alignment, false});
        return ptr;
    }

    void do_deallocate(void* ptr, std::size_t /*bytes*/, std::size_t /*alignment*/) override {
        auto it = std::find_if(blocks_.begin(), blocks_.end(), [&](const block_info& block) {
            return block.ptr == ptr;
        });

        if (it == blocks_.end()) {
            if (ptr) {
                ::operator delete(ptr);
            }
            return;
        }

        it->free = true;
    }

    bool do_is_equal(const std::pmr::memory_resource& other) const noexcept override {
        return this == &other;
    }

private:
    struct block_info {
        void* ptr;
        std::size_t size;
        std::size_t alignment;
        bool free;
    };

    std::vector<block_info> blocks_;
};

}  // namespace hw5

