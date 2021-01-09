#include "serialization/tilemap.h"

#include "tilemap.h"

#include "serialization/file.h"
#include "serialization/tileset.h"

namespace nl = nlohmann;

namespace jmp
{
void to_json(nl::json& j, const Tile& t);
void from_json(const nl::json& j, Tile& t);

void to_json(nl::json& j, const Tilemap& t)
{
    j["width"] = t.width;
    j["height"] = t.height;
    j["tiles"] = t.tiles;
}

void from_json(const nl::json& j, Tilemap& t)
{
    j["width"].get_to(t.width);
    j["height"].get_to(t.height);
    j["tiles"].get_to(t.tiles);
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
