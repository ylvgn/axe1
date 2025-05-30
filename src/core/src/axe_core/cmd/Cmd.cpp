#include "Cmd.h"
#include <axe_core/file/Directory.h>

namespace axe {

void Cmd::exec(const Param* params, size_t n) const {
	AXE_ASSERT(!_execFileName.empty());

	auto oldDir = Directory::current();
	bool hasChangeDir = !_workingDir.empty();

	if (hasChangeDir) Directory::setCurrent(_workingDir);
	onExec(params, n);
	if (hasChangeDir) Directory::setCurrent(oldDir);
}

}