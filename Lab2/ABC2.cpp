#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <list>
#include <condition_variable>
#include <atomic>
#include <queue>

using namespace std;

mutex _mutex;
int mcounter = 0;
atomic<int> _atomic = 0;
const int NumTasks = 1024 * 1024;
const int TaskNum = 100;
int ProducerNum[] = { 1, 2, 4 };
int ConsumerNum[] = { 1, 2, 4 };
int QueueSize[] = { 1, 4, 16 };
int sum;


void counter(int task, int *arr, bool sleep) {
    int _counter = 0;
    while (_counter <= NumTasks) {
        if (task == 1.1) {
            _mutex.lock();
            _counter = mcounter++;
            _mutex.unlock();
        }
        else {
            _counter = _atomic++;
        }
        arr[_counter]++;
        if (sleep) 
            this_thread::sleep_for(chrono::nanoseconds(10));
    }
}


void ArrayTask(int task, bool sleep) {
    int* _array = new int[NumTasks];
    int miss;
    int NumThreads[4] = { 4, 8, 16, 32 };
    for (int NumThread : NumThreads) {
        mcounter = _atomic = miss = 0;
        for (int i = 0; i < NumTasks; i++) {
            _array[i] = 0;
        }
        vector<thread> threads;
        auto start = chrono::high_resolution_clock::now();
        for (int j = 0; j < NumThread; j++) {
            threads.emplace_back(counter, task, _array, sleep);
        }
        for (thread& _thread : threads) {
            _thread.join();
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<float> duration = end - start;
        for (int k = 0; k < NumTasks; k++) {
            if (_array[k] != 1)
                miss++;
        }
        cout << NumThread << " threads lasted " << duration.count() << " with " << miss << " misses" << "\n";
    }
    cout << "\n";
}


class Queue
{
private:
    queue<uint8_t> _queue;
public:

    void push(uint8_t val)
    {
        _mutex.lock();
        _queue.push(val);
        _mutex.unlock();
    }

    bool pop(uint8_t& val) {
        _mutex.lock();
        if (_queue.empty()) {
            _mutex.unlock();
            this_thread::sleep_for(chrono::milliseconds(1));
            _mutex.lock();
        }
        if (!_queue.empty()) {
            val = _queue.front();
            _queue.pop();
            _mutex.unlock();
            return true;
        }
        _mutex.unlock();
        return false;
    }
};



class Queue2
{
private:
    queue<uint8_t> _queue;
    int size;
    condition_variable queue_check;
    condition_variable queue_check2;
public:
    explicit Queue2(int size) {
        this->size = size;
    }

    void push(uint8_t val)
    {
        unique_lock<mutex> locker(_mutex);
            queue_check.wait(locker, [this]() { return _queue.size() < size; });
        _queue.push(val);
        queue_check2.notify_one();
    }

    bool pop(uint8_t& val) {
        unique_lock<mutex> locker(_mutex);
        queue_check2.wait_for(locker, chrono::milliseconds(1), [this]() { return !_queue.empty(); });
        
        if (!_queue.empty()) {
            val = _queue.front();
            _queue.pop();
            queue_check.notify_one();
            return true;
        }
        return false;
    }
};



template<typename T>
class Task2 {
public:
    static void producer(T& _queue) {
        for (int i = 0; i < TaskNum; i++) {
            _queue.push(1);
        }
    }


    static void consumer(T& _queue) {
        int counter = 0;
        uint8_t val = 0;
        while (true) {
            if (!_queue.pop(val)) {
                break;
            }
            counter += val;
        }
        sum += counter;
    }


    static void _task(T& _queue) {
        for (int Producer : ProducerNum) {
            for (int Consumer : ConsumerNum) {
                vector<thread> _producers;
                vector<thread> _consumers;
                auto start = chrono::high_resolution_clock::now();
                sum = 0;
                for (int i = 0; i < Producer; i++) {
                    _producers.emplace_back(&Task2<T>::producer, ref(_queue));
                }
                for (int i = 0; i < Consumer; i++) {
                    _consumers.emplace_back(&Task2<T>::consumer, ref(_queue));
                }
                for (thread& _producer : _producers) {
                    _producer.join();
                }
                for (thread& _consumer : _consumers) {
                    _consumer.join();
                }

                auto end = chrono::high_resolution_clock::now();
                chrono::duration<float> duration = end - start;
                cout << "With producers : consumerNum = " << Producer << " : " << Consumer << " threads lasted " << duration.count() << " with result " << sum << "\n";
            }

        }
    }
};



int main()
{
    

    double task = 0;


    cout << "Tasks:\n";
    cout << "1 - array task\n";
    cout << "2.1 - first queue task\n";
    cout << "2.2 - second queue task\n";

    cin >> task;
    cout << "\n";

    if (task == 1) {
        cout << "Mutex" << "\n";
        ArrayTask(1.1, false);
        cout << "Mutex + sleep" << "\n";
        ArrayTask(1.1, true);
        cout << "Atomic" << "\n";
        ArrayTask(1.2, false);
        cout << "Atomic + sleep" << "\n";
        ArrayTask(1.2, true);
    }
    else if (task == 2.1) {
        Queue queue1;
        Task2<Queue>::_task(queue1);
    }
    else {
        for (int num : QueueSize) {
            Queue2 queue2(num);
            cout << "Queue with size " << num << " \n ";
            Task2<Queue2>::_task(queue2);
            cout << "\n";
        }
    }


}

