#include "RenderMesh.h"
#include "EditMesh.h"

#include <axe_render/vertex/Vertex.h>
#include <axe_render/vertex/VertexLayoutManager.h>
#include <axe_render/RenderDevice.h>

namespace axe {

#if 0
#pragma mark ========= RenderMesh_InternalHelper ============
#endif
struct RenderMesh_InternalHelper {
	static bool hasAttr(size_t arraySize, size_t vertexCount) {
		if (arraySize <= 0) return false;
		if (arraySize != vertexCount) {
			AXE_ASSERT(false);
			return false;
		}
		return true;
	}

	/*
		|--offset--|
		| position , uv , color || position , uv , color | ...
		|---------stride--------||---------stride--------| ...
		[        vertex1        ][        vertex2        ] ...
	*/
	template<class DST, class T> inline
	static void copyVertexData(DST* dst
								, size_t vertexCount
								, const VertexLayout::Element& element
								, size_t stride
								, const T* src)
	{
		u8* p = dst + element.offset;

		for (size_t i = 0; i < vertexCount; ++i) {
			*reinterpret_cast<T*>(p) = *src;
			src++;
			p += stride;
		}
	}
}; // RenderMesh_InternalHelper


#if 0
#pragma mark ========= RenderMesh ============
#endif
void RenderMesh::create(const EditMesh& src) {
	using Helper = RenderMesh_InternalHelper;
	clear();

	size_t vertexCount = src.pos.size();
	if (vertexCount <= 0)
		return;

	u8 uvCount			= 0;
	u8 colorCount		= 0;
	u8 normalCount		= 0;
	u8 tangentCount		= 0;
	u8 binormalCount	= 0;

	_primitive = RenderPrimitiveType::Triangles;

	if (Helper::hasAttr(src.color.size(), vertexCount)) colorCount = 1;
	if (Helper::hasAttr(src.normal.size(), vertexCount)) normalCount = 1;
	if (Helper::hasAttr(src.tangent.size(),	vertexCount)) tangentCount = 1;
	if (Helper::hasAttr(src.binormal.size(), vertexCount)) binormalCount = 1;

	for (u8 i = 0; i < EditMesh::kUvCountMax; ++i) {
		if (Helper::hasAttr(src.uv[i].size(), vertexCount))
			uvCount = i + 1;
	}

	using PosType		= remove_extent_t<decltype(src.pos)>::value_type;
	using ColorType		= remove_extent_t<decltype(src.color)>::value_type;
	using UvType		= remove_extent_t<decltype(src.uv)>::value_type;
	using NormalType	= remove_extent_t<decltype(src.normal)>::value_type;

	auto vertexType = VertexTypeUtil::make(
						RenderDataTypeUtil::get<PosType>(),
						RenderDataTypeUtil::get<ColorType>(), colorCount,
						RenderDataTypeUtil::get<UvType>(), uvCount,
						RenderDataTypeUtil::get<NormalType>(), normalCount, tangentCount, binormalCount);

	_vertexLayout = VertexLayoutManager::s_instance()->getLayout(vertexType);
	if (!_vertexLayout) {
		AXE_THROW;
	}

	setSubMeshCount(1);
	_subMeshes[0].create(src);
}

void RenderMesh::createQuad(float w, float h) {
	clear();

	if (w <= 0 || h <= 0)
		return;

	_primitive = RenderPrimitiveType::Triangles;

	EditMesh src;

	int nVertices = 4;
	int nTriangles = 2;

	src.pos.resize(nVertices);
	src.normal.resize(nVertices);
	src.uv[0].resize(nVertices);
	src.color.resize(nVertices);

	src.indices.resize(3 * nTriangles);
	float halfW = w * 0.5f;
	float halfH = h * 0.5f;
	/*
	   0 --- 1
	   !    /!
       !  /  !
	   !/    !
	   2 --- 3
	*/
	src.pos[0].set(-halfW, halfH, 0); src.uv[0][1].set(0,0);
	src.pos[1].set( halfW, halfH, 0); src.uv[0][2].set(0,1);
	src.pos[2].set(-halfW,-halfH, 0); src.uv[0][3].set(1,0);
	src.pos[3].set( halfW,-halfH, 0); src.uv[0][4].set(1,1);

	for (int i = 0; i < nVertices; ++i) {
		src.normal[i].set(0, 0, 0);
		src.color[i].set(255, 255, 255, 255);
	}

	src.indices.assign({
		0, 1, 2,
		2, 1, 3,
	});
}

void RenderMesh::createSphere(float radius, int nU, int nV) {
	clear();

	if (radius <= 0 || nU < 2 || nV < 2)
		return;

	_primitive = RenderPrimitiveType::Triangles;

	static constexpr float PI = Math::PI<float>();
	static constexpr float PI2 = 2.f * PI;

	EditMesh src;
	
	int nVertices  = (nU + 1) * (nV + 1);
	int nTriangles = nU * nV * 2;

	src.pos.resize(nVertices);
	src.normal.resize(nVertices);
	src.tangent.resize(nVertices);
	src.uv[0].resize(nVertices);
	src.color.resize(nVertices);
	src.indices.resize(3 * nTriangles);

	for (int v = 0; v <= nV; ++v)
	{
		for (int u = 0; u <= nU; ++u)
		{
			float theta = float(u) / float(nU) * PI;
			float phi   = float(v) / float(nV) * PI * 2;

			int index = u + (nU + 1) * v;

			Vec3f vertex, tangent, normal;
			Vec2f texCoord;

			// normal
			normal[0] = Math::sin(theta) * Math::cos(phi);
			normal[1] = Math::sin(theta) * Math::sin(phi);
			normal[2] = Math::cos(theta);
			normal.normalize();

			// position
			vertex = normal * radius;

			// tangent
			theta += PI2;
			tangent[0] = Math::sin(theta) * Math::cos(phi);
			tangent[1] = Math::sin(theta) * Math::sin(phi);
			tangent[2] = Math::cos(theta);
			tangent.normalize();

			// texture coordinates
			texCoord[1] = float(u) / float(nU);
			texCoord[0] = float(v) / float(nV);

			src.pos[index] = vertex;
			src.normal[index] = normal;
			src.tangent[index] = tangent;
			src.uv[0][index] = texCoord;
			src.color[index] = Color4b(255,255,255,255);
		}
	}

	int index = 0;
	for (int v = 0; v < nV; ++v)
	{
		for (int u = 0; u < nU; ++u)
		{
			int vindex = u + (nU + 1) * v;

			src.indices[index + 0] = vindex;
			src.indices[index + 1] = vindex + 1;
			src.indices[index + 2] = vindex + 1 + (nU + 1);

			src.indices[index + 3] = vindex;
			src.indices[index + 4] = vindex + 1 + (nU + 1);
			src.indices[index + 5] = vindex + (nU + 1);

			index += 6;
		}
	}

	create(src);
}

void RenderMesh::createRadialGrid(float gridSize, int nU, int nV) {
	clear();

	if (gridSize <= 0 || nU < 1 || nV < 1)
		return;

	_primitive = RenderPrimitiveType::Triangles;

	EditMesh src;

	int nVertices = (nU + 1) * (nV + 1);
	int nTriangles = nU * nV * 2;

	src.pos.resize(nVertices);
	src.normal.resize(nVertices);
	src.tangent.resize(nVertices);
	src.uv[0].resize(nVertices);
	src.color.resize(nVertices);
	src.indices.resize(3 * nTriangles);

	static constexpr float PI = Math::PI<float>();
	static constexpr float PI2 = 2.f * PI;

	for (int v = 0; v <= nV; ++v)
	{
		for (int u = 0; u <= nU; ++u)
		{
			float dx = float(u) / float(nU);
			float dy = float(v) / float(nV);

			float r = gridSize * dx * dx;

			int index = u + (nU + 1) * v;

			Vec3f vertex;
			Vec2f texCoord;

			vertex.x = r * Math::cos(PI2 * dy);
			vertex.y = 0.f;
			vertex.z = r * Math::sin(PI2 * dy);

			texCoord[1] = float(u) / float(nU);
			texCoord[0] = float(v) / float(nV);

			src.pos[index]	   = vertex;
			src.uv[0][index]   = texCoord;
			src.tangent[index] = Tuple3f(0,0,1);
			src.normal[index]  = Tuple3f(0,1,0);
			src.color[index]   = Color4b(255, 255, 255, 255);
		}
	}

	int index = 0;
	for (int v = 0; v < nV; ++v)
	{
		for (int u = 0; u < nU; ++u)
		{
			int vindex = u + (nU + 1) * v;

			src.indices[index++] = vindex;
			src.indices[index++] = vindex + 1;
			src.indices[index++] = vindex + 1 + (nU + 1);

			src.indices[index++] = vindex;
			src.indices[index++] = vindex + 1 + (nU + 1);
			src.indices[index++] = vindex + (nU + 1);
		}
	}

	create(src);
}

void RenderMesh::clear() {
	_vertexLayout = nullptr;
	_subMeshes.clear();
}

void RenderMesh::setSubMeshCount(size_t newSize) {
	size_t oldSize = _subMeshes.size();
	_subMeshes.resize(newSize);
	for (size_t i = oldSize; i < newSize; i++) {
		_subMeshes[i]._mesh = this;
	}
}

#if 0
#pragma mark ========= RenderSubMesh ============
#endif
RenderSubMesh::RenderSubMesh(RenderSubMesh&& r) noexcept
	: _mesh(r._mesh)
	, _indexType(r._indexType)
	, _vertexData(AXE_MOVE(r._vertexData))
	, _indexData(AXE_MOVE(r._indexData))
	, _vertexBuffer(AXE_MOVE(r._vertexBuffer))
	, _indexBuffer(AXE_MOVE(r._indexBuffer))
	, _vertexCount(r._vertexCount)
	, _indexCount(r._indexCount)
	, _boundingBox(r._boundingBox)
{
	r._mesh = nullptr;
	r._indexType = RenderDataType::None;
	r._vertexCount = 0;
	r._indexCount  = 0;
	r._boundingBox.reset();
}

void RenderSubMesh::create(const EditMesh& src) {
	_createVB(src, src.pos.size());
	_createIB(src, src.indices.size());
}

void RenderSubMesh::clear() {
	_vertexBuffer	= nullptr;
	_indexBuffer	= nullptr;
	_vertexCount	= 0;
	_indexCount		= 0;
	_indexType		= RenderDataType::None;
	_vertexData.clear();
	_indexData.clear();
}

void RenderSubMesh::setVertexCount(size_t vc) {
	_vertexCount = vc;
	_vertexData.resize(vertexLayout()->stride * _vertexCount);

	if (_vertexCount > std::numeric_limits<u16>::max()) {
		_indexType = RenderDataType::UInt32;
	} else {
		_indexType = RenderDataType::UInt16;
	}
}

void RenderSubMesh::setIndexCount(size_t ic) {
	using SRC	= RenderDataType;
	using T		= decltype(_indexData)::value_type;

	_indexCount = ic;

	switch (_indexType) {
		case SRC::UInt16: _indexData.resize(ic * sizeof(u16) / sizeof(T)); break;
		case SRC::UInt32: _indexData.resize(ic * sizeof(u32) / sizeof(T)); break;
		default: AXE_THROW_ERROR("unsupported indexType {}", _indexType);
	}
}

void RenderSubMesh::setVertexBuffer(bool isKeepData /*= false*/) {
	_setVertexBuffer(_vertexData);
	if (!isKeepData) _vertexData.clear();
}

void RenderSubMesh::setIndexBuffer(bool isKeepData /*= false*/) {
	_setIndexBuffer(_indexData);
	if (!isKeepData) _indexData.clear();
}

void RenderSubMesh::setIndexData(const Span<const u16> indexData) {
	using SRC = RenderDataType;

	switch (_indexType) {
		case SRC::UInt16: {
			setIndexCount(indexData.size());
			auto byteSpan = ByteSpan_make(indexData);
			_indexData.assign(byteSpan.begin(), byteSpan.end());
			_indexCount = _indexData.size();
		} break;
		case SRC::UInt32: {
			using DST_T = u32;
			using SRC_T = decltype(indexData)::element_type;

			AXE_ASSERT(_vertexCount > UINT16_MAX);
			setIndexCount(indexData.size());
			_setIndexData<DST_T, SRC_T>(indexData);
		} break;
	//---
		default: AXE_THROW_ERROR("setIndexData unsupported indexType");
	}
}

void RenderSubMesh::setIndexData(const Span<const u32> indexData) {
	using SRC = RenderDataType;

	switch (_indexType) {
		case SRC::UInt16: {
			using DST_T = u16;
			using SRC_T = decltype(indexData)::element_type;

			AXE_ASSERT(_vertexCount <= UINT16_MAX);
			setIndexCount(indexData.size());
			_setIndexData<DST_T, SRC_T>(indexData);
		} break;
		case SRC::UInt32: {
			setIndexCount(indexData.size());

			auto byteSpan = ByteSpan_make(indexData);
			_indexData.assign(byteSpan.begin(), byteSpan.end());
		} break;
	//---
		default: AXE_THROW_ERROR("setIndexData unsupported indexType");
	}
}

void RenderSubMesh::_createVB(const EditMesh& src, size_t vc) {
	using Helper = RenderMesh_InternalHelper;

	clear();

	if (vc <= 0)
		return;

	setVertexCount(vc);

	auto* _vertexLayout = vertexLayout();
	auto* pData = _vertexData.data();
	auto stride = _vertexLayout->stride;

	for (auto& e : _vertexLayout->elements) {
		using S	 = VertexSemantic;
		using ST = VertexSemanticType;
		using U	 = VertexSemanticUtil;

		auto semanticType  = U::getType(e.semantic);
		auto semanticIndex = U::getIndex(e.semantic);

		switch (semanticType) {
			case ST::TEXCOORD: {
				if (semanticIndex < EditMesh::kUvCountMax) {
					Helper::copyVertexData(pData, vc, e, stride, src.uv[semanticIndex].data()); break;
				}
				continue;
			} break;
		}

		switch (e.semantic) {
			case S::POSITION: {
//				Helper::copyVertexData(pData, vc, e, stride, src.pos.data());
				const Tuple3f* srcData = src.pos.data();

				u8* dstData = pData + e.offset;
				for (size_t i = 0; i < vc; i++) {
					*reinterpret_cast<Tuple3f*>(dstData) = *srcData;

					_boundingBox.encapsulate(*srcData);

					++srcData;
					dstData += stride;
				}
			} break;
			case S::COLOR0:		Helper::copyVertexData(pData, vc, e, stride, src.color.data()); break;
			case S::NORMAL:		Helper::copyVertexData(pData, vc, e, stride, src.normal.data()); break;
			case S::TANGENT:	Helper::copyVertexData(pData, vc, e, stride, src.tangent.data()); break;
			case S::BINORMAL:	Helper::copyVertexData(pData, vc, e, stride, src.binormal.data()); break;
		}
	}

	setVertexBuffer();
}

void RenderSubMesh::_createIB(const EditMesh& src, size_t ic) {
	if (ic <= 0)
		return;

	auto span = EditMeshUtil::subIndices(src, ic);

	setIndexData(span);
	setIndexBuffer();
}

void RenderSubMesh::_setVertexBuffer(ByteSpan vertexData) {
	AXE_ASSERT(_vertexCount > 0);
	AXE_ASSERT(!vertexData.empty());

	RenderGpuBuffer::CreateDesc desc;
	desc.type		= GpuBufferType::Vertex;
	desc.bufferSize = vertexData.size();

	//_vertexBuffer = SPtr_fromUPtr(Renderer::s_rootDevice()->createGpuBuffer(desc)); TODO
	//_vertexBuffer->uploadToGpu(vertexData); TODO
}

void RenderSubMesh::_setIndexBuffer(ByteSpan indexData) {
	AXE_ASSERT(_indexType != RenderDataType::None);
	AXE_ASSERT(_indexCount > 0);
	AXE_ASSERT(!indexData.empty());

	RenderGpuBuffer::CreateDesc desc;
	desc.type		= GpuBufferType::Index;
	desc.bufferSize = indexData.size();

	//_indexBuffer = SPtr_fromUPtr(Renderer::s_rootDevice()->createGpuBuffer(desc)); TODO
	//_indexBuffer->uploadToGpu(indexData); TODO
}

} // namespace axe