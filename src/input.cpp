#include "input.h"

namespace jmp
{
void Input::reset()
{
    mouse.left.just_down = false;
    joystick.a.just_down = false;
    joystick.x.just_down = false;
    key.esc.just_down = false;
    key.f12.just_down = false;
}

} // namespace jmp
