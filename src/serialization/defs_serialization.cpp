#include "serialization/defs_serialization.h"

namespace jmp
{
OPTION<EntityDef> EntityDef::from_json(const char* path)
{
    if (!nc::fs::exists(PATH(path))) {
        return None;
    }

    LOGI_X("Loading %s", path);
    auto data = read_file(PATH(path));
    auto j = nl::json::parse(data.data(), data.data() + data.size());
    return j.get<EntityDef>();
}

void save(const EntityDef& def, const char* path)
{
    nl::json j = def;
    auto data = j.dump();
    write_file(PATH(path), data);
    LOGI_X("Entity saved to %s", path);
}

} // namespace jmp
