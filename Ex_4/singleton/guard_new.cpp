#include <pthread.h>

/*Explanation:
Constructor (Guard(pthread_mutex_t& mutex)):
    Accepts a mutex reference and locks it immediately upon object creation.

Destructor (~Guard()):
    Automatically unlocks the mutex when the object goes out of scope.

Copy constructor and assignment operator are deleted:
    This prevents accidental copying, which could lead to multiple objects trying to manage the same mutex.

Why Use This?
    Guarantees that mutex unlocks properly, even if an exception occurs.
    Prevents forgetting to unlock, which could lead to deadlocks.
*/

// Guard class for managing mutex lock and unlock
class Guard {
public:
    explicit Guard(pthread_mutex_t& mutex) : mutex_(mutex) {
        pthread_mutex_lock(&mutex_);
    }

    ~Guard() {
        pthread_mutex_unlock(&mutex_);
    }

    // Prevent copying
    Guard(const Guard&) = delete;
    Guard& operator=(const Guard&) = delete;

private:
    pthread_mutex_t& mutex_;
};
