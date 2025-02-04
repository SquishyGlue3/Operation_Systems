#include <pthread.h>
#include <iostream>
#include <thread>

// Singleton abstract base class template
template <typename T>
class SingletonBase {
public:
    // Static method to get the instance of the derived class
    static T* getInstance() {
        pthread_mutex_lock(&mutex_);
        if (instance_ == nullptr) {
            instance_ = new T();
        }
        pthread_mutex_unlock(&mutex_);
        return instance_;
    }

    // Delete copy constructor and assignment operator to prevent copying
    SingletonBase(const SingletonBase&) = delete;
    SingletonBase& operator=(const SingletonBase&) = delete;

protected:
    SingletonBase() = default;
    virtual ~SingletonBase() = default;

private:
    static T* instance_;
    static pthread_mutex_t mutex_;
};

// Static member initialization
template <typename T> T* SingletonBase<T>::instance_ = nullptr;
template <typename T> pthread_mutex_t SingletonBase<T>::mutex_ = PTHREAD_MUTEX_INITIALIZER;

// Concrete Singleton class
class ConcreteSingleton : public SingletonBase<ConcreteSingleton> {
public:
    // Method specific to the concrete Singleton class
    void showMessage() {
        std::cout << "ConcreteSingleton instance!" << std::endl;
    }
};

void threadTask() {
    // Get the Singleton instance and use it
    ConcreteSingleton* singleton = ConcreteSingleton::getInstance();
    singleton->showMessage();
}

int main() {
    std::thread t1(threadTask);
    std::thread t2(threadTask);
    t1.join();
    t2.join();
    return 0;
}



