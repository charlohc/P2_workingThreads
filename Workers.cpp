#include <thread>
#include <vector>
#include <iostream>
#include <functional>
#include <mutex>
#include <list>
#include <condition_variable>

using namespace std;
//TODO: finn ut hvorfor programmet slutter å kjøre etter tråd 1, fordi trenger CV
//TODO: hva er målet?? skal gjøre mange oppgaver paralellt
//TODO:
class Workers {
    int numberOfThreads;
    list<function<void()>> tasks;
    mutex tasks_mutex;
    condition_variable cv;

public:
    Workers(int threads) {
        numberOfThreads = threads;
    };

    void post_tasks() {
        for (int i = 0; i < 3; i++) {
            cout << "task " << i << endl;
            unique_lock<mutex> lock(tasks_mutex);
            tasks.emplace_back([i] {
                cout << "task " << i << " runs in thread " << &this_thread::get_id << " " << endl;
            });
        }
    }
    void start() {
        vector<thread> worker_thread;
        for (int i = 0; i < numberOfThreads; i++) {
            cout << "thread: " << i << endl;
            worker_thread.emplace_back([this] {
                while (true) {
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(tasks_mutex);
                        //cv.wait(lock);
                        //TODO:use conditional variable
                        if(!tasks.empty()) {
                            task = *tasks.begin();
                            tasks.pop_front();
                        }
                    }
                    if (task) {
                        task();
                    }
                }

            });
        }
        //cv.notify_one();
        for (auto &thread: worker_thread)
            if (thread.joinable())
                thread.join();
    }
};

    int main() {
        Workers worker_threads(2);
        Workers event_loop(1);
        worker_threads.post_tasks();
        event_loop.post_tasks();

        worker_threads.start();
        //event_loop.start();


        return 0;
    }
