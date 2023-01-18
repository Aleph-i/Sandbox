#include "gtest/gtest.h"
#include "sandbox/plugin/plugin_manager.h"
#include "sandbox/interfaces/entity_component_interface.h"
#include "sandbox/entity.h"
#include "sandbox/parameter_set.h"

namespace sandbox {
  
class BaseComponent {
public:
    BaseComponent() : val(0) {
        std::cout << "BaseComponent()" << std::endl;
    }

    void test() {
        std::cout << "BaseComponent::test() " << val << std::endl;
    }

public:
    int val;
};

class TestComponent : public sandbox::Component, public BaseComponent {
public:
    TestComponent() : val(0), version(0) {
        std::cout << "TestComponent()" << std::endl;
        addType<TestComponent>();
        addType<BaseComponent>();
    }

    TestComponent(const ParameterSet& set) : val(0), version(0) {
        std::cout << "TestComponent(ParameterSet set)" << std::endl;
        val = set["val"].get<int>(2);
        addType<TestComponent>();
        addType<BaseComponent>();
    }

    void update() {
        std::cout << "TestComponent::update()" << std::endl;
        version++;
    }

    void again() {
        std::cout << "TestComponent::again() " << val << std::endl;
    }

public:
    int val;
    int version;
};


class EntityComponentTest : public ::testing::Test {
public:
    EntityComponentTest() {
        ec = new EntityComponentInterface();
        pm.addPluginInterface(ec);
        pm.loadPlugin("lib/libtest.so");
        printTask = ec->tasks().create("PrintTask");
    }

    void SetUp() { 
        entity = new Entity("Root");
        entity->addComponent(new TestComponent());
    }

    void TearDown() {
        delete entity;
    }

    void print() {
        entity->runTask(*printTask);
    }

protected:
    PluginManager pm;
    EntityComponentInterface* ec;
    Entity* entity;
    Task* printTask;
};

TEST_F(EntityComponentTest, ComponentAsType) {  
    BaseComponent* bc = entity->getComponent<BaseComponent>();
    TestComponent* tc = entity->getComponent<TestComponent>();
    std::string* str = entity->getComponent<std::string>();

    if (bc) {
        bc->test();
        EXPECT_EQ(bc->val, 0);
    }

    if (tc) {
        tc->again();
        tc->test();
        EXPECT_EQ(tc->val, 0);
    }

    EXPECT_NE(bc, nullptr) << "Base ptr should exist";
    EXPECT_NE(tc, nullptr) << "Dervived should exist";
    EXPECT_EQ(str, nullptr) << "Not a valid type";
}

TEST_F(EntityComponentTest, AddChild) {  
    Entity& test = entity->addChild(new Entity("test"));
    TestComponent* tc = entity->getComponent<TestComponent>();
    EXPECT_EQ(tc->version, 2);
}

TEST_F(EntityComponentTest, AddComponent) {  
    TestComponent* tc = entity->getComponent<TestComponent>();
    EXPECT_EQ(tc->version, 1);

    TestComponent* tc2 = new TestComponent();
    int version = tc2->version;
    entity->addComponent(tc2);
    EXPECT_EQ(tc2->version, version+1);

    EXPECT_EQ(tc->version, 2);
}

TEST_F(EntityComponentTest, AddComponentParams) {  
    TestComponent* tc = new TestComponent(ParameterSet().add("val", 42).add("val2", 42));
    EXPECT_EQ(tc->val, 42);
    delete tc;

    ParameterSet params;
    params.add("val", 52).add("val2", 42);
    tc = new TestComponent(params);
    EXPECT_EQ(tc->val, 52);
    delete tc;
}

}