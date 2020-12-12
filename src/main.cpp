#include "main.h"

#include <ncine/AppConfiguration.h>
#include <ncine/Application.h>
#include <ncine/FileSystem.h>

nctl::UniquePtr<nc::IAppEventHandler> createAppEventHandler()
{
    return nctl::makeUnique<JumpHandler>();
}

void JumpHandler::onPreInit(nc::AppConfiguration &config)
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
    game = nctl::makeUnique<jmp::Game>();
}

void JumpHandler::onFrameStart()
{
    game->update(nc::theApplication().interval());
}

void JumpHandler::onKeyReleased(const nc::KeyboardEvent &event)
{
    if (event.sym == nc::KeySym::ESCAPE) {
        LOGI("Bye");
        nc::theApplication().quit();
    }
}
