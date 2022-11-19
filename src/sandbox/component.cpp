#include "component.h"
#include "entity.h"

namespace sandbox {

void Component::setEntity(Entity& entity) {
    this->entity = &entity;
}

void Component::updateComponent() {
    update();
}

}