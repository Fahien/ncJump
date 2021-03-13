#include "serialization/tilemap.h"

#include "tilemap.h"

#include "serialization/defs_serialization.h"
#include "serialization/file.h"
#include "serialization/tileset.h"

namespace nl = nlohmann;

namespace jmp
{
void to_json(nl::json& j, const Tile& t);
void from_json(const nl::json& j, Tile& t);

void to_json(nl::json& j, const Tilemap& t)
{
    j["width"] = t.get_width();
    j["height"] = t.get_height();
    to_json(j["initial_position"], t.initial_position);
    j["tiles"] = t.tile_descs;
    j["entities"] = t.get_entity_defs();
}

void from_json(const nl::json& j, Tilemap& t)
{
    u32 width = 0;
    u32 height = 0;

    j["width"].get_to(width);
    j["height"].get_to(height);
    if (j.contains("initial_position")) {
        from_json(j["initial_position"], t.initial_position);
    }
    j["tiles"].get_to(t.tile_descs);
    if (j.contains("entities")) {
        j["entities"].get_to(t.get_entity_defs_mut());
    }

    t.set_dimensions(width, height);
}

Tilemap Tilemap::from_json(const char* path)
{
    if (nc::fs::exists(path)) {
        auto data = read_file(path);
        auto j = nl::json::parse(data.data(), data.data() + data.size());
        return j.get<Tilemap>();
    }

    return {};
}

void save(const Tilemap& t, const char* file_path)
{
    nl::json j = t;
    auto data = j.dump();
    write_file(file_path, data);
    LOGI_X("Tilemap saved to %s", file_path);
}

} // namespace jmp
