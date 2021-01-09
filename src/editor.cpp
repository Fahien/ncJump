#include "editor.h"
#include "game.h"
#include <ncine/imgui.h>

namespace jmp
{
Editor::Editor(Game& g)
    : game {g}
{
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ScaleAllSizes(game.config.scale.scene);
    auto& io = ImGui::GetIO();
    io.FontGlobalScale = game.config.scale.scene;
}

void Editor::update_state(StateComponent& state)
{
    ImGui::Text("state: %s", to_str(*CharacterStateComponent::into(state).state));
}

void Editor::update_physics(PhysicsComponent& physics)
{
    auto vel = physics.body->GetLinearVelocity();
    ImGui::Text("vel: { x: %.2f, y: %.2f }", vel.x, vel.y);

    ImGui::Text("obstacle: { %s }", to_str(physics.obstacle).c_str());

    if (ImGui::TreeNode("Contacts")) {
        for (auto edge = physics.body->GetContactList(); edge; edge = edge->next) {
            auto normal = edge->contact->GetManifold()->localNormal;
            if (edge->contact->GetFixtureA() == physics.body->GetFixtureList()) {
                normal = -normal;
            }
            ImGui::Text("normal: { %.2f, %.2f }", normal.x, normal.y);
        }
        ImGui::TreePop();
    }

    b2MassData mass;
    physics.body->GetMassData(&mass);
    if (ImGui::DragFloat("Mass", &mass.mass, 0.125f)) {
        physics.body->SetMassData(&mass);
    }

    ImGui::DragFloat("Air factor", &physics.air_factor, 1 / 16.0f, 0.0f, 0.0f, "%.4f");
    ImGui::DragFloat("Velocity factor", &physics.velocity_factor, 0.125f);
    ImGui::DragFloat("Jump Y factor", &physics.jump_y_factor);
    ImGui::DragFloat("Jump X factor", &physics.jump_x_factor, 0.125f);
    ImGui::DragFloat("Max X speed", &physics.max_x_speed, 0.125f);
}

void Editor::update_entity(Entity& entity)
{
    ImGui::Begin("Player");

    if (entity.state) {
        update_state(**entity.state);
    }

    if (entity.physics) {
        update_physics(*entity.physics);
    }

    ImGui::End();
}

void Editor::update_input(Input& input)
{
    ImGui::Begin("Input");
    ImGui::Text("mouse.left: {\n\tdown: %s,\n\tpos: { x: %d, y: %d }\n}",
        input.mouse.left.down ? "T" : "F",
        input.mouse.pos.x,
        input.mouse.pos.y);
    ImGui::Text("joystick: {");
    ImGui::Text("\tmove: { x: %f, y: %f },", input.joystick.move.x, input.joystick.move.y);
    ImGui::Text("\ta: { down: %s, just_down: %s }\n}",
        input.joystick.a.down ? "T" : "F",
        input.joystick.a.just_down ? "T" : "F");
    ImGui::End();
}

ImVec2 get_tile_size(Config& config)
{
    f32 size = config.size.tile * config.scale.scene;
    return {size, size};
}

ARRAY<ImVec2, 2> get_tile_uvs(Tileset& tileset, usize index)
{
    auto& sprite = tileset.sprites[index];
    f32 width = tileset.texture.width();
    f32 height = tileset.texture.height();

    auto uvs = ARRAY<ImVec2, 2>(nctl::StaticArrayMode::EXTEND_SIZE);
    uvs[0] = {sprite->texRect().x / width, sprite->texRect().y / height};
    uvs[1] = {uvs[0].x + sprite->texRect().w / width,
        uvs[0].y + sprite->texRect().h / height};

    return uvs;
}

void Editor::update_tileset(Tileset& tileset)
{
    ImGui::Begin("Tileset");

    auto tile_size = get_tile_size(game.config);

    // Draw selectable tiles using image buttons
    for (usize i = 0; i < tileset.sprites.size(); ++i) {
        auto uvs = get_tile_uvs(tileset, i);

        ImGui::PushID(i);
        bool selected = selected_tile == i;
        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_WHITE);
        }
        if (ImGui::ImageButton(tileset.texture.guiTexId(), tile_size, uvs[0], uvs[1])) {
            selected_tile = i;
        }
        if (selected) {
            ImGui::PopStyleColor();
        }
        ImGui::PopID();
        if ((i + 1) % tileset.width) {
            ImGui::SameLine();
        }
    }

    ImGui::End();
}

void Editor::update_selected_tile(Tileset& tileset)
{
    if (selected_tile < 0) {
        return;
    }

    ImGui::Begin("Tile");

    auto tile_size = get_tile_size(game.config);
    auto uvs = get_tile_uvs(tileset, selected_tile);

    ImGui::Image(tileset.texture.guiTexId(), tile_size, uvs[0], uvs[1]);

    auto& tile = tileset.tiles[selected_tile];
    ImGui::Checkbox("Passable", &tile.passable);

    ImGui::End();
}

void Editor::update_tilemap()
{
    ImGui::Begin("Tilemap");
    ImGui::Text("width: %u\nheight: %u", game.tilemap.width, game.tilemap.height);
    ImGui::End();

    // Do not place any tile if mouse is hovering ImGui
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered()) {
        return;
    }
    // Place selected tile on the map at clicked position
    if (selected_tile >= 0 && game.input.mouse.left.down) {
        i32 global_scale = game.config.scale.global;
        i32 tile_size = game.config.size.tile * game.config.scale.scene * global_scale;

        auto screen_camera = Vec2i {i32(game.camera.get_position().x * global_scale),
            i32(game.camera.get_position().y * global_scale)};

        Vec2i tile_target = (game.input.mouse.pos - screen_camera) / tile_size;

        if (tile_target.x < game.tilemap.width && tile_target.y < game.tilemap.height) {
            game.tilemap.set(
                tile_target.x, tile_target.y, game.tileset.create_entity(selected_tile, game));
        }
    }
}

void Editor::update()
{
    update_entity(game.entity);
    update_input(game.input);
    update_tileset(game.tileset);
    update_selected_tile(game.tileset);
    update_tilemap();
}

} // namespace jmp
