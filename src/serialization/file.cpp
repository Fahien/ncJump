#include "serialization/file.h"

#include <ncine/FileSystem.h>
#include <ncine/IFile.h>

namespace nctl
{
void to_json(nl::json& j, const String& v)
{
    auto s = std::string(v.data(), v.length());
    j = s;
}

void from_json(const nl::json& j, String& v)
{
    ASSERT(j.is_string());
    std::string s;
    j.get_to(s);
    v = String(s.c_str());
}

} // namespace nctl

namespace ncine
{
void to_json(nl::json& j, const Recti& v)
{
    j["x"] = v.x;
    j["y"] = v.y;
    j["w"] = v.h;
    j["h"] = v.w;
}

void from_json(const nl::json& j, Recti& v)
{
    j["x"].get_to(v.x);
    j["y"].get_to(v.y);
    j["w"].get_to(v.w);
    j["h"].get_to(v.h);
}

} // namespace ncine

namespace jmp
{
nctl::Array<char> read_file(const char* file_path)
{
    auto file = nc::IFile::createFileHandle(file_path);
    file->open(nc::IFile::OpenMode::READ);
    nctl::Array<char> data;
    data.setSize(file->size());
    file->read(data.data(), file->size());
    return data;
}

void write_file(const char* file_path, const std::string& data)
{
    auto file = nc::IFile::createFileHandle(file_path);
    file->open(nc::IFile::OpenMode::WRITE);
    file->write((void*)data.c_str(), data.size());
}

} // namespace jmp
