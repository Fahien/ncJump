#pragma once

#include <ncine/IAppEventHandler.h>
#include <ncine/IInputEventHandler.h>
#include <nctl/UniquePtr.h>

#include "game.h"
#include "types.h"

namespace ncine
{
class AppConfiguration;
}

namespace nc = ncine;

/// Jump game event handler
class JumpHandler : public nc::IAppEventHandler, public nc::IInputEventHandler
{
public:
    void onPreInit(nc::AppConfiguration& config) override;

    void onInit() override;

    void onFrameStart() override;

    void onMouseMoved(const nc::MouseState& state) override;
    void onMouseButtonPressed(const nc::MouseEvent& event) override;
    void onMouseButtonReleased(const nc::MouseEvent& event) override;

    void onKeyPressed(const nc::KeyboardEvent& event) override;
    void onKeyReleased(const nc::KeyboardEvent& event) override;

    void onJoyAxisMoved(const nc::JoyAxisEvent& event) override;

    void onJoyMappedButtonPressed(const nc::JoyMappedButtonEvent& event) override;
    void onJoyMappedButtonReleased(const nc::JoyMappedButtonEvent& event) override;

    UNIQUE<jmp::Game> game;
};
