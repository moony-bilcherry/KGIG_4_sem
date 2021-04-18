#include "stdafx.h"
#include "CMATRIX.H"
#include "LibChart2D.h"

void setMode(CDC& dc, CRect& rectWindow, CRectD& rectWorld)//параметры+функция
{
	//прямоугольник в мировых коордщинатах
	double dsx = rectWorld._right - rectWorld._left;
	double dsy = rectWorld._top - rectWorld._bottom;
	double xsL = rectWorld._left;
	double ysL = rectWorld._bottom;

	//прямоугольник оконных координат
	int dwx = rectWindow.right - rectWindow.left;
	int dwy = rectWindow.bottom - rectWindow.top;
	int xwL = rectWindow.left;
	int ywH = rectWindow.bottom;

	//вписывает в окно
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt((int)dsx, (int)dsy);
	dc.SetViewportExt(dwx, -dwy);
	dc.SetWindowOrg((int)xsL, (int)ysL);
	dc.SetViewportOrg(xwL, ywH);
}

void CRectD::setRectD(double left, double top, double right, double bottom)// запись координат для прямоугольников
{
	_left = left;
	_top = top;
	_right = right;
	_bottom = bottom;
}

CSizeD CRectD::SizeD()					// размерность прямоугольников
{
	CSizeD cz;
	cz._cx = fabs(_right - _left);		// класс,и то, к чему обращаемся
	cz._cy = fabs(_top - _bottom);
	return cz;
}

CMatrix getTranslation(double xDistance, double yDistance)
{	//переедвижение орбиты
	CMatrix translator(3, 3);
	translator(0, 0) = 1;
	translator(0, 2) = xDistance;
	translator(1, 1) = 1;
	translator(1, 2) = yDistance;
	translator(2, 2) = 1;
	return translator;
}

CMatrix getRotation(double angle)
{//поворот
	double fg = fmod(angle, 360.0);//вычисляет остаток от деления возвращая значение с плавающей точкой для определения на сколько повернулась планета
	double ff = (fg / 180.0) * PI;//переводим из градусов, в Pi
	CMatrix rotator(3, 3);//создаем матрицу вычисления угла наклона
	rotator(0, 0) = cos(ff);//заполняем матрицу (формула в маткаде)
	rotator(0, 1) = -sin(ff);
	rotator(1, 0) = sin(ff);
	rotator(1, 1) = cos(ff);
	rotator(2, 2) = 1;
	return rotator;
}

CSolarSystem::CSolarSystem()
{
	double rSun = 120,		//радиус солнца
		rEarth = 80,		//радиус земли
		rMoon = 30,			//радиус луны
		rMerc = 80;			//радиус меркурия
	double rSunToEarth = 7 * rSun,			//радиус от солнца до земли
		rEarthToMoon = 3 * rEarth,			//радиус от земли до луны
		rSunToMercury = 4 * rSun,			//радиус от солнца до меркурия
		RoV = 6 * rSun;
	double fullRadius = rSunToEarth + rEarthToMoon + rMoon + RoV;		//общий радиус всей системы

	RS.setRectD(-fullRadius, fullRadius, fullRadius, -fullRadius);				// МК
	RW.SetRect(100, -100, 900, 700);											// ОК

	SUN.SetRect(-rSun, rSun, rSun, -rSun);					// кординаты планет
	EARTH.SetRect(-rEarth, rEarth, rEarth, -rEarth);
	MOON.SetRect(-rMoon, rMoon, rMoon, -rMoon);
	MERCURY.SetRect(-rMerc, rMerc, rMerc, -rMerc);

	//положения и размеры орбиты
	EarthOrbit.SetRect(-rSunToEarth, rSunToEarth, rSunToEarth, -rSunToEarth);
	MoonOrbit.SetRect(-rEarthToMoon, rEarthToMoon, rEarthToMoon, -rEarthToMoon);
	MercuryOrbit.SetRect(-rSunToMercury, rSunToMercury, rSunToMercury, -rSunToMercury);

	//позиции начальные(Угловые позиции)
	fiEarth = 0;
	fiMoon = 0;
	fiMercury = 0;


	//скорость вращения
	wEarth = -5;
	wMoon = 10;
	wMercury = -30;

	//создаем матрицы заново
	_earthCoords.RedimMatrix(3);
	_moonCoords.RedimMatrix(3);
	_mercuryCoords.RedimMatrix(3);

	_earthCoords(2) = 1;
	_moonCoords(2) = 1;
	_mercuryCoords(2) = 1;
}


void CSolarSystem::setCoords()
{
	double RoM = (MoonOrbit.right - MoonOrbit.left) / 2;		//радиус орбиты луны
	double ff = (fiMoon / 180.0) * PI;							//длина окружности орбиты луны
	double x = RoM * cos(ff);
	double y = RoM * sin(ff);
	_moonCoords(0) = x;
	_moonCoords(1) = y;
	fiMoon += wMoon * dt;
	CMatrix P = getRotation(fiMoon);
	_moonCoords = P * _moonCoords;

	double RoE = (EarthOrbit.right - EarthOrbit.left) / 2;
	ff = (fiEarth / 180.0) * PI;
	x = RoE * cos(ff);
	y = RoE * sin(ff);
	_earthCoords(0) = x;
	_earthCoords(1) = y;
	P = getTranslation(x, y);
	_moonCoords = P * _moonCoords;

	fiEarth += wEarth * dt;
	P = getRotation(fiEarth);					//угол поворота относительно земли
	_moonCoords = P * _moonCoords;
	_earthCoords = P * _earthCoords;

	double RoMerc = (MercuryOrbit.right - MercuryOrbit.left) / 2;
	ff = (fiMercury / 180.0) * PI;
	x = RoMerc * cos(ff);
	y = RoMerc * sin(ff);
	_mercuryCoords(0) = x;
	_mercuryCoords(1) = y;

	fiMercury += wMercury * dt;
	P = getRotation(fiMercury);
	_mercuryCoords = P * _mercuryCoords;
}

void CSolarSystem::draw(CDC& dc)
{
	CBrush SBrush, EBrush, MBrush, MercBrush, *pOldBrush;
	CRect R;

	// цвета планет
	SBrush.CreateSolidBrush(RGB(255, 0, 24));
	EBrush.CreateSolidBrush(RGB(0, 255, 0));
	MBrush.CreateSolidBrush(RGB(107, 120, 142));
	MercBrush.CreateSolidBrush(RGB(255, 100, 0));

	// задание орбит
	dc.SelectStockObject(NULL_BRUSH);
	dc.Ellipse(EarthOrbit);
	dc.Ellipse(MercuryOrbit);

	// задание правых координат
	int temp = MoonOrbit.right;
	R.SetRect(_earthCoords(0) - temp, _earthCoords(1) + temp, _earthCoords(0) + temp, _earthCoords(1) - temp);
	dc.Ellipse(R);

	pOldBrush = dc.SelectObject(&SBrush);
	dc.Ellipse(SUN);

	temp = EARTH.right;
	R.SetRect(_earthCoords(0) - temp, _earthCoords(1) + temp, _earthCoords(0) + temp, _earthCoords(1) - temp);
	dc.SelectObject(&EBrush);
	dc.Ellipse(R);

	temp = MERCURY.right;
	R.SetRect(_mercuryCoords(0) - temp, _mercuryCoords(1) + temp, _mercuryCoords(0) + temp, _mercuryCoords(1) - temp);
	dc.SelectObject(&MercBrush);
	dc.Ellipse(R);

	temp = MOON.right;
	R.SetRect(_moonCoords(0) - temp, _moonCoords(1) + temp, _moonCoords(0) + temp, _moonCoords(1) - temp);
	dc.SelectObject(&MBrush);
	dc.Ellipse(R);

	dc.SelectObject(pOldBrush);
}

// Кооординаты для всего мира
void CSolarSystem::getRS(CRectD& RSX)
{
	RSX._left = RS._left;
	RSX._top = RS._top;
	RSX._right = RS._right;
	RSX._bottom = RS._bottom;
}