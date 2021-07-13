#include "main.h"

#include <ncine/AppConfiguration.h>
#include <ncine/Application.h>
#include <ncine/FileSystem.h>
#include <nctl/algorithms.h>

#include "serialization/file.h"

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
    return nctl::makeUnique<JumpHandler>();
}

void JumpHandler::onPreInit(nc::AppConfiguration& ncfg)
{
#if defined(__ANDROID__)
    ncfg.dataPath() = "asset::";
#elif defined(__EMSCRIPTEN__)
    ncfg.dataPath() = "/";
#else
#ifdef NCPROJECT_DEFAULT_DATA_DIR
    ncfg.dataPath() = NCPROJECT_DEFAULT_DATA_DIR;
#else
    ncfg.dataPath() = "data/";
#endif
#endif

    ncfg.consoleLogLevel = nc::ILogger::LogLevel::INFO;

    ncfg.windowTitle = "ncJump";
    ncfg.windowIconFilename = "jump48.png";

#ifndef __EMSCRIPTEN__
    // Try setting resolution from config
    auto config = jmp::Config::from_json(PATH("config.json"));
    ncfg.resolution.x = i32(config.scale.window) * i32(config.size.window.width);
    ncfg.resolution.y = i32(config.scale.window) * i32(config.size.window.height);
#endif
}

void JumpHandler::onInit()
{
    auto imini = jmp::read_file(PATH("imgui.ini"));
    ImGui::LoadIniSettingsFromMemory(imini.data(), imini.size());

    auto config = jmp::Config::from_json(PATH("config.json"));
#ifdef __EMSCRIPTEN__
    // Override config window if resolution failed to set
    auto ncfg = nc::theApplication().appConfiguration();
    config.scale.window = 1.0f;
    config.size.window.width = u32(ncfg.resolution.x);
    config.size.window.height = u32(ncfg.resolution.y);
#endif

    game = MK<jmp::Game>(config);
}

void JumpHandler::onFrameStart()
{
    game->update(nc::theApplication().interval());
}

void JumpHandler::onMouseMoved(const nc::MouseState& state)
{
    game->input.mouse.pos.x = state.x;
    game->input.mouse.pos.y = state.y;
}

void JumpHandler::onMouseButtonPressed(const nc::MouseEvent& event)
{
    if (event.isLeftButton()) {
        game->input.mouse.left.down = true;
        game->input.mouse.left.just_down = true;
        game->input.mouse.pos.x = event.x;
        game->input.mouse.pos.y = event.y;
    }
}

void JumpHandler::onMouseButtonReleased(const nc::MouseEvent& event)
{
    if (event.isLeftButton()) {
        game->input.mouse.left.down = false;
    }
}

void JumpHandler::onKeyPressed(const nc::KeyboardEvent& event)
{
    switch (event.sym) {
    case nc::KeySym::W:
        game->input.joystick.move.y += 1.0f;
        break;
    case nc::KeySym::S:
        game->input.joystick.move.y += -1.0f;
        break;
    case nc::KeySym::A:
        game->input.joystick.move.x += -1.0f;
        break;
    case nc::KeySym::D:
        game->input.joystick.move.x += 1.0f;
        break;
    case nc::KeySym::SPACE:
        game->input.joystick.a.down = game->input.joystick.a.just_down = true;
        break;
    case nc::KeySym::ESCAPE:
        game->input.key.esc.down = game->input.key.esc.just_down = true;
        break;
    case nc::KeySym::F12:
        game->input.key.f12.down = game->input.key.f12.just_down = true;
        break;
    default:
        break;
    }

    // Make sure move vector has valid values
    game->input.joystick.move.x = nctl::clamp(game->input.joystick.move.x, -1.0f, 1.0f);
    game->input.joystick.move.y = nctl::clamp(game->input.joystick.move.y, -1.0f, 1.0f);
}

void JumpHandler::onKeyReleased(const nc::KeyboardEvent& event)
{
    switch (event.sym) {
    case nc::KeySym::W:
        game->input.joystick.move.y += -1.0;
        break;
    case nc::KeySym::S:
        game->input.joystick.move.y += 1.0;
        break;
    case nc::KeySym::A:
        game->input.joystick.move.x += 1.0;
        break;
    case nc::KeySym::D:
        game->input.joystick.move.x += -1.0;
        break;
    case nc::KeySym::SPACE:
        game->input.joystick.a.down = false;
        break;
    case nc::KeySym::ESCAPE:
        game->input.key.esc.down = false;
        break;
    case nc::KeySym::F12:
        game->input.key.f12.down = false;
        break;
    default:
        break;
    }

    // Make sure move vector has valid values
    game->input.joystick.move.x = nctl::clamp(game->input.joystick.move.x, -1.0f, 1.0f);
    game->input.joystick.move.y = nctl::clamp(game->input.joystick.move.y, -1.0f, 1.0f);
}

void JumpHandler::onJoyAxisMoved(const nc::JoyAxisEvent& event)
{
    constexpr int X_AXIS = 0;
    constexpr int Y_AXIS = 1;

    if (event.axisId == X_AXIS) {
        game->input.joystick.move.x = event.normValue;
    } else if (event.axisId == Y_AXIS) {
        game->input.joystick.move.y = -event.normValue;
    }

    // Process input
    if (game->input.joystick.move.x != 0 || game->input.joystick.move.y != 0) {
        constexpr f32 TRESHOLD = 0.05f;

        auto length = game->input.joystick.move.length();
        if (length > 1.0f) {
            game->input.joystick.move.normalize();
        } else if (length < TRESHOLD) {
            game->input.joystick.move.x = 0.0;
            game->input.joystick.move.y = 0.0;
        }
    }
}

void JumpHandler::onJoyMappedButtonPressed(const nc::JoyMappedButtonEvent& event)
{
    if (event.buttonName == nc::ButtonName::A) {
        game->input.joystick.a.down = true;
        game->input.joystick.a.just_down = true;
    }

    if (event.buttonName == nc::ButtonName::X) {
        game->input.joystick.x.down = true;
        game->input.joystick.x.just_down = true;
    }
}

void JumpHandler::onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent& event)
{
    if (event.buttonName == nc::ButtonName::A) {
        game->input.joystick.a.down = false;
    }

    if (event.buttonName == nc::ButtonName::X) {
        game->input.joystick.x.down = false;
    }
}
