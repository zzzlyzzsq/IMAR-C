#ifndef BOX_H_
#define BOX_H_

#include <iostream>
#include <cassert>

#include "Point.h"
#include "Size.h"
#include "numericFunctions.h"

template <typename T = double>
class Box
{
public:
        typedef T ValueType;

        Box()
        : xp(T(0)), yp(T(0)), w(T(0)), h(T(0)) { }

        Box(const Point<T> &topLeft, const Size<T> &size)
        : xp(topLeft.getX()), yp(topLeft.getY()), w(size.getWidth()), h(size.getHeight()) { }

        Box(T left, T top, T width, T height)
        : xp(left), yp(top), w(width), h(height) { }

        bool isEmpty() const;
        bool isValid() const;
        Box normalized() const;

        T getLeft() const { return xp; }
        T getTop() const { return yp; }
        T getRight() const { return xp + w; }
        T getBottom() const { return yp + h; }
        Size<T> getSize() const;

        T getX() const;
        T getY() const;
        void setLeft(T pos);
        void setTop(T pos);
        void setRight(T pos);
        void setBottom(T pos);
        void setX(T pos) { setLeft(pos); }
        void setY(T pos) { setTop(pos); }

        Point<T> getTopLeft() const { return Point<T>(xp, yp); }
        Point<T> getBottomRight() const { return Point<T>(xp+w, yp+h); }
        Point<T> getTopRight() const { return Point<T>(xp+w, yp); }
        Point<T> getBottomLeft() const { return Point<T>(xp, yp+h); }
        Point<T> getCenter() const;

        void setTopLeft(const Point<T> &p);
        void setBottomRight(const Point<T> &p);
        void setTopRight(const Point<T> &p);
        void setBottomLeft(const Point<T> &p);

        void moveLeft(T pos);
        void moveTop(T pos);
        void moveRight(T pos);
        void moveBottom(T pos);
        void moveTopLeft(const Point<T> &p);
        void moveBottomRight(const Point<T> &p);
        void moveTopRight(const Point<T> &p);
        void moveBottomLeft(const Point<T> &p);
        void moveCenter(const Point<T> &p);

        void translate(T dx, T dy);
        void translate(const Point<T> &p);

        Box translated(T dx, T dy) const;
        Box translated(const Point<T> &p) const;

        void moveTo(T x, T t);
        void moveTo(const Point<T> &p);

        void setBox(T x, T y, T w, T h);
        void getBox(T *x = 0, T *y = 0, T *w = 0, T *h = 0) const;

        void setCoords(T x1, T y1, T x2, T y2);
        void getCoords(T *x1 = 0, T *y1 = 0, T *x2 = 0, T *y2 = 0) const;

        template<typename T2>
    void scale(T2 xScale, T2 yScale);

        T getWidth() const;
        T getHeight() const;
        void setWidth(T w);
        void setHeight(T h);
        void setSize(const Size<T> &s);
        T getArea() const;

        Box<T> operator|(const Box &r) const;
        Box<T> operator&(const Box &r) const;
        Box<T>& operator|=(const Box &r);
        Box<T>& operator&=(const Box &r);
        template<typename T2>
        Box<T>& operator*=(T2 c);
        template<typename T2>
        Box<T>& operator/=(T2 c);

        template<typename T2>
        bool contains(const Point<T2> &p) const;
        template<typename T2>
        bool contains(T2 x, T2 y) const;
        bool contains(const Box<T> &r) const;
        Box<T> unite(const Box<T> &r) const;
        Box<T> intersect(const Box<T> &r) const;
        bool intersects(const Box<T> &r) const;

        template<typename T2>
        friend bool operator==(const Box<T2> &, const Box<T2> &);
        template<typename T2>
        friend bool operator!=(const Box<T2> &, const Box<T2> &);
        template<typename T2, typename T3>
        friend Box<T2> operator*(const Box<T2> &, T3 c);
        template<typename T2, typename T3>
        friend Box<T2> operator*(T3 c, const Box<T2> &);
        template<typename T2, typename T3>
        friend Box<T2> operator/(const Box<T2> &, T3 c);

        template<typename T2>
        operator Box<T2>() const;

private:
        T xp;
        T yp;
        T w;
        T h;
};

// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& o, const Box<T>& b);

template<typename T>
std::istream& operator>>(std::istream& i, Box<T>& b);


//bool operator==(const Box &, const Box &);
//bool operator!=(const Box &, const Box &);

// INLINE FUNCTIONS //
template<typename T>
inline
bool Box<T>::isEmpty() const
{ return w == T(0) || h == T(0); }

template<typename T>
inline
bool Box<T>::isValid() const
{ return w > T(0) && h > T(0); }

template<typename T>
inline
T Box<T>::getX() const
{ return xp; }

template<typename T>
inline
T Box<T>::getY() const
{ return yp; }

template<typename T>
inline
void Box<T>::setLeft(T pos) { T diff = pos - xp; xp += diff; w -= diff; }

template<typename T>
inline
void Box<T>::setRight(T pos) { w = pos - xp; }

template<typename T>
inline
void Box<T>::setTop(T pos) { T diff = pos - yp; yp += diff; h -= diff; }

template<typename T>
inline
void Box<T>::setBottom(T pos) { h = pos - yp; }

template<typename T>
inline
void Box<T>::setTopLeft(const Point<T> &p) { setLeft(p.getX()); setTop(p.getY()); }

template<typename T>
inline
void Box<T>::setTopRight(const Point<T> &p) { setRight(p.getX()); setTop(p.getY()); }

template<typename T>
inline void Box<T>::setBottomLeft(const Point<T> &p) { setLeft(p.getX()); setBottom(p.getY()); }

template<typename T>
inline void Box<T>::setBottomRight(const Point<T> &p) { setRight(p.getX()); setBottom(p.getY()); }

template<typename T>
inline Point<T> Box<T>::getCenter() const
{ return Point<T>(xp + w/2, yp + h/2); }

template<typename T>
inline void Box<T>::moveLeft(T pos) { xp = pos; }

template<typename T>
inline void Box<T>::moveTop(T pos) { yp = pos; }

template<typename T>
inline void Box<T>::moveRight(T pos) { xp = pos - w; }

template<typename T>
inline void Box<T>::moveBottom(T pos) { yp = pos - h; }

template<typename T>
inline void Box<T>::moveTopLeft(const Point<T> &p) { moveLeft(p.getX()); moveTop(p.getY()); }

template<typename T>
inline void Box<T>::moveTopRight(const Point<T> &p) { moveRight(p.getX()); moveTop(p.getY()); }

template<typename T>
inline void Box<T>::moveBottomLeft(const Point<T> &p) { moveLeft(p.getX()); moveBottom(p.getY()); }

template<typename T>
inline void Box<T>::moveBottomRight(const Point<T> &p) { moveRight(p.getX()); moveBottom(p.getY()); }

template<typename T>
inline void Box<T>::moveCenter(const Point<T> &p) { xp = p.getX() - w/2; yp = p.getY() - h/2; }

template<typename T>
inline T Box<T>::getWidth() const
{ return w; }

template<typename T>
inline T Box<T>::getHeight() const
{ return h; }

template<typename T>
inline Size<T> Box<T>::getSize() const
{ return Size<T>(w, h); }

template<typename T>
inline void Box<T>::translate(T dx, T dy)
{
        xp += dx;
        yp += dy;
}

template<typename T>
inline void Box<T>::translate(const Point<T> &p)
{
        xp += p.getX();
        yp += p.getY();
}

template<typename T>
inline void Box<T>::moveTo(T ax, T ay)
{
        xp = ax;
        yp = ay;
}

template<typename T>
inline void Box<T>::moveTo(const Point<T> &p)
{
        xp = p.getX();
        yp = p.getY();
}

template<typename T>
inline Box<T> Box<T>::translated(T dx, T dy) const
{ return Box(xp + dx, yp + dy, w, h); }

template<typename T>
inline Box<T> Box<T>::translated(const Point<T> &p) const
{ return Box(xp + p.getX(), yp + p.getY(), w, h); }

template<typename T>
inline void Box<T>::getBox(T *ax, T *ay, T *aaw, T *aah) const
{
        if (ax)
                *ax = this->xp;
        if (ay)
                *ay = this->yp;
        if (aaw)
                *aaw = this->w;
        if (aah)
                *aah = this->h;
}

template<typename T>
inline void Box<T>::setBox(T ax, T ay, T aaw, T aah)
{
        this->xp = ax;
        this->yp = ay;
        this->w = aaw;
        this->h = aah;
}

template<typename T>
inline void Box<T>::getCoords(T *xp1, T *yp1, T *xp2, T *yp2) const
{
        if (xp1)
                *xp1 = xp;
        if (yp1)
                *yp1 = yp;
        if (xp2)
                *xp2 = xp + w;
        if (yp2)
                *yp2 = yp + h;
}

template<typename T>
inline void Box<T>::setCoords(T xp1, T yp1, T xp2, T yp2)
{
        xp = xp1;
        yp = yp1;
        w = xp2 - xp1;
        h = yp2 - yp1;
}

template<typename T>
template<typename T2>
inline void Box<T>::scale(T2 xScale, T2 yScale)
{
        T x = static_cast<T>(xScale * getLeft());
        T y = static_cast<T>(yScale * getTop());
        T x2 = static_cast<T>(xScale * getRight());
        T y2 = static_cast<T>(yScale * getBottom());
        setCoords(x, y, x2, y2);
}

template<>
template<>
inline void Box<int>::scale(float xScale, float yScale)
{
        int x = static_cast<int>(roundf(xScale * getLeft()));
        int y = static_cast<int>(roundf(yScale * getTop()));
        int x2 = static_cast<int>(roundf(xScale * getRight()));
        int y2 = static_cast<int>(roundf(yScale * getBottom()));
        setCoords(x, y, x2, y2);
}

template<>
template<>
inline void Box<long>::scale(float xScale, float yScale)
{
        long x = static_cast<long>(roundf(xScale * getLeft()));
        long y = static_cast<long>(roundf(yScale * getTop()));
        long x2 = static_cast<long>(roundf(xScale * getRight()));
        long y2 = static_cast<long>(roundf(yScale * getBottom()));
        setCoords(x, y, x2, y2);
}

template<>
template<>
inline void Box<int>::scale(double xScale, double yScale)
{
        int x = static_cast<int>(round(xScale * getLeft()));
        int y = static_cast<int>(round(yScale * getTop()));
        int x2 = static_cast<int>(round(xScale * getRight()));
        int y2 = static_cast<int>(round(yScale * getBottom()));
        setCoords(x, y, x2, y2);
}

template<>
template<>
inline void Box<long>::scale(double xScale, double yScale)
{
        long x = static_cast<long>(round(xScale * getLeft()));
        long y = static_cast<long>(round(yScale * getTop()));
        long x2 = static_cast<long>(round(xScale * getRight()));
        long y2 = static_cast<long>(round(yScale * getBottom()));
        setCoords(x, y, x2, y2);
}


template<typename T>
inline void Box<T>::setWidth(T aw)
{ this->w = aw; }

template<typename T>
inline void Box<T>::setHeight(T ah)
{ this->h = ah; }

template<typename T>
inline void Box<T>::setSize(const Size<T> &s)
{
        w = s.getWidth();
        h = s.getHeight();
}

template<typename T>
template<typename T2>
inline bool Box<T>::contains(T2 ax, T2 ay) const
{
        return contains(Point<T2>(ax, ay));
}

template<typename T>
inline Box<T>& Box<T>::operator|=(const Box &r)
{
        *this = *this | r;
        return *this;
}

template<typename T>
inline Box<T>& Box<T>::operator&=(const Box &r)
{
        *this = *this & r;
        return *this;
}

template<typename T>
template<typename T2>
inline Box<T>& Box<T>::operator*=(T2 c)
{
        scale(c, c);
        return *this;
}

template<typename T>
template<typename T2>
inline Box<T>& Box<T>::operator/=(T2 c)
{
        assert(c != T(0));
        T cInv(1 / c);
        scale(cInv, cInv);
        return *this;
}

template<typename T>
inline Box<T> Box<T>::intersect(const Box &r) const
{
        return *this & r;
}

template<typename T>
inline Box<T> Box<T>::unite(const Box &r) const
{
        return *this | r;
}


template<typename T>
inline bool operator==(const Box<T> &r1, const Box<T> &r2)
{
        return r1.xp == r2.xp && r1.yp == r2.yp
                   && r1.w == r2.w && r1.h == r2.h;
}

template<>
inline bool operator==(const Box<double> &r1, const Box<double> &r2)
{
        return fuzzyEqual(r1.xp, r2.xp) && fuzzyEqual(r1.yp, r2.yp)
                   && fuzzyEqual(r1.w, r2.w) && fuzzyEqual(r1.h, r2.h);
}

template<>
inline bool operator==(const Box<float> &r1, const Box<float> &r2)
{
        return fuzzyEqual(r1.xp, r2.xp) && fuzzyEqual(r1.yp, r2.yp)
                   && fuzzyEqual(r1.w, r2.w) && fuzzyEqual(r1.h, r2.h);
}


template<typename T>
inline bool operator!=(const Box<T> &r1, const Box<T> &r2)
{
        return r1.xp != r2.xp || r1.yp != r2.yp
                   || r1.w != r2.w || r1.h != r2.h;
}

template<>
inline bool operator!=(const Box<double> &r1, const Box<double> &r2)
{
        return !fuzzyEqual(r1.xp, r2.xp) || !fuzzyEqual(r1.yp, r2.yp)
                   || !fuzzyEqual(r1.w, r2.w) || !fuzzyEqual(r1.h, r2.h);
}

template<>
inline bool operator!=(const Box<float> &r1, const Box<float> &r2)
{
        return !fuzzyEqual(r1.xp, r2.xp) || !fuzzyEqual(r1.yp, r2.yp)
                   || !fuzzyEqual(r1.w, r2.w) || !fuzzyEqual(r1.h, r2.h);
}

template<typename T1, typename T2>
inline Box<T1> operator*(const Box<T1> &r, T2 c)
{
        Box<T1> box = r;
        box.scale(c, c);
        return box;
}

template<typename T1, typename T2>
inline Box<T1> operator*(T2 c, const Box<T1> &r)
{
        Box<T1> box = r;
        box.scale(c, c);
        return box;
}

template<typename T1, typename T2>
inline Box<T1> operator/(const Box<T1> &r, T2 c)
{
        T2 invC = 1 / c;
        Box<T1> box = r;
        box.scale(invC, invC);
        return box;
}



template<typename T>
inline T Box<T>::getArea() const
{
        if (w <= T(0) || h <= T(0))
                return 0;
        return w*h;
}
template<typename T>
template<typename T2>
inline Box<T>::operator Box<T2>() const
{
        Box<T2> box;
        box.setCoords(static_cast<T2>(getLeft()), static_cast<T2>(getTop()), static_cast<T2>(getRight()), static_cast<T2>(getBottom()));
        return box;
}


template<>
template<>
inline Box<float>::operator Box<int>() const
{
        Box<int> box;
        box.setCoords(static_cast<int>(roundf(getLeft())), static_cast<int>(roundf(getTop())),
                        static_cast<int>(roundf(getRight())), static_cast<int>(roundf(getBottom())));
        return box;
}

template<>
template<>
inline Box<float>::operator Box<long>() const
{
        Box<long> box;
        box.setCoords(static_cast<long>(roundf(getLeft())), static_cast<long>(roundf(getTop())),
                        static_cast<long>(roundf(getRight())), static_cast<long>(roundf(getBottom())));
        return box;
}

template<>
template<>
inline Box<double>::operator Box<int>() const
{
        Box<int> box;
        box.setCoords(static_cast<int>(round(getLeft())), static_cast<int>(round(getTop())),
                        static_cast<int>(round(getRight())), static_cast<int>(round(getBottom())));
        return box;
}

template<>
template<>
inline Box<double>::operator Box<long>() const
{
        Box<long> box;
        box.setCoords(static_cast<long>(round(getLeft())), static_cast<long>(round(getTop())),
                        static_cast<long>(round(getRight())), static_cast<long>(round(getBottom())));
        return box;
}



template<typename T>
inline std::ostream& operator<<(std::ostream& o, const Box<T>& b)
{
        o << b.getTopLeft() << ", " << b.getSize();
        return o;
}

template<typename T>
inline std::istream& operator>>(std::istream& i, Box<T>& b)
{
        Point<T> topLeft;
        Size<T> size;
        i.setf(std::ios_base::skipws);
        i >> topLeft;
        i.ignore(256, ',');
        i >> size;
        b = Box<T>(topLeft, size);
        return i;
}



#endif /*BOX_H_*/
