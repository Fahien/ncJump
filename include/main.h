#pragma once

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>

#include "game.h"
#include <nctl/UniquePtr.h>

namespace ncine
{
class AppConfiguration;
}

namespace nc = ncine;

/// Jump game event handler
class JumpHandler : public nc::IAppEventHandler, public nc::IInputEventHandler
{
public:
    void onPreInit(nc::AppConfiguration &config) override;

    void onInit() override;

    void onFrameStart() override;

    void onKeyReleased(const nc::KeyboardEvent &event) override;

    nctl::UniquePtr<jmp::Game> game;
};
