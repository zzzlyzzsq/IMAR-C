#include "Box.h"

template<typename T>
Box<T> Box<T>::normalized() const
{
	Box<T> r = *this;
	if (r.w < T(0)) {
		r.xp = r.xp + r.w;
		r.w = -r.w;
	}
	if (r.h < T(0)) {
		r.yp = r.yp + r.h;
		r.h = -r.h;
	}
	return r;
}


template<typename T>
template<typename T2>
bool Box<T>::contains(const Point<T2> &p) const
{
	if (isEmpty())
		return false;
	Box r = normalized();
	return p.getX() >= r.xp && p.getX() < r.xp + r.w &&
		   p.getY() >= r.yp && p.getY() < r.yp + r.h;
}



template<typename T>
bool Box<T>::contains(const Box<T> &r) const
{
	if (isEmpty() || r.isEmpty())
		return false;
	Box r1 = normalized();
	Box r2 = r.normalized();
	return r2.xp >= r1.xp && r2.xp + r2.w <= r1.xp + r1.w
		&& r2.yp >= r1.yp && r2.yp + r2.h <= r1.yp + r1.h;
}


template<typename T>
Box<T> Box<T>::operator|(const Box<T> &r) const
{
	if (isEmpty())
		return r;
	if (r.isEmpty())
		return *this;
	Box<T> r1 = normalized();
	Box<T> r2 = r.normalized();
	Box<T> tmp;
	tmp.xp = std::min(r1.xp, r2.xp);
	tmp.yp = std::min(r1.yp, r2.yp);
	tmp.w = std::max(r1.xp + r1.w, r2.xp + r2.w) - tmp.xp;
	tmp.h = std::max(r1.yp + r1.h, r2.yp + r2.h) - tmp.yp;
	return tmp;
}

template<typename T>
Box<T> Box<T>::operator&(const Box<T> &r) const
{
	if (isEmpty() || r.isEmpty())
		return Box();
	Box<T> r1 = normalized();
	Box<T> r2 = r.normalized();
	Box<T> tmp;
	tmp.setX(std::max(r1.xp, r2.xp));
	tmp.setY(std::max(r1.yp, r2.yp));
	tmp.setWidth(std::min(r1.xp + r1.w, r2.xp + r2.w) - tmp.xp);
	tmp.setHeight(std::min(r1.yp + r1.h, r2.yp + r2.h) - tmp.yp);
	return tmp.isEmpty() ? Box() : tmp;
}

template<typename T>
bool Box<T>::intersects(const Box<T> &r) const
{
	if (isEmpty() || r.isEmpty())
		return false;
	Box<T> r1 = normalized();
	Box<T> r2 = r.normalized();
	return std::max(r1.xp, r2.xp) <= std::min(r1.xp + r1.w, r2.xp + r2.w)
		&& std::max(r1.yp, r2.yp) <= std::min(r1.yp + r1.h, r2.yp + r2.h);
}
