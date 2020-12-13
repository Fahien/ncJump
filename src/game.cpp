#include "game.h"

#include <ncine/Application.h>
#include <ncine/FileSystem.h>
#include <ncine/imgui.h>

namespace jmp
{
Game::Game()
    : root {nc::theApplication().rootNode()}
{
    root.setScale({4.0f, 4.0f});
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ScaleAllSizes(2.0f);
    auto& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;
}

void Game::update(const float dt)
{
    ImGui::Begin("Player", nullptr);
    ImGui::Text("state: %s", to_string(entity.getState()));
    ImGui::End();
}

} // namespace jmp
