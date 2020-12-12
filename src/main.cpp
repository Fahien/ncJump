#include "main.h"

#include <ncine/AppConfiguration.h>
#include <ncine/Application.h>
#include <ncine/FileSystem.h>
#include <nctl/algorithms.h>

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
    return nctl::makeUnique<JumpHandler>();
}

void JumpHandler::onPreInit(nc::AppConfiguration& config)
{
#if defined(__ANDROID__)
    config.dataPath() = "asset::";
#elif defined(__EMSCRIPTEN__)
    config.dataPath() = "/";
#else
#ifdef PACKAGE_DEFAULT_DATA_DIR
    config.dataPath() = PACKAGE_DEFAULT_DATA_DIR;
#else
    config.dataPath() = "data/";
#endif
#endif

    config.consoleLogLevel = nc::ILogger::LogLevel::INFO;

    config.windowTitle = "ncJump";
    config.windowIconFilename = "jump48.png";
    config.resolution.x = 1600;
    config.resolution.y = 800;
}

void JumpHandler::onInit()
{
    game = MK<jmp::Game>();
}

void JumpHandler::onFrameStart()
{
    game->update(nc::theApplication().interval());
}

void JumpHandler::onKeyReleased(const nc::KeyboardEvent& event)
{
    if (event.sym == nc::KeySym::ESCAPE) {
        LOGI("Bye");
        nc::theApplication().quit();
    }
}

void JumpHandler::onJoyAxisMoved(const nc::JoyAxisEvent& event)
{
    constexpr int X_AXIS = 0;
    constexpr int Y_AXIS = 1;

    if (event.axisId == X_AXIS) {
        game->input.move.x = event.normValue;
    } else if (event.axisId == Y_AXIS) {
        game->input.move.y = -event.normValue;
    }

    // Process input
    if (game->input.move.x != 0 || game->input.move.y != 0) {
        constexpr float TRESHOLD = 0.05f;

        auto length = game->input.move.length();
        if (length > 1.0f) {
            game->input.move.normalize();
        } else if (length < TRESHOLD) {
            game->input.move.x = 0.0;
            game->input.move.y = 0.0;
        }
    }
}
