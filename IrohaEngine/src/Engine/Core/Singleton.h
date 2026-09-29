# pragma once

#include "Base.h"

//明示的に生成、削除ができ、生成順序の逆に削除する事が保証されているスレッドセーフなシングルトンクラス
class SingletonFinalizer {
public:
    using FinalizerFunc = void(*)();
    static void addFinalizer(FinalizerFunc func);
    static void finalize();
};

template <typename T>
class Singleton  {
public:
    static T& GetInstance() {
        std::call_once(initFlag, Create);
        assert(instance);
        return *instance;
    }

private:
    static void Create() {
        instance = new T;
        SingletonFinalizer::addFinalizer(&Singleton<T>::Destroy);
    }
    static void Destroy() {
        delete instance;
        instance = nullptr;
    }



    static std::once_flag initFlag;
    static T* instance;
};

template <typename T> std::once_flag Singleton<T>::initFlag;
template <typename T> T* Singleton<T>::instance = nullptr;
