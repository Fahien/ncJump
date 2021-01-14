#include "serialization/config.h"

#include "config.h"

#include "serialization/file.h"

namespace nl = nlohmann;

namespace jmp
{
void to_json(nl::json& j, const Scale& s)
{
    j["window"] = s.window;
    j["global"] = s.global;
    j["scene"] = s.scene;
}

void to_json(nl::json& j, const Extent& e)
{
    j["width"] = e.width;
    j["height"] = e.height;
}

void to_json(nl::json& j, const Size& s)
{
    j["tile"] = s.tile;
    j["window"] = s.window;
}

void to_json(nl::json& j, const Config& c)
{
    j["scale"] = c.scale;
    j["size"] = c.size;
}

void from_json(const nl::json& j, Scale& s)
{
    j["window"].get_to(s.window);
    j["global"].get_to(s.global);
    j["scene"].get_to(s.scene);
}

void from_json(const nl::json& j, Extent& e)
{
    j["width"].get_to(e.width);
    j["height"].get_to(e.height);
}

void from_json(const nl::json& j, Size& s)
{
    j["tile"].get_to(s.tile);
    j["window"].get_to(s.window);
}

void from_json(const nl::json& j, Config& c)
{
    j["scale"].get_to(c.scale);
    j["size"].get_to(c.size);
}

Config Config::from_json(const char* path)
{
    if (nc::fs::exists(path)) {
        auto data = read_file(path);
        auto j = nl::json::parse(data.data(), data.data() + data.size());
        return j.get<Config>();
    }

    return {};
}

void save(const Config& c, const char* file_path)
{
    nl::json j = c;
    auto data = j.dump();
    write_file(file_path, data);
    LOGI_X("Config saved to %s", file_path);
}

} // namespace jmp