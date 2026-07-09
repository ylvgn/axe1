#pragma once

namespace axe {

class ImUIView : public RttiObject {
	AXE_RTTI_INFO(ImUIView, RttiObject)
public:
	ImUIView()
		: _worldPosDirty(true)
	{}

	void setSize(const Vec2f& s);
	void setWorldPos(const Vec2f& pos);
	void setWorldRect(const Rect2f& rc) { setWorldPos(rc.pos); setSize(rc.size); }

	void addChild(ImUIView* p);
	
	const	Rect2f&	rect()		const { return _rect; }
	const	Vec2f&	worldPos()	const;
			Rect2f	worldRect() const { return Rect2f(worldPos(), _rect.size); }

protected:
	virtual void onSetSize			(const Vec2f& s);
	virtual void onSizeChanged		(const Vec2f& s);
	
	virtual void onSetWorldPos		(const Vec2f& pos);
	virtual void onWorldPosChanged	(const Vec2f& pos);

	Vector<SPtr<ImUIView>, 4>	_children;
	ImUIView*					_parent = nullptr;
	Rect2f						_rect			{};
	Vec2f						_worldPos		{};
	bool						_worldPosDirty : 1;

private:
	void _computeWorldPos();
	void _setWorldPosDirty();
}; // ImUIView

} // namespace axe