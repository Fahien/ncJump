#include "editor.h"

#include <ncine/AppConfiguration.h>
#include <ncine/Application.h>
#include <ncine/imgui.h>

#include "command/command.h"
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
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // Enable Docking
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

void Editor::reset()
{
    selected_tile = None;
    selected_entity = None;
    set_mode(Mode::NONE);
}

void Editor::set_mode(Mode new_mode)
{
    f32 tiles_alpha = 1.0f;
    f32 entities_alpha = 1.0f;

    switch (new_mode) {
    case Mode::TILE:
        entities_alpha = 0.5f;
        break;
    case Mode::ENTITY:
        tiles_alpha = 0.5;
        break;
    default:
        break;
    }

    game.tilemap.tiles_root->setAlphaF(tiles_alpha);
    game.tilemap.entities_root->setAlphaF(entities_alpha);

    mode = new_mode;
}

void Editor::set_selected_tile(OPTION<u32> s)
{
    selected_tile = s;
    selected_entity = None;
}

void Editor::set_selected_entity(OPTION<u32> s)
{
    selected_entity = s;
    selected_tile = None;
}

void Editor::update_menu()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    window_flags |= ImGuiWindowFlags_NoBackground;
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiID dockspace_id = ImGui::GetID("DockId");
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpace(dockspace_id, {0.0f, 0.0f}, dockspace_flags);

    if (ImGui::BeginMenuBar()) {
        u32 button_count = 2;
        auto text_size = ImGui::CalcTextSize("TileEntity");
        float padding = ImGui::GetStyle().FramePadding.x * (button_count + 1);
        float hwidth = ImGui::GetIO().DisplaySize.x / 2.0f;
        float center = hwidth - padding - text_size.x / 2.0f;
        ImGui::SetCursorPosX(center);

        if (active_button("Tile", mode == Mode::TILE)) {
            set_mode(mode == Mode::TILE ? Mode::NONE : Mode::TILE);
        }

        if (active_button("Entity", mode == Mode::ENTITY)) {
            set_mode(mode == Mode::ENTITY ? Mode::NONE : Mode::ENTITY);
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
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
        ImGui::Text("x: %.0f, y: %.0f", game.scene.x, game.scene.y);
        ImGui::PushID(&config.scale.scene);
        if (ImGui::SliderFloat("Scale", &config.scale.scene, 1.0f, 4.0f, "%.0f")) {
            game.scene.setScale(config.scale.scene);
        }
        ImGui::PopID();

        u32 tile_min = 8;
        u32 tile_max = 64;
        ImGui::SliderScalarN("Tile", ImGuiDataType_U32, &config.size.tile, 1, &tile_min, &tile_max);

        f32 map_len = game.tilemap.get_width() * game.config.size.tile * game.config.scale.scene;
        ImGui::SliderFloat2("Offset", game.camera.offset.data(), -map_len, map_len, "%.0f");
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
    }

    ImGui::End();
}

void Editor::update_camera(Camera& camera, Config& config)
{
    ImGui::Begin("Camera");
    ImGui::Text("offset: { %.2f, %.2f }", camera.offset.x, camera.offset.y);
    auto pos = camera.get_position();
    ImGui::Text("pos: { %.2f, %.2f }", pos.x, pos.y);
    auto screen = config.scene_to_screen(pos);
    ImGui::Text("screen: { %d, %d }", screen.x, screen.y);
    ImGui::Text("target: { %.2f, %.2f }", camera.target.x, camera.target.y);
    ImGui::End();
}

void Editor::update_state(StateComponent& state)
{
    ImGui::Text("state: %s", to_str(CharacterStateComponent::into(state).get_state()));
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

    bool enabled = entity.is_enabled();
    if (ImGui::Checkbox("enabled:", &enabled)) {
        entity.set_enabled(enabled);
    }

    for (const auto& command : entity.get_commands()) {
        ImGui::Text("%s", command->to_str().data());
    }

    auto pos = entity.get_position();

    f32 max = f32(game.tilemap.get_width() * game.config.size.tile);
    if (ImGui::DragFloat2("pos:", &pos.x, 1.0f, 0.0f, max, "%.2f")) {
        entity.set_position(pos, game.config);
    }

    if (entity.state) {
        update_state(*entity.state);
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

ARRAY<ImVec2, 2> get_tile_uvs(Tileset& tileset, u32 index)
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
            set_selected_tile(i);
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
    if (!selected_tile) {
        return;
    }

    ImGui::Begin("Tile");

    auto tile_size = get_tile_size(game.config);
    auto uvs = get_tile_uvs(tileset, *selected_tile);

    ImGui::Image(tileset.texture->guiTexId(), tile_size, uvs[0], uvs[1]);

    auto& tile = tileset.tiles[*selected_tile];
    ImGui::Text("id: %u", tile.id);
    ImGui::Checkbox("passable", &tile.passable);
    ImGui::Checkbox("destructible", &tile.destructible);

    ImGui::End();
}

void Editor::update_entities(EntityFactory& factory)
{
    ImGui::Begin("Entities");

    for (u32 i = 0; i < factory.entities.size(); ++i) {
        auto& entity = factory.entities[i];
        auto& graphics = CharacterGraphicsComponent::into(*entity->get_graphics());
        graphics.idle.update(ncine::theApplication().interval());
        auto& frame = graphics.idle.animations()[graphics.idle.animationIndex()];
        auto& rect = frame.rect();
        auto size = ImVec2(rect.w * game.config.scale.gui, rect.h * game.config.scale.gui);
        auto texture = const_cast<nc::Texture*>(graphics.idle.texture());
        ImVec2 uv[2] = {ImVec2(rect.x / float(texture->width()), rect.y / float(texture->height())),
            ImVec2((rect.x + rect.w) / float(texture->width()),
                (rect.y + rect.h) / float(texture->height()))};

        bool selected = selected_entity == i;
        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32_WHITE);
        }

        if (ImGui::ImageButton(texture->guiTexId(), size, uv[0], uv[1])) {
            set_selected_entity(i);
            set_mode(Mode::ENTITY);
        }

        if (selected) {
            ImGui::PopStyleColor();
        }
    }

    if (selected_entity) {
        ImGui::Text("Selected: %u", *selected_entity);
    }

    ImGui::End();
}

/// @brief Place selected tile on the map at clicked position
void Editor::place_selected_tile()
{
    auto mouse_scene_pos = game.config.screen_to_scene(game.input.mouse.pos);
    auto camera_scene_pos = game.camera.get_position();

    if (mode == Mode::TILE) {
        // Adjust mouse position to get center of tile
        auto tile_index =
            into<Vec2i>((mouse_scene_pos + camera_scene_pos) / f32(game.config.size.tile));

        if (tile_index.x < i32(game.tilemap.get_width()) &&
            tile_index.y < i32(game.tilemap.get_height())) {
            game.tilemap.set_tile(tile_index, game.tileset, game.tileset.tiles[*selected_tile]);
        }
    } else if (mode == Mode::ENTITY && game.input.mouse.left.just_down) {
        // Center of entity will be at mouse pos
        mouse_scene_pos.x -= game.config.size.tile / 2.0f;
        mouse_scene_pos.y -= game.config.size.tile / 2.0f;
        auto entity_position = mouse_scene_pos + camera_scene_pos;
        game.tilemap.add_entity_from_tile(
            entity_position, game.tileset, game.tileset.tiles[*selected_tile]);
    }
}

void Editor::place_selected_entity()
{
    auto entity = game.entity_factory.entities[*selected_entity]->clone();
    auto pos = game.config.screen_to_scene(game.input.mouse.pos) + game.camera.get_position();
    entity->set_position(pos, game.config);
    game.tilemap.add_entity(MV(entity));
}

void Editor::update_collisions(Tilemap& tilemap)
{
    ImGuiWindowFlags full_flags = ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::SetNextWindowPos(ImVec2());
    ImGui::Begin("Full", nullptr, full_flags);
    auto win = game.config.get_real_window_size();
    auto size = ImVec2(f32(win.width), f32(win.height));
    ImGui::InvisibleButton("canvas", size);
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    auto draw_list = ImGui::GetWindowDrawList();
    draw_list->PushClipRect(p0, p1);

    // Draw debug shapes for physics objects
    for (auto& entity : tilemap.entities) {
        if (auto& physics = entity->get_physics()) {
            auto fixture_list = physics->body->GetFixtureList();
            for (auto fixture = fixture_list; fixture; fixture = fixture->GetNext()) {
                auto shape = fixture->GetShape();
                assert(shape);
                auto entity_pos = entity->get_position();
                auto pos = game.config.scene_to_gui(entity_pos);
                auto scene_pos = game.config.scene_to_screen(game.camera.get_position());
                auto htile = game.config.size.tile * game.config.scale.scene / 2.0f;
                pos.x -= scene_pos.x - htile;
                pos.y += scene_pos.y - htile;

                // Draw a circle at entity position
                if (shape->GetType() == shape->e_circle) {
                    draw_list->AddCircle(pos, htile, IM_COL32_WHITE);
                } else {
                    auto p2 = ImVec2(pos.x + htile, pos.y - htile);
                    pos.x -= htile;
                    pos.y += htile;
                    draw_list->AddRect(pos, p2, IM_COL32_WHITE);
                }
            }
        }
    }

    draw_list->PopClipRect();
    ImGui::End(); // Full
}

void update_size(Tilemap& tilemap)
{
    // When trying to shrink the tilemap, show a warning before losing data
    static bool show_modal_warning = false;
    static u32 new_dimensions[2] = {0, 0};

    const u32 width = tilemap.get_width();
    const u32 height = tilemap.get_height();
    u32 dimensions[2] = {width, height};
    const u32 min = 16;
    const u32 max = 256;
    if (ImGui::DragScalarN("Size", ImGuiDataType_U32, dimensions, 2, 1.0f, &min, &max, "%u")) {
        if (dimensions[0] < width || dimensions[1] < height) {
            new_dimensions[0] = dimensions[0];
            new_dimensions[1] = dimensions[1];
            show_modal_warning = true;
        } else {
            tilemap.set_dimensions(dimensions[0], dimensions[1]);
        }
    }

    if (show_modal_warning) {
        ImGui::OpenPopup("Warning");
        if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text(
                "Shrinking the tilemap will destroy some tiles.\n"
                "Are you sure you want to continue?");
            if (ImGui::Button("Yes")) {
                tilemap.set_dimensions(new_dimensions[0], new_dimensions[1]);
                show_modal_warning = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("No")) {
                show_modal_warning = false;
            }
            ImGui::EndPopup();
        }
    }
}

OPTION<usize> Editor::update_entity(usize i, Entity& entity)
{
    OPTION<usize> ret = None;

    auto name = nctl::String().format("%zu", i);
    if (ImGui::TreeNode(name.data())) {
        if (ImGui::Button("Delete")) {
            ret = i;
        }
        ImGui::TreePop();
    }

    return ret;
}

void Editor::update_tilemap(Tilemap& tilemap)
{
    ImGui::Begin("Tilemap");

    f32 max = f32(tilemap.get_width() * game.config.size.tile);
    ImGui::DragFloat2("initial pos: ", tilemap.initial_position.data(), 1.0f, 0.0f, max, "%.0f");

    ImGui::Text("pos { x: %.0f, y: %.0f }", tilemap.node->x, tilemap.node->y);

    update_size(tilemap);

    if (ImGui::TreeNode("entities:")) {
        OPTION<usize> to_delete = None;
        for (usize i = 0; i < tilemap.entities.size(); ++i) {
            if (auto del = update_entity(i, *tilemap.entities[i])) {
                to_delete = del;
            }
        }
        if (to_delete) {
            tilemap.entities.erase(std::begin(tilemap.entities) + *to_delete);
        }
        ImGui::TreePop();
    }

    ImGui::Checkbox("Show bodies", &show_bodies);

    ImGui::End();

    if (show_bodies) {
        update_collisions(tilemap);
    }

    // Do not place any tile if mouse is hovering ImGui
    bool gui_hovered =
        ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered();
    if (mode != Mode::NONE && !gui_hovered && game.input.mouse.left.just_down) {
        if (selected_tile) {
            place_selected_tile();
        } else if (mode == Mode::ENTITY && selected_entity) {
            place_selected_entity();
        }
    }
}

void Editor::update()
{
    if (game.input.key.f12.just_down) {
        game.config.toggle.editor = !game.config.toggle.editor;
    }

    if (game.config.toggle.editor) {
        if (game.input.key.esc.just_down) {
            reset();
        }

        update_menu();

        if (game.config.toggle.gui_demo) {
            ImGui::ShowDemoWindow();
        }

        update_config(game.config);
        update_camera(game.camera, game.config);
        update_player(game.entity);
        update_input(game.input);
        update_tileset(game.tileset);
        update_selected_tile(game.tileset);
        update_entities(game.entity_factory);
        update_tilemap(game.tilemap);
    }
}

} // namespace jmp
