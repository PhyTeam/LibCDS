#include <iostream>
#include <iomanip>
#include <thread>
#include <queue>
#include <stack>
#include <exception>

#include <unordered_map>
#include <unordered_set>

#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

namespace Concurrency {

using std::make_pair;

class non_copyable
{
private:
    non_copyable(const non_copyable& other) = delete;
    non_copyable& operator= (const non_copyable& other) = delete;
public:
    non_copyable() {}
};

#define THREADS_MUTEX(x)		pthread_mutex_t (x)
#define THREADS_MUTEX_INIT(x)	pthread_mutex_init(x, 0)
#define THREADS_MUTEX_LOCK(x) 	pthread_mutex_lock(x)
#define THREADS_MUTEX_UNLOCK(x)	pthread_mutex_unlock(x)
#define THREADS_MUTEX_DELETE(x) pthread_mutex_destroy(x)

class Mutex : public non_copyable
{
public:
    Mutex() : _mtx()
    {
        THREADS_MUTEX_INIT(&_mtx);
    }

    ~Mutex()
    {
        THREADS_MUTEX_DELETE(&_mtx);
    }

    void Lock()
    {
        THREADS_MUTEX_LOCK(&_mtx);
    }

    void Unlock()
    {
        THREADS_MUTEX_UNLOCK(&_mtx);
    }
private:
    THREADS_MUTEX(_mtx);
};

#define DEFAULT_MUTEX_POLICY Mutex

class Object {};

template<class Host = Object, class MutexPolicy = DEFAULT_MUTEX_POLICY>
class SingleThreaded
{
public:
    struct Lock
    {
        Lock() {}
        explicit Lock(Host&) {}
        explicit Lock(Host*) {}
    };
};

template <class Host = Object, class MutexPolicy = DEFAULT_MUTEX_POLICY>
class ObjectLockable
{
protected:
    struct Lock
    {
    public:
        explicit Lock(Host& host) : host_(host)
        {
            host_.mtx_.Lock();
        }
        explicit Lock(Host* host) : host_(*host)
        {
            host_.mtx_.Lock();
        }

        ~Lock()
        {
            host_.mtx_.Unlock();
        }
    private:
        Host& host_;
        Lock() {}
    };
protected:
    MutexPolicy mtx_;
};

template <class T, template <class Object, class MutexPolicy> class ThreadingModel>
class Container : ThreadingModel<Object, DEFAULT_MUTEX_POLICY>, public Object
{
public:
   using typename ThreadingModel<Object, DEFAULT_MUTEX_POLICY>::Lock;
    void get()
    {
        Lock _lock(this);
    }
};

class invalid_access_exception : public std::exception
{
public:
   virtual const char* what() const throw()
   {
       return "queue empty";
   }
};

template <class K, class T, template<class Object, class MutexPolicy> class ThreadingModel, class _Compare = std::less<K>, class MutexPolicy = DEFAULT_MUTEX_POLICY>
class PriorityQueue : ThreadingModel<Object, MutexPolicy> , public Object
{
private:
    typedef T 			container_value_type;
    typedef T const&	const_value_reference;
    typedef size_t		container_size;
    typedef K			container_key_type;

    typedef std::pair<K, T>	container_type;
    typedef typename std::queue<container_type>	container_single_queue;

    struct PriorityQueueCompare : std::binary_function<container_single_queue, container_single_queue, bool>
    {
        bool operator() (const container_single_queue& __x, const container_single_queue& __y)
        {
            if (__x.empty()) return true;
            else if (__y.empty()) return false;
            else
            {
                return _Compare() (__x.front().first, __y.front().first);
            }
        }
    };

    struct PriorityQueueHash : std::unary_function<container_single_queue, size_t>
    {
        size_t operator() (const container_single_queue& __q)
        {
            return std::hash<K>(__q.front().first);
        }
    };

    using typename ThreadingModel<Object, MutexPolicy>::Lock;
    typedef typename std::unordered_map<container_key_type, container_single_queue> container_set;
    typedef typename std::priority_queue<std::reference_wrapper<container_single_queue>, std::vector<std::reference_wrapper<container_single_queue>>, PriorityQueueCompare> container_mutiple_queue;
    container_mutiple_queue __data;
    container_set 			__set;
    std::priority_queue<container_value_type, std::vector<container_value_type>, _Compare> _data;
public:
    PriorityQueue() {}
    PriorityQueue(const PriorityQueue&) {} // copy constructor

    PriorityQueue& operator=(const PriorityQueue& queue)
    {
        Lock _lock(this);
        (void) queue;
        // TODO: implementation
    }

    bool empty() { Lock _lock(*this); return __data.empty(); }

    container_size size()
    {
        Lock _lock(this);
        size_t size_ = 0;
        for (auto it = __set.begin(); it != __set.end(); ++it)
        {
            size_ += (*it).second.size();
        }
        return size_;
    }
    container_value_type& top()
    {
        Lock _lock(this);
        if (size() == 0) throw invalid_access_exception();
        container_single_queue& reference_ = __data.top();
        if (reference_.empty()) throw invalid_access_exception();
        return reference_.front().second;
    }

    void push(const container_key_type& key, const container_value_type& value)
    {
        Lock _lock(this);
        /* check the value exit in unordered map */
        auto it = __set.find(key);
        if (it != __set.end())
        {
            (*it).second.push(make_pair(key,value));
        }
        else
        {
            __set.insert(make_pair(key, container_single_queue()));
            container_single_queue& ref_ = (*__set.find(key)).second;
            ref_.push(make_pair(key, value));
            __data.push(ref_);
        }
    }

    void pop()
    {
        Lock _lock(this);
        /* get the single queue refernce */
        container_single_queue& reference_ = __data.top();
        container_type& top_ = reference_.front();
        container_key_type key = top_.first;
        reference_.pop();
        if (reference_.empty())
        {
            auto it = __set.find(key);
            __set.erase(it);
            __data.pop();
        }
    }

    void clear()
    {
        Lock _lock(this);
        while (!__data.empty()) __data.pop();
        __set.clear();
    }
};

}

#endif // PRIORITYQUEUE_H

