#pragma once

#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <memory_resource>
#include <utility>

namespace hw5 {

template <typename T, typename Allocator = std::pmr::polymorphic_allocator<T>>
class pmr_dynamic_array {
public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;

    class iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator() = default;
        explicit iterator(pointer ptr) : ptr_(ptr) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }

        iterator& operator++() {
            ++ptr_;
            return *this;
        }

        iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator& a, const iterator& b) { return a.ptr_ == b.ptr_; }
        friend bool operator!=(const iterator& a, const iterator& b) { return !(a == b); }

    private:
        pointer ptr_{nullptr};
    };

    class const_iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator() = default;
        explicit const_iterator(pointer ptr) : ptr_(ptr) {}
        const_iterator(const iterator& it) : ptr_(it.operator->()) {}

        reference operator*() const { return *ptr_; }
        pointer operator->() const { return ptr_; }

        const_iterator& operator++() {
            ++ptr_;
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        friend bool operator==(const const_iterator& a, const const_iterator& b) { return a.ptr_ == b.ptr_; }
        friend bool operator!=(const const_iterator& a, const const_iterator& b) { return !(a == b); }

    private:
        pointer ptr_{nullptr};
    };

    pmr_dynamic_array() = default;

    explicit pmr_dynamic_array(const allocator_type& alloc) : alloc_(alloc) {}

    pmr_dynamic_array(std::initializer_list<T> init, const allocator_type& alloc = allocator_type())
        : alloc_(alloc) {
        reserve(init.size());
        for (const auto& value : init) {
            emplace_back(value);
        }
    }

    pmr_dynamic_array(const pmr_dynamic_array& other) : alloc_(other.alloc_) {
        reserve(other.size_);
        for (const auto& value : other) {
            emplace_back(value);
        }
    }

    pmr_dynamic_array(pmr_dynamic_array&& other) noexcept
        : alloc_(std::move(other.alloc_)), data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    pmr_dynamic_array& operator=(const pmr_dynamic_array& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        reserve(other.size_);
        for (const auto& value : other) {
            emplace_back(value);
        }
        return *this;
    }

    pmr_dynamic_array& operator=(pmr_dynamic_array&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        clear();
        deallocate();
        alloc_ = std::move(other.alloc_);
        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
        return *this;
    }

    ~pmr_dynamic_array() {
        clear();
        deallocate();
    }

    void push_back(const T& value) { emplace_back(value); }
    void push_back(T&& value) { emplace_back(std::move(value)); }

    template <typename... Args>
    reference emplace_back(Args&&... args) {
        ensure_capacity(size_ + 1);
        alloc_.construct(data_ + size_, std::forward<Args>(args)...);
        ++size_;
        return back();
    }

    void pop_back() {
        if (size_ == 0) {
            return;
        }
        --size_;
        alloc_.destroy(data_ + size_);
    }

    void reserve(size_type new_cap) {
        if (new_cap <= capacity_) {
            return;
        }
        reallocate(new_cap);
    }

    reference operator[](size_type index) { return data_[index]; }
    const_reference operator[](size_type index) const { return data_[index]; }

    reference front() { return data_[0]; }
    const_reference front() const { return data_[0]; }

    reference back() { return data_[size_ - 1]; }
    const_reference back() const { return data_[size_ - 1]; }

    iterator begin() { return iterator(data_); }
    iterator end() { return iterator(data_ + size_); }
    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + size_); }
    const_iterator cbegin() const { return const_iterator(data_); }
    const_iterator cend() const { return const_iterator(data_ + size_); }

    size_type size() const noexcept { return size_; }
    size_type capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }

    void clear() noexcept {
        while (size_ > 0) {
            pop_back();
        }
    }

private:
    void ensure_capacity(size_type desired) {
        if (desired <= capacity_) {
            return;
        }
        size_type new_capacity = capacity_ == 0 ? 1 : capacity_ * 2;
        while (new_capacity < desired) {
            new_capacity *= 2;
        }
        reallocate(new_capacity);
    }

    void reallocate(size_type new_capacity) {
        pointer new_data = alloc_.allocate(new_capacity);
        for (size_type i = 0; i < size_; ++i) {
            alloc_.construct(new_data + i, std::move_if_noexcept(data_[i]));
            alloc_.destroy(data_ + i);
        }
        deallocate();
        data_ = new_data;
        capacity_ = new_capacity;
    }

    void deallocate() {
        if (data_) {
            alloc_.deallocate(data_, capacity_);
            data_ = nullptr;
            capacity_ = 0;
        }
    }

    using pointer = T*;

    allocator_type alloc_{};
    pointer data_{nullptr};
    size_type size_{0};
    size_type capacity_{0};
};

}  // namespace hw5

