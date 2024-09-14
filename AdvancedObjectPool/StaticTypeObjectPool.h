#pragma once

#include <stack>
#include <memory>
#include <stdexcept>
#include <vector>
#include <iterator>


template<typename T>
class StaticTypeObjectPool
{
    
public:
    template <typename... Args>
    StaticTypeObjectPool(size_t capacity, Args&&... args)
        : mCapacity(capacity)
        , mLeftObjectCount(capacity)
    {
        mStack.c.reserve(mCapacity);
        for (size_t i = 0; i < mCapacity; ++i)
        {
            mStack.push(T(std::forward<Args>(args)...));
        }
    }

    ~StaticTypeObjectPool() = default;

    StaticTypeObjectPool(const StaticTypeObjectPool&) = delete;
    StaticTypeObjectPool(StaticTypeObjectPool&&) = delete;
    StaticTypeObjectPool& operator=(const StaticTypeObjectPool&) = delete;
    StaticTypeObjectPool& operator=(StaticTypeObjectPool&&) = delete;
    StaticTypeObjectPool& operator=(std::initializer_list<T>) = delete;

    auto Acquire() noexcept
    {
        auto deleter = [this](T* obj)
            {
                if (obj != nullptr)
                {
                    mStack.push(std::move(*obj));
                    ++mLeftObjectCount;
                }
            };

        if (mLeftObjectCount == 0)
        {
            return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
        }

        auto obj = std::make_unique<T>(std::move(mStack.top()));
        mStack.pop();
        --mLeftObjectCount;
        return std::unique_ptr<T, decltype(deleter)>(obj.release(), deleter);
    }

    size_t GetLeftObjectCount() const noexcept { return mLeftObjectCount; }

private:

    template<typename T>
    class AccessibleStack : public std::stack<T, std::vector<T>>
    {
    public:
        using std::stack<T, std::vector<T>>::c;
    };

    size_t mCapacity;
    size_t mLeftObjectCount;
    AccessibleStack<T> mStack;
};