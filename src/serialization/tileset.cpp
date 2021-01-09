#include "serialization/tileset.h"

#include "tileset.h"

#include "serialization/file.h"

namespace nl = nlohmann;

namespace jmp
{
void to_json(nl::json& j, const Tile& t)
{
    j["passable"] = t.passable;
}

void to_json(nl::json& j, const Tileset& t)
{
    j["tiles"] = t.tiles;
}

void from_json(const nl::json& j, Tile& t)
{
    j["passable"].get_to(t.passable);
}

void from_json(const nl::json& j, Tileset& t)
{
    j["tiles"].get_to(t.tiles);
}

Tile Tile::from_json(const char* path)
{
    if (nc::fs::exists(path)) {
        auto data = read_file(path);
        auto j = nl::json::parse(data.data(), data.data() + data.size());
        return j.get<Tile>();
    }

    return {};
}

Tileset Tileset::from_json(const char* path)
{
    if (nc::fs::exists(path)) {
        auto data = read_file(path);
        auto j = nl::json::parse(data.data(), data.data() + data.size());
        return j.get<Tileset>();
    }

    return {};
}

void save(const Tile& t, const char* file_path)
{
    nl::json j = t;
    auto data = j.dump();
    write_file(file_path, data);
    LOGI_X("Tile saved to %s", file_path);
}

void save(const Tileset& t, const char* file_path)
{
    nl::json j = t;
    auto data = j.dump();
    write_file(file_path, data);
    LOGI_X("Tile saved to %s", file_path);
}

} // namespace jmp
