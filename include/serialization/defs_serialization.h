#pragma once

#include "model/defs.h"
#include "serialization/file.h"

namespace jmp
{
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(SubGraphicsDef, type, path, rects, loop);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GraphicsDef, subs);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(PhysicsDef, type, scale, dynamic);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(StateDef, dummy);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ScriptDef, type);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EntityDef, type, pos, graphics, physics, state, scripts);

void save(const EntityDef& def, const char* path);

} // namespace jmp
