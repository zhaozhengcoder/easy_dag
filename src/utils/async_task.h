#pragma once

#include <iostream>
#include <thread>
#include <memory>
#include <functional>
#include <mutex>

#include "src/utils/thread_pool.h"

namespace easy_dag {
class BackgroundTask {
public:
    virtual ~BackgroundTask() = default;
    virtual void run(Callback callback = nullptr) = 0; 
};

class BackgroundTaskManager {
public:
    static BackgroundTaskManager& getInstance() {
        static BackgroundTaskManager instance;
        return instance;
    }

    void startTask(std::shared_ptr<BackgroundTask> task, Callback callback = nullptr) {
        pool.enqueue([task, callback]() {
            task->run(callback);
        });
    }

    BackgroundTaskManager(const BackgroundTaskManager&) = delete;
    BackgroundTaskManager& operator=(const BackgroundTaskManager&) = delete;

private:
    BackgroundTaskManager(): pool(std::thread::hardware_concurrency()) {
        // LOG(INFO) << "BackgroundTaskManager initialized.";
    }

    ~BackgroundTaskManager() {
        // LOG(INFO) << "BackgroundTaskManager destroyed.";
    }

private:
    easy_dag::ThreadPool pool;
};

}