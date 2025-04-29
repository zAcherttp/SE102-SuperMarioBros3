#pragma once

template <typename EntityType>
class State {
public:
    State() = default;
    virtual ~State() = default;

    virtual void Enter(EntityType* entity) = 0;
    virtual void Exit(EntityType* entity) = 0;
    virtual void Update(EntityType* entity, float dt) = 0;
};