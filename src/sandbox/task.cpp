#include "task.h"
#include "entity.h"

namespace sandbox {

void RecursiveTask::run(Entity& entity, TaskContext* context) {
    runEntity(entity, context);

    if (context) {
        context->push();
    }
    const std::vector<Entity*>& entities = entity.getChildren();
    for (std::vector<Entity*>::const_iterator it = entities.begin(); it != entities.end(); it++) {
        (*it)->runTask(*this, context);
    }

    if (context) {
        context->pop();
    }
}

}

