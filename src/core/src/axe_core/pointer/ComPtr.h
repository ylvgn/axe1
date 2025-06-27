#pragma once

namespace axe {

template<class T>
class ComPtr : public NonCopyable {
public:
	ComPtr() = default;
	ComPtr(const ComPtr& r)	noexcept { reset(r._p); }
	ComPtr(ComPtr && r)		noexcept { _p = r.detach(); }

	~ComPtr() noexcept { reset(nullptr); }

	void operator=(const ComPtr& r) noexcept { reset(r._p); }
	void operator=(ComPtr&& r)		noexcept { reset(nullptr); _p = r.detach(); }

		  T* operator->()			noexcept	{ return _p; }
	const T* operator->()	const	noexcept	{ return _p; }

	AXE_NODISCARD operator       T*()				noexcept	{ return _p; }
	AXE_NODISCARD operator const T*()		const	noexcept	{ return _p; }

	AXE_NODISCARD 		T* ptr()				noexcept	{ return _p; }
	AXE_NODISCARD const	T* ptr()		const	noexcept	{ return _p; }

	void reset(T* p) noexcept {
		if (p == _p) return;
		if (_p) {
			_p->Release();
		}
		_p = p;
		if (_p) {
			_p->AddRef();
		}
	}

	AXE_NODISCARD T** ptrForInit()	noexcept { reset(nullptr); return &_p; }

	AXE_NODISCARD T*  detach()		noexcept { T* o = _p; _p = nullptr; return o; }

#if AXE_OS_WINDOWS
	template <typename R>
	::HRESULT As(ComPtr<R>* dst) {
		AXE_STATIC_ASSERT(is_base_of_v<IUnknown, R> && is_base_of_v<IUnknown, T>);
		dst->reset(nullptr);
		return _p->QueryInterface(IID_PPV_ARGS(dst->ptrForInit()));
	}

	AXE_NODISCARD T*	   Get() noexcept		{ return ptr(); }
	AXE_NODISCARD const T* Get() const noexcept { return ptr(); }
#endif

private:
	T* _p = nullptr;
}; // ComPtr

} // namespace axe