#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include <ncine/FileSystem.h>
#include <nctl/Array.h>

#include "types.h"

namespace nl = nlohmann;

namespace jmp
{
nctl::Array<char> read_file(const char* path);
void write_file(const char* file_path, const std::string& data);

template<typename T> void to_json(nl::json& j, const OPTION<T>& t)
{
    if (t) {
        j = *t;
    } else {
        j = nullptr;
    }
}

template<typename T> void from_json(const nl::json& j, OPTION<T>& t)
{
    if (j.is_null()) {
        t = None;
    } else {
        t = j.get<T>();
    }
}

} // namespace jmp

namespace nctl
{
void to_json(nl::json& j, const String& v);
void from_json(const nl::json& j, String& v);

template<typename T> void to_json(nl::json& j, const VECTOR<T>& t)
{
    for (auto& e : t) {
        j.emplace_back(e);
    }
}

template<typename T> void from_json(const nl::json& j, VECTOR<T>& t)
{
    ASSERT(j.is_array());
    t.setCapacity(u32(j.size()));

    for (auto& e : j) {
        t.emplaceBack(e.get<T>());
    }
}

} // namespace nctl

namespace ncine
{
template<typename T> void to_json(nl::json& j, const Vector2<T>& v)
{
    j["x"] = v.x;
    j["y"] = v.y;
}

template<typename T> void from_json(const nl::json& j, Vector2<T>& v)
{
    j["x"].get_to(v.x);
    j["y"].get_to(v.y);
}

void to_json(nl::json& j, const Recti& v);
void from_json(const nl::json& j, Recti& v);

} // namespace ncine
