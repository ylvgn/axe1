#pragma once

#include <axe_core/base/axe_base.h>

namespace axe {

enum class UIKeyboardEventKeyCode : u32;

class NativeUICommonBase : public StaticClass {
public:
	using KeyCode = UIKeyboardEventKeyCode;
};
AXE_STATIC_ASSERT_NO_MEMBER_CLASS(NativeUICommonBase);

} // namespace axe