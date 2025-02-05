#include <pthread.h>

/*
Explanation:
getInstance() (Thread-Safe Access)
    Locks the mutex before accessing instance_.
    If instance_ == nullptr, it creates a new instance.
    Unlocks the mutex before returning.

Deleted copy constructor and assignment operator
    Ensures only one instance exists.

static pthread_mutex_t mutex_
    Protects instance creation to avoid race conditions in multithreading.
*/

// Singleton abstract base class template
template <typename T>
class SingletonBase {
public:
    static T* getInstance() {
        pthread_mutex_lock(&mutex_);
        if (instance_ == nullptr) {
            instance_ = new T();
        }
        pthread_mutex_unlock(&mutex_);
        return instance_;
    }

    // Prevent copying
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
