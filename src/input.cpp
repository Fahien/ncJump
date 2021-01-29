#include "input.h"

namespace jmp
{
void Input::reset()
{
    this->mouse.left.just_down = false;
    this->joystick.a.just_down = false;
    this->joystick.x.just_down = false;
}

} // namespace jmp
