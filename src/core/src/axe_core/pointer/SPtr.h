#pragma once

namespace axe {

#if 0
#pragma mark ========= WeakRefBlock ============
#endif
class WeakRefBlock : public NonCopyable {
public:
	void*	  _obj = nullptr;
	AtomicInt _weakCount = 0;
}; // WeakRefBlock


#if 0
#pragma mark ========= RefCountBase ============
#endif
class RefCountBase : public NonCopyable {
public:
	virtual ~RefCountBase() {
		AXE_ASSERT(_refCount == 0);
	}

	virtual void onRefCountZero() {
		axe_delete(this);
	}

	WeakRefBlock* _weakRefBlock = nullptr;
	AtomicInt	  _refCount		= 0;
}; // RefCountBase


#if 0
#pragma mark ========= SPtr ============
#endif
template<class T>
class SPtr : public NonCopyable {
public:
	SPtr() = default;
	SPtr(Null)			noexcept {}
	SPtr(T* p)			noexcept { reset(p); }
	SPtr(SPtr && r)		noexcept { _p = r.detach(); }
	SPtr(const SPtr& r)	noexcept { reset(r._p); }

	~SPtr()				noexcept { reset(nullptr); }

	void operator=(Null)			noexcept { reset(nullptr); }
	void operator=(T* p)			noexcept { reset(p); }
	void operator=(const SPtr& r)	noexcept { reset(r._p); }
	void operator=(SPtr && r)		noexcept { reset(nullptr); _p = r.detach(); }

		  T* operator->()			noexcept { return _p; }
	const T* operator->()	const	noexcept { return _p; }

	operator T* () const & { return _p; }
	operator T* () && = delete;

	explicit operator bool() const { return _p != nullptr; }

	AXE_NODISCARD 		T* ptr()		noexcept { return _p; }
	AXE_NODISCARD const	T* ptr() const	noexcept { return _p; }

	void reset(T* p) noexcept {
		AXE_STATIC_ASSERT(is_base_of_v<RefCountBase, T>);

		if (p == _p) return;
		if (_p) {
			auto c = --_p->_refCount;
			if (c <= 0) {
				if (_p->_weakRefBlock) {
					_p->_weakRefBlock->_obj = nullptr;
					_p->_weakRefBlock = nullptr;
				}
				_p->onRefCountZero();
			}
		}
		_p = p;
		if (_p) {
			++_p->_refCount;
		}
	}

	T* detach() noexcept { T* o = _p; _p = nullptr; return o; }

	AXE_NODISCARD AXE_INLINE static SPtr<T> s_ref(T* p) noexcept { return SPtr(p); }

private:
	T* _p = nullptr;
}; // SPtr


template<class T> AXE_INLINE bool operator== (const SPtr<T>& l, const SPtr<T>& r)	noexcept { return l.ptr() == r.ptr(); }
template<class T> AXE_INLINE bool operator!= (const SPtr<T>& l, const SPtr<T>& r)	noexcept { return l.ptr() != r.ptr(); }

template<class T> AXE_INLINE bool operator== (const SPtr<T>& l, const T*& r)		noexcept { return l.ptr() == r; }
template<class T> AXE_INLINE bool operator!= (const SPtr<T>& l, const T*& r)		noexcept { return l.ptr() != r; }
template<class T> AXE_INLINE bool operator== (const T*& l, const SPtr<T>& r)		noexcept { return l == r.ptr(); }
template<class T> AXE_INLINE bool operator!= (const T*& l, const SPtr<T>& r)		noexcept { return l != r.ptr(); }

template<class T> AXE_INLINE bool operator== (const SPtr<T>& l,	const Null&)		noexcept { return l.ptr() == nullptr; }
template<class T> AXE_INLINE bool operator== (const Null&,		const SPtr<T>& r)	noexcept { return r.ptr() == nullptr; }
template<class T> AXE_INLINE bool operator!= (const SPtr<T>& l,	const Null&)		noexcept { return l.ptr() != nullptr; }
template<class T> AXE_INLINE bool operator!= (const Null&,		const SPtr<T>& r)	noexcept { return r.ptr() != nullptr; }

template <class T> AXE_NODISCARD AXE_INLINE
SPtr<T> SPtr_make(T* p) {
	return SPtr<T>::s_ref(p);
}

template <class T, class... ARGS> AXE_NODISCARD AXE_INLINE
SPtr<T> SPtr_make(/*const MemAllocRequest& req,*/ ARGS&&... args) {
	return SPtr<T>::s_ref(new /*(req)*/ T(AXE_FORWARD(args)...));
}

template<class T>
inline SPtr<T> SPtr_fromUPtr(UPtr<T>&& p) noexcept {
	return SPtr<T>::s_ref(p.release());
}

} // namespace axe