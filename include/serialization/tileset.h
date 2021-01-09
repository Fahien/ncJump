#pragma once

namespace jmp
{
class Tile;
class Tileset;

void save(const Tile& c, const char* file_path);
void save(const Tileset& c, const char* file_path);

} // namespace jmp
