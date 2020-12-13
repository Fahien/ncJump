#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>
#include <ncine/imgui.h>

namespace jmp
{
Game::Game()
    : root {nc::theApplication().rootNode()}
    , scene {&root}
    , entity {scene}
    , resource {PATH("img/tile/tileset.png")}
    , tileset {resource, 16}
    , tilemap {scene, tileset}
{
    resource.setMagFiltering(nc::Texture::Filtering::NEAREST);
    root.setScale({4.0f, 4.0f});
    scene.setScale({2.0f, 2.0f});

    auto& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ScaleAllSizes(2.0f);
    auto& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;
}

void Game::update(const f32 dt)
{
    // Update game state
    entity.move(input.move);

    // Gui here
    ImGui::Begin("Player", nullptr);
    ImGui::Text("state: %s", to_string(entity.getState()));
    ImGui::End();

    ImGui::Begin("Input");
    ImGui::Text("left: {\n\tdown: %s,\n\tpos: { x: %d, y: %d }\n}",
        input.left.down ? "T" : "F",
        input.left.pos.x,
        input.left.pos.y);
    ImGui::Text("move: { x: %f, y: %f }", input.move.x, input.move.y);
    ImGui::End();

    ImGui::Begin("Tileset");

    auto tile_size = ImVec2(tileset.tile_size * 2.0f, tileset.tile_size * 2.0f);
    for (usize i = 0; i < tileset.tiles.size(); ++i) {
        auto& tile = tileset.tiles[i];

        float width = resource.width();
        float height = resource.height();

        ImVec2 uvs[2] = {};
        uvs[0] = {tile->texRect().x / width, tile->texRect().y / height};
        uvs[1] = {uvs[0].x + tile->texRect().w / width, uvs[0].y + tile->texRect().h / height};

        ImGui::PushID(i);
        bool focused = selected_tile == i;
        if (focused) {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_WHITE);
        }
        if (ImGui::ImageButton(resource.guiTexId(), tile_size, uvs[0], uvs[1])) {
            LOGI_X("Selecting tile %lu", i);
            selected_tile = i;
        }
        if (focused) {
            ImGui::PopStyleColor();
        }
        ImGui::PopID();
        if ((i + 1) % tileset.width) {
            ImGui::SameLine();
        }
    }

    if (selected_tile >= 0 && input.left.down) {
        auto x = input.left.pos.x / int(tile_size.x) / 4;
        auto y = input.left.pos.y / int(tile_size.y) / 4;
        LOGI_X("Tile %d %d", x, y);
        if (x < tilemap.width && y < tilemap.height) {
            tilemap.set(x, y, tileset.create_tile(selected_tile));
        }
    }

    ImGui::End();

    // Tilemap
    ImGui::Begin("Tilemap");
    ImGui::Text("width: %u\nheight: %u", tilemap.width, tilemap.height);
    ImGui::End();
}

} // namespace jmp
