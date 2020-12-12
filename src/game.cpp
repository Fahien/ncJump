#include "game.h"

#include <ncine/Application.h>
#include <ncine/imgui.h>

namespace nc = ncine;

namespace jmp
{
Game::Game()
    : root {&nc::theApplication().rootNode()}
{
    root->setScale({8.0f, 8.0f});
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ScaleAllSizes(2.0f);
    auto& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;
}

void Game::update(const f32 dt)
{
    ImGui::Begin("Player", nullptr);
    ImGui::Text("state: %s", to_string(character.getState()));
    ImGui::End();
}

} // namespace jmp
