#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include "array_ptr.h"

struct ReserveProxyObj 
{
    explicit ReserveProxyObj(size_t capacity) : capacity_(capacity) {}
    size_t GetCapacity() const { return capacity_; }
private:
    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) 
{
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
class SimpleVector 
{
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(const ReserveProxyObj& reserve_obj)
        : size_(0), capacity_(reserve_obj.GetCapacity()), items_(reserve_obj.GetCapacity()) {}

    SimpleVector(const SimpleVector& other)
        : size_(other.size_), capacity_(other.capacity_), items_(other.capacity_) 
    {
        std::copy(other.begin(), other.end(), items_.Get());
    }

    SimpleVector(SimpleVector&& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), items_(std::move(other.items_)) 
    {
        other.size_ = 0;
        other.capacity_ = 0;
    }

    SimpleVector& operator=(const SimpleVector& rhs)
    {
        if (this != &rhs) 
        {
            SimpleVector temp(rhs);
            swap(temp);
        }
        return *this;
    }

    SimpleVector& operator=(SimpleVector&& rhs) noexcept 
    {
        if (this != &rhs) 
        {
            size_ = rhs.size_;
            capacity_ = rhs.capacity_;
            items_ = std::move(rhs.items_);
            rhs.size_ = 0;
            rhs.capacity_ = 0;
        }
        return *this;
    }

    void PushBack(const Type& item) 
   {
        if (size_ == capacity_) 
        {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> temp(new_capacity);
            std::move(begin(), end(), temp.Get()); 
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        items_[size_] = item;
        ++size_;
    }

    void PushBack(Type&& item)
    {
        if (size_ == capacity_) 
        {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> temp(new_capacity);
            std::move(begin(), end(), temp.Get()); 
            items_.swap(temp);
            capacity_ = new_capacity;
        }
        items_[size_] = std::move(item);
        ++size_;
    }

    Iterator Insert(ConstIterator pos, const Type& value) 
    {
        auto pos_index = pos - begin();
        if (size_ == capacity_)
        {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> temp(new_capacity);
            std::move(begin(), begin() + pos_index, temp.Get()); 
            temp[pos_index] = value;
            std::move(begin() + pos_index, end(), temp.Get() + pos_index + 1); 
            items_.swap(temp);
            capacity_ = new_capacity;
        } 
        else 
        {
            std::move_backward(begin() + pos_index, end(), end() + 1); 
            items_[pos_index] = value;
        }
        ++size_;
        return begin() + pos_index;
    }

    Iterator Insert(ConstIterator pos, Type&& value) 
     {
        auto pos_index = pos - begin();
        if (size_ == capacity_) 
        {
            size_t new_capacity = (capacity_ == 0) ? 1 : capacity_ * 2;
            ArrayPtr<Type> temp(new_capacity);
            std::move(begin(), begin() + pos_index, temp.Get()); 
            temp[pos_index] = std::move(value);
            std::move(begin() + pos_index, end(), temp.Get() + pos_index + 1); 
            items_.swap(temp);
            capacity_ = new_capacity;
        } 
        else 
        {
            std::move_backward(begin() + pos_index, end(), end() + 1);
            items_[pos_index] = std::move(value);
        }
        ++size_;
        return begin() + pos_index;
    }

    void PopBack() noexcept 
    {
        if (size_ == 0) return;
        --size_;
    }

    Iterator Erase(ConstIterator pos) 
    {
        auto pos_index = pos - begin();
        std::move(begin() + pos_index + 1, end(), begin() + pos_index); 
        --size_;
        return begin() + pos_index;
    }

    void swap(SimpleVector& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        items_.swap(other.items_);
    }

    explicit SimpleVector(size_t size) : size_(size), capacity_(size), items_(size)
    {
        std::fill(items_.Get(), items_.Get() + size_, Type{});
    }

    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size), items_(size)
    {
        std::fill(items_.Get(), items_.Get() + size_, value);
    }

    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()), items_(init.size())
    {
        std::copy(init.begin(), init.end(), items_.Get());
    }

    size_t GetSize() const noexcept
    {
        return size_;
    }

    size_t GetCapacity() const noexcept
    {
        return capacity_;
    }

    bool IsEmpty() const noexcept 
    {
        return size_ == 0;
    }

    Type& operator[](size_t index) noexcept 
    {
        assert(index < size_);
        return items_[index];
    }

    const Type& operator[](size_t index) const noexcept 
    {
        assert(index < size_);
        return items_[index];
    }

    Type& At(size_t index) 
   {
        if (index >= size_) 
        {
            throw std::out_of_range("Index out of range");
        }
        return items_[index];
    }

    const Type& At(size_t index) const 
   {
        if (index >= size_) 
        {
            throw std::out_of_range("Index out of range");
        }
        return items_[index];
    }

    void Clear() noexcept 
    {
        size_ = 0;
    }

   void Resize(size_t new_size) 
   {
     if (new_size <= size_)
     {
        size_ = new_size;
     } 
     else if (new_size <= capacity_) 
     {
       for (size_t i = size_; i < new_size; ++i) 
          {
            new (items_.Get() + i) Type{};  
          }
       size_ = new_size;
     } 
     else 
     {
        size_t new_capacity = std::max(capacity_ * 2, new_size);
        ArrayPtr<Type> temp(new_capacity);
        std::move(items_.Get(), items_.Get() + size_, temp.Get());  
        for (size_t i = size_; i < new_size; ++i) 
        {
            new (temp.Get() + i) Type{};  
        }
        items_.swap(temp);
        size_ = new_size;
        capacity_ = new_capacity;
     }
   }
    
    Iterator begin() noexcept 
    {
        return items_.Get();
    }

    Iterator end() noexcept 
    {
        return items_.Get() + size_;
    }

    ConstIterator begin() const noexcept 
    {
        return items_.Get();
    }

    ConstIterator end() const noexcept 
    {
        return items_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept 
    {
        return items_.Get();
    }

    ConstIterator cend() const noexcept 
    {
        return items_.Get() + size_;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) 
        {
            ArrayPtr<Type> temp(new_capacity);
            std::move(items_.Get(), items_.Get() + size_, temp.Get()); 
            items_.swap(temp);
            capacity_ = new_capacity;
        }
    }

private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    ArrayPtr<Type> items_;
};

// Операторы сравнения
template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs)
{
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) 
{
    return !(rhs > lhs);
}
