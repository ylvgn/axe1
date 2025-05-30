#include "NativeUI.h"

namespace axe {

NativeUIApp* NativeUIApp::_current = nullptr;

NativeUIApp* NativeUIApp::current() {
	return _current;
}

NativeUIApp::NativeUIApp() noexcept {
	AXE_ASSERT(_current == nullptr);
	_current = this;
}

NativeUIApp::~NativeUIApp() noexcept {
	AXE_ASSERT(_current == this);
	_current = nullptr;
}

} // namespace axe
