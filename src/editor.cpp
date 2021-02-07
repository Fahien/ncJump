#include "editor.h"

#include <ncine/AppConfiguration.h>
#include <ncine/Application.h>
#include <ncine/imgui.h>

#include "component/graphics.h"
#include "game.h"

namespace jmp
{
Editor::Editor(Game& g)
    : game {g}
{
    auto& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.ScaleAllSizes(game.config.scale.gui);
    auto& io = ImGui::GetIO();
    io.FontGlobalScale = game.config.scale.gui;
}

bool active_button(const char* name, bool active)
{
    bool pushed = false;

    if (active) {
        auto& color = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        pushed = true;
    }
    bool ret = ImGui::Button(name);
    if (pushed) {
        ImGui::PopStyleColor();
    }

    return ret;
}

void Editor::update_menu()
{
    if (ImGui::BeginMainMenuBar()) {
        u32 button_count = 2;
        auto text_size = ImGui::CalcTextSize("TileEntity");
        float padding = ImGui::GetStyle().FramePadding.x * (button_count + 1);
        float hwidth = ImGui::GetIO().DisplaySize.x / 2.0f;
        float center = hwidth - padding - text_size.x / 2.0f;
        ImGui::SetCursorPosX(center);

        if (active_button("Tile", mode == Mode::TILE)) {
            if (mode == Mode::TILE) {
                mode = Mode::NONE;
                game.tilemap.entities_root->setAlphaF(1.0f);
            } else {
                mode = Mode::TILE;
                game.tilemap.tiles_root->setAlphaF(1.0f);
                game.tilemap.entities_root->setAlphaF(0.5f);
            }
        }

        if (active_button("Entity", mode == Mode::ENTITY)) {
            if (mode == Mode::ENTITY) {
                mode = Mode::NONE;
                game.tilemap.tiles_root->setAlphaF(1.0f);
            } else {
                mode = Mode::ENTITY;
                game.tilemap.tiles_root->setAlphaF(0.5f);
                game.tilemap.entities_root->setAlphaF(1.0f);
            }
        }

        ImGui::EndMainMenuBar();
    }
}

void update_resolution(Config& config)
{
    nc::theApplication().gfxDevice().setResolution(
        int(config.size.window.width * config.scale.window),
        int(config.size.window.height * config.scale.window));
}

void Editor::update_config(Config& config)
{
    ImGui::Begin("Config");

    if (ImGui::CollapsingHeader("Window")) {
        if (ImGui::SliderFloat("Scale", &config.scale.window, 1.0f, 4.0f, "%.0f")) {
            update_resolution(config);
        }

        u32 window_min = 240;
        u32 window_max = 960;
        if (ImGui::SliderScalarN("Size",
                ImGuiDataType_U32,
                &config.size.window.width,
                2,
                &window_min,
                &window_max)) {
            update_resolution(config);
        }
    }

    if (ImGui::CollapsingHeader("Scene")) {
        ImGui::PushID(&config.scale.scene);
        if (ImGui::SliderFloat("Scale", &config.scale.scene, 1.0f, 4.0f, "%.0f")) {
            game.scene.setScale(config.scale.scene);
        }
        ImGui::PopID();

        u32 tile_min = 8;
        u32 tile_max = 64;
        ImGui::SliderScalarN("Tile", ImGuiDataType_U32, &config.size.tile, 1, &tile_min, &tile_max);

        ImGui::SliderFloat2("Offset", game.camera.offset.data(), -960.0f, 960.0f, "%.0f");
    }

    if (ImGui::CollapsingHeader("Gui")) {
        int new_scale = int(config.scale.gui);
        ImGui::PushID(&new_scale);
        if (ImGui::SliderInt("Scale", &new_scale, 1, 4)) {
            game.config.scale.gui = float(new_scale);
            auto new_style = ImGuiStyle();
            new_style.ScaleAllSizes(game.config.scale.gui);
            ImGui::GetStyle() = new_style;
            ImGui::GetIO().FontGlobalScale = game.config.scale.gui;
        }
        ImGui::PopID();
        ImGui::Checkbox("Demo", &config.toggle.gui_demo);
    }

    ImGui::End();
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

void Editor::update_player(Entity& entity)
{
    ImGui::Begin("Player");

    if (entity.state) {
        update_state(**entity.state);
    }

    if (auto& physics = entity.get_physics()) {
        update_physics(*physics);
    }

    ImGui::End();
}

void Editor::update_input(Input& input)
{
    ImGui::Begin("Input");
    ImGui::Text("mouse.left: {\n\tdown: %s,\n\tpos: { x: %d, y: %d },\n}",
        input.mouse.left.down ? "T" : "F",
        input.mouse.pos.x,
        input.mouse.pos.y);
    ImGui::Text("joystick: {");
    ImGui::Text("\tmove: { x: %f, y: %f },", input.joystick.move.x, input.joystick.move.y);
    ImGui::Text("\ta: { down: %s, just_down: %s },",
        input.joystick.a.down ? "T" : "F",
        input.joystick.a.just_down ? "T" : "F");
    ImGui::Text("\tx: { down: %s, just_down: %s },",
        input.joystick.x.down ? "T" : "F",
        input.joystick.x.just_down ? "T" : "F");
    ImGui::Text("}");
    ImGui::End();
}

ImVec2 get_tile_size(Config& config)
{
    f32 size = config.size.tile * config.scale.gui;
    return {size, size};
}

ARRAY<ImVec2, 2> get_tile_uvs(Tileset& tileset, usize index)
{
    auto& sprite = tileset.sprites[index];
    f32 width = tileset.texture->width();
    f32 height = tileset.texture->height();

    auto uvs = ARRAY<ImVec2, 2>(nctl::StaticArrayMode::EXTEND_SIZE);
    uvs[0] = {sprite->texRect().x / width, sprite->texRect().y / height};
    uvs[1] = {uvs[0].x + sprite->texRect().w / width, uvs[0].y + sprite->texRect().h / height};

    return uvs;
}

void Editor::update_tileset(Tileset& tileset)
{
    static u32 tileset_window_width = 1;

    ImGui::Begin("Tileset");

    auto tile_size = get_tile_size(game.config);

    u32 step = u32(tile_size.x) * 2;
    u32 elements_per_line = tileset_window_width / step - 1;

    // Draw selectable tiles using image buttons
    for (u32 i = 0; i < tileset.sprites.size(); ++i) {
        auto uvs = get_tile_uvs(tileset, i);

        ImGui::PushID(i);
        bool selected = selected_tile == i;
        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_WHITE);
        }
        if (ImGui::ImageButton(tileset.texture->guiTexId(), tile_size, uvs[0], uvs[1])) {
            selected_tile = i;
        }
        if (selected) {
            ImGui::PopStyleColor();
        }
        ImGui::PopID();

        if (elements_per_line && ((i + 1) % elements_per_line)) {
            ImGui::SameLine();
        }
    }

    tileset_window_width = u32(ImGui::GetWindowSize().x);
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

    ImGui::Image(tileset.texture->guiTexId(), tile_size, uvs[0], uvs[1]);

    auto& tile = tileset.tiles[selected_tile];
    ImGui::Text("id: %u", tile.id);
    ImGui::Checkbox("passable", &tile.passable);
    ImGui::Checkbox("destructible", &tile.destructible);

    ImGui::End();
}

void Editor::update_entities(EntityFactory& factory)
{
    ImGui::Begin("Entities");

    for (auto& entity : factory.entities) {
        auto& graphics = CharacterGraphicsComponent::into(*entity->get_graphics());
        graphics.idle.update(ncine::theApplication().interval());
        auto& frame = graphics.idle.animations()[graphics.idle.animationIndex()];
        auto& rect = frame.rect();
        auto size = ImVec2(rect.w * game.config.scale.gui, rect.h * game.config.scale.gui);
        auto& texture = graphics.idle_texture;
        ImVec2 uv[2] = {ImVec2(rect.x / float(texture.width()), rect.y / float(texture.height())),
            ImVec2((rect.x + rect.w) / float(texture.width()),
                (rect.y + rect.h) / float(texture.height()))};
        ImGui::ImageButton(graphics.idle_texture.guiTexId(), size, uv[0], uv[1]);
    }

    ImGui::End();
}

void Editor::place_selected_tile()
{
    // Place selected tile on the map at clicked position
    i32 tile_size = game.config.size.tile * game.config.scale.scene * game.config.scale.global;

    auto camera_pos = game.config.scene_to_screen(game.camera.get_position());

    if (mode == Mode::TILE) {
        Vec2i tile_target = (game.input.mouse.pos - camera_pos) / tile_size;

        if (tile_target.x < game.tilemap.get_width() && tile_target.y < game.tilemap.get_height()) {
            game.tilemap.set_tile(tile_target, game.tileset, game.tileset.tiles[selected_tile]);
        }
    } else if (mode == Mode::ENTITY && game.input.mouse.left.just_down) {
        // Place an object only on mouse left just down
        auto entity_position = Vec2f {f32((game.input.mouse.pos.x - camera_pos.x) / tile_size),
            f32((game.input.mouse.pos.y - camera_pos.y) / tile_size)};
        game.tilemap.set_entity(entity_position, game.tileset, game.tileset.tiles[selected_tile]);
    }
}

void Editor::update_tilemap()
{
    ImGui::Begin("Tilemap");

    i32 dimensions[2] = {i32(game.tilemap.get_width()), i32(game.tilemap.get_height())};
    if (ImGui::DragInt2("Size", dimensions, 1.0f, 0, 64)) {
        game.tilemap.set_dimensions(dimensions[0], dimensions[1]);
    }
    if (ImGui::TreeNode("entities:")) {
        i32 del_num = -1;
        for (i32 i = 0; i < game.tilemap.entities.size(); ++i) {
            std::string entity_num = "Delete " + std::to_string(i);
            if (ImGui::Button(entity_num.c_str())) {
                del_num = i;
            }
        }
        if (del_num >= 0) {
            game.tilemap.entities.erase(std::begin(game.tilemap.entities) + del_num);
        }
        ImGui::TreePop();
    }

    ImGui::End();

    // Do not place any tile if mouse is hovering ImGui
    bool gui_hovered =
        ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered();
    if (mode != Mode::NONE && !gui_hovered && selected_tile >= 0 && game.input.mouse.left.down) {
        place_selected_tile();
    }
}

void Editor::update()
{
    if (game.config.toggle.editor) {
        if (game.config.toggle.gui_demo) {
            ImGui::ShowDemoWindow();
        }

        update_menu();
        update_config(game.config);
        update_player(game.entity);
        update_input(game.input);
        update_tileset(game.tileset);
        update_selected_tile(game.tileset);
        update_entities(game.entity_factory);
        update_tilemap();
    }
}

} // namespace jmp
