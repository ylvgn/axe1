#pragma once

#include <axe_core/base/Error.h>

namespace axe {

struct ProjectSettings : public NonCopyable {

	static ProjectSettings* instance();

	void		setProjectRoot(StrView path);
	StrView		projectRoot() const { return _projectRoot; }

	StrView		importedPath() const;

private:
	String _projectRoot;
};

} // namespace axe