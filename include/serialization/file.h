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

template<typename T> void to_json(nl::json& j, const nctl::Array<T>& t)
{
    for (auto& e : t) {
        j.emplace_back(e);
    }
}

template<typename T> void from_json(const nl::json& j, nctl::Array<T>& t)
{
    ASSERT(j.is_array());
    t.setCapacity(u32(j.size()));

    for (auto& e : j) {
        t.emplaceBack(e.get<T>());
    }
}

} // namespace jmp
