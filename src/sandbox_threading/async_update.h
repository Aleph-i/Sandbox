#ifndef SANDBOX_THREADING_ASYNC_UPDATE_H_
#define SANDBOX_THREADING_ASYNC_UPDATE_H_

#include "component.h"
#include "entity.h"
#include <thread>
#include <mutex>
#include <iostream>

namespace sandbox {

class AsyncUpdate : public sandbox::Component {
public:
    AsyncUpdate() : running(true), updateThread(NULL), entity(NULL) {
        addType<AsyncUpdate>();
        addAttribute(new sandbox::TypedAttributeRef<sandbox::Entity*>("entity", entity));
    }

    ~AsyncUpdate() {
        running = false;
        updateThread->join();
        delete updateThread;
    }

    void updateLoop() {
        std::cout << "during" << std::endl;
        while(running) {
            if (entity) {
                entity->update();
            }
        }
    }

    void update() {
        if (!updateThread) {
            updateThread = new std::thread(&AsyncUpdate::updateLoop, this);
        }
    }



private:
    bool running;
    std::thread* updateThread;
    sandbox::Entity* entity;
};

}

#endif