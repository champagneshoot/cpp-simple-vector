#pragma once

#include <cassert>
#include <cstdlib>
#include <utility>

template <typename Type>
class ArrayPtr 
{
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() noexcept = default;

    // Создаёт в куче массив из size элементов типа Type.
    explicit ArrayPtr(size_t size)
    {
        if (size == 0)
        {
            raw_ptr_ = nullptr;
        }
        else
        {
            raw_ptr_ = new Type[size];
        }
    }

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept
        : raw_ptr_(raw_ptr){}

    ArrayPtr(ArrayPtr&& other) noexcept
    {
        swap(other);
        //other.raw_ptr_ = nullptr;
    }

    ~ArrayPtr()
    {
        delete[] raw_ptr_;
    }

    // Оператор присваивания перемещением
    ArrayPtr& operator=(ArrayPtr&& other) noexcept
    {
        if (this != &other)
        {
            ArrayPtr tmp(std::move(other));
            swap(tmp);
        }
        return *this;
    }

    ArrayPtr(const ArrayPtr&) = delete;
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept
    {
        Type* temp = raw_ptr_;
        raw_ptr_ = nullptr;
        return temp;
    }

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept
    {
        return raw_ptr_[index];
    }

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept
    {
        return raw_ptr_[index];
    }

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const noexcept
    {
        return raw_ptr_ != nullptr;
    }

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept
    {
        return raw_ptr_;
    }

    // Обменивается значениями указателя на массив с объектом other
    void swap(ArrayPtr& other) noexcept
    {
        //using std::swap;
        std::swap(raw_ptr_, other.raw_ptr_);
    }

private:
    Type* raw_ptr_ = nullptr; // указатель на массив в динамической памяти 
};
