#pragma once

#include "Vertex.h"

namespace axe {

class VertexLayoutManager : public NonCopyable {
public:
	static VertexLayoutManager* s_instance();

	VertexLayoutManager();

	const VertexLayout* getLayout(VertexType type);

	template<class VERTEX>
	void registerLayout() {
		VertexLayout* layout = _createLayout(VERTEX::kType);
		layout->type = VERTEX::kType;

		AXE_TODO("ImGui_AXE not support now, but how ???");
#if 0 // ImGui_AXE not support now, but how ???
		layout->stride = Math::nextPow16(sizeof(VERTEX));
		AXE_ASSERT(layout->stride % 16 == 0);
#else
		layout->stride = sizeof(VERTEX);
#endif
		VERTEX::s_onRegister(layout);
	}

private:
	VertexLayout* _createLayout(VertexType type);
	Map<VertexType, VertexLayout> _table;
}; // VertexLayoutManager

} // namespace axe