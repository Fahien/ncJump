#pragma once

#include "model/defs.h"

namespace jmp
{
class Entity;

class Script
{
public:
    static UNIQUE<Script> from(const ScriptDef& def);

    virtual ~Script() = default;

    virtual void update(Entity& entity) = 0;

    virtual UNIQUE<Script> clone() = 0;
};

class WanderingScript : public Script
{
public:
    void update(Entity& entity) override;

    inline UNIQUE<Script> clone() override;

    Direction::Value direction = Direction::LEFT;
};

inline UNIQUE<Script> WanderingScript::clone() {
    return MK<WanderingScript>(*this);
}

} // namespace jmp
