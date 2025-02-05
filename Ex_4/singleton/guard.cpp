#include <pthread.h>
#include <iostream>

// Guard class for managing mutex lock and unlock
class Guard {
public:
    // Constructor locks the mutex
    explicit Guard(pthread_mutex_t& mutex) : mutex_(mutex) {
        pthread_mutex_lock(&mutex_);
    }

    // Destructor unlocks the mutex
    ~Guard() {
        pthread_mutex_unlock(&mutex_);
    }

    // Delete copy constructor and assignment operator to prevent copying
    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

private:
    pthread_mutex_t& mutex_;
};

// Example class using Guard for thread-safe operations
class Example {
public:
    Example() : value(0) {
        pthread_mutex_init(&mutex_, nullptr);
    }

    ~Example() {
        pthread_mutex_destroy(&mutex_);
    }

    void increment() {
        Guard guard(mutex_);  // Lock the mutex for the duration of this scope
        ++value;
        std::cout << "Value: " << value << std::endl;
    }

private:
    int value;
    pthread_mutex_t mutex_;
};

int main() {
    Example example;
    example.increment();
    example.increment();

    return 0;
}
