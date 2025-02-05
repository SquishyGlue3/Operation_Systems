#include <iostream>
#include <pthread.h>
#include <thread>
#include "singleton_new.cpp"
#include "guard_new.cpp"

/*
Explanation:
    Inherits from SingletonBase<ConcreteSingleton> to become a singleton.
    Provides showMessage() as a test function.
*/

// Concrete class inheriting from SingletonBase
class ConcreteSingleton : public SingletonBase<ConcreteSingleton> {
public:
    void showMessage() {
        std::cout << "Singleton instance accessed!" << std::endl;
    }
};

// Function to test the Singleton(only one instance) in a multithreaded environment
void threadTask() {
    ConcreteSingleton* singleton = ConcreteSingleton::getInstance();//Multiple threads execute this simultaneously, but only one instance should be created.
    singleton->showMessage();
}

// Function to test the Guard class
void testGuard() {
    pthread_mutex_t testMutex;
    pthread_mutex_init(&testMutex, nullptr);

    {
        Guard guard(testMutex);  // Lock acquired
        std::cout << "Mutex is locked inside Guard scope." << std::endl;
    }  // Guard goes out of scope, mutex is unlocked(the destructor does it when out of scope)

    std::cout << "Mutex has been released." << std::endl;
    pthread_mutex_destroy(&testMutex);
}

int main() {
    // Test Singleton in multiple threads
    std::thread t1(threadTask);
    std::thread t2(threadTask);
    t1.join();
    t2.join();

    // Test Guard
    testGuard();

    return 0;
}
