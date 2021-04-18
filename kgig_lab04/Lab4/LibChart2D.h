#pragma once

const double PI = 3.14159;

struct CPen2D
{
	int _penStyle, _penWidth;
	COLORREF _color;

	CPen2D()
	{
		_penStyle = PS_SOLID;
		_penWidth = 1;
		_color = RGB(0, 0, 0);
	}

	void set(int penStyle, int penWidth, COLORREF color)
	{
		_penStyle = penStyle;
		_penWidth = penWidth;
		_color = color;
	}
};

struct CSizeD
{
	double _cx;
	double _cy;
};

struct CRectD
{
	double _top, _bottom, _left, _right;

	CRectD()
	{
		_top = _bottom = _left = _right = 0;
	}

	CRectD(double top, double bottom, double left, double right)
	{
		_top = top;
		_bottom = bottom;
		_left = left;
		_right = right;
	};

	void setRectD(double, double, double, double);

	CSizeD SizeD();
};

void setMode(CDC&, CRect&, CRectD&);

CMatrix getTranslation(double, double);

CMatrix getRotation(double);

class CSolarSystem
{
private:
	CRect SUN, EARTH, MOON, MERCURY;

	CRect EarthOrbit, MoonOrbit, MercuryOrbit;

	CMatrix _earthCoords, _moonCoords, _mercuryCoords;
	CRect RW;
	CRectD RS;

	double wEarth;				// Угловая скорость Земли относительно Солнца, град./сек.
	double wMoon;				// Угловая скорость Луны относительно Солнца, град./сек.
	double wMercury;

	double fiEarth;				// Угловое положение Земли в системе кординат Солнца, град
	double fiMoon;				// Угловое положение Луны в системе кординат Земли, град
	double fiMercury;

	double dt;	// Интервал дискретизации, сек.( Интервал между сигналами)

public:
	CSolarSystem();

	void setDt(double dtx)
	{
		dt = dtx;
	};
	void setCoords();
	void getRS(CRectD&);
	CRect getRW()
	{
		return RW;
	};
	void draw(CDC&);
};