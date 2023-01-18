#ifndef SANDBOX_TASK_H_
#define SANDBOX_TASK_H_

namespace sandbox {

class Entity;

class TaskContext {
public:
    virtual ~TaskContext() {}
    virtual void push() {}
    virtual void pop() {}
};

class Task {
public:
    virtual ~Task() {}
    virtual void run(Entity& entity, TaskContext* context) = 0;
    virtual TaskContext* createContext() { return nullptr; }
};

class RecursiveTask : public Task {
public:
    virtual ~RecursiveTask() {}
    virtual void run(Entity& entity, TaskContext* context);
    virtual void runEntity(Entity& entity, TaskContext* context) = 0;
};





}

#endif