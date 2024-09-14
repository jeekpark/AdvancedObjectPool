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
            mStack.push(std::make_unique<T>(std::forward<Args>(args)...));
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
                    mStack.push(std::unique_ptr<T>(obj));
                    ++mLeftObjectCount;
                }
            };

        if (mLeftObjectCount == 0)
        {
            return std::unique_ptr<T, decltype(deleter)>(nullptr, deleter);
        }

        std::unique_ptr<T> obj = std::move(mStack.top());
        mStack.pop();
        --mLeftObjectCount;
        return std::unique_ptr<T, decltype(deleter)>(obj.release(), deleter);
    }

    template <typename... Args>
    void Expand(size_t extraCapacity, Args&&... args)
    {
        mStack.c.reserve(mCapacity + extraCapacity);
        for (size_t i = 0; i < extraCapacity; ++i)
        {
            mStack.push(std::make_unique<T>(std::forward<Args>(args)...));
        }
        mCapacity += extraCapacity;
        mLeftObjectCount += extraCapacity;
    }

    size_t GetLeftObjectCount() const noexcept { return mLeftObjectCount; }

private:

    template<typename T>
    class AccessibleStack : public std::stack<std::unique_ptr<T>, std::vector<std::unique_ptr<T>>>
    {
    public:
        using std::stack<std::unique_ptr<T>, std::vector<std::unique_ptr<T>>>::c;
    };

    size_t mCapacity;
    size_t mLeftObjectCount;
    AccessibleStack<T> mStack;
};