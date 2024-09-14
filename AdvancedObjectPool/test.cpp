#include "StaticTypeObjectPool.h"
#include <iostream>

class Test
{
public:
    Test() = default;

    Test(int a)
        : mValue(a)
    {
    }
    ~Test()
    {
    }

    int GetValue() const
    {
        return mValue;
    }
private:
    int mValue;
};

int main()
{
    StaticTypeObjectPool<Test> pool(1, 27);
    std::cout << "Pool Created" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl << std::endl;

    auto obj1 = pool.Acquire();
    std::cout << "Acquire Object 1" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl << std::endl;

    auto obj2 = pool.Acquire();
    std::cout << "Acquire Object 2" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl;
    std::cout << "Obj2 address: " << obj2.get() << std::endl << std::endl;

    obj1.reset();
    std::cout << "Release Object 1" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl << std::endl;

    auto obj3 = pool.Acquire();
    std::cout << "Acquire Object 3" << std::endl;
    std::cout << "Pool Left Object: " << pool.GetLeftObjectCount() << std::endl;
    std::cout << "Obj3 address: " << obj3.get() << std::endl << std::endl;

}