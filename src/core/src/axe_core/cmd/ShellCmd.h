#pragma once

#include "Cmd.h"

namespace axe {

class ShellCmd : public Cmd {
	using Base = Cmd;
public:
	ShellCmd() : Base(Base::Type::Shell) {}
protected:
	virtual void onExec(const Param* params, size_t n) const override;
};

} // namespace axe