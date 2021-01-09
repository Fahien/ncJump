#include "serialization/file.h"

#include <ncine/FileSystem.h>
#include <ncine/IFile.h>

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
