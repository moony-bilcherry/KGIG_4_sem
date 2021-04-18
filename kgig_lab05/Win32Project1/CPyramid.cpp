#include "CPyramid.h"
#include <iostream>
#define pi 3.14159265;

CPyramid::CPyramid()
{
	this->Vertices.RedimMatrix(4, 7);	// создание матрицы с координатами точек пирамиды
	this->Nabcd.RedimMatrix(3);			// создание матрицы с вектором нормали к ABC
	//	A	A1	B	B1	C	C1	
	//	3	1	0	0	0	0	0
	//	0	0	0	0	3	1	0
	//	0	3	0	3	0	3	0
	//	1	1	1	1	1	1	1
	this->Vertices(0, 0) = 3;		// A (ось х)
	this->Vertices(0, 1) = 1;		// A1 (ось х)
	this->Vertices(2, 1) = 3;		// A1 (ось z)
	this->Vertices(2, 3) = 3;		// B1 (ось z)
	this->Vertices(1, 4) = 3;		// C (ось y)
	this->Vertices(1, 5) = 1;		// C1 (ось у)
	this->Vertices(2, 5) = 3;		// C1 (ось z)
	for (int i = 0; i < 7; i++)
	{
		this->Vertices(3, i) = 1;
	}
	this->Nabcd(0) = 1;
	this->Nabcd(1) = 0;
	this->Nabcd(2) = -1;			// вектор нормали к основанию пирамиды
}

CMatrix CreateViewCoord(double r, double fi, double q)
{
	// координаты точки наблюдения
	double fi_r = (fi / 180) * pi;
	double q_r = (q / 180) * pi;

	// заполнение матрицы пересчета из МСК в ОСК
	CMatrix K(4, 4);
	K(0, 0) = -sin(fi_r);
	K(0, 1) = cos(fi_r);
	K(1, 0) = -cos(q_r) * cos(fi_r);
	K(1, 1) = -cos(q_r) * sin(fi_r);
	K(1, 2) = sin(q_r);
	K(2, 0) = -sin(q_r) * cos(fi_r);
	K(2, 1) = -sin(q_r) * sin(fi_r);
	K(2, 2) = -cos(q_r);
	K(2, 3) = r;
	K(3, 3) = 1;
	return K;
}


void CPyramid::Draw1(CDC &dc, CMatrix&PView, CRect &RW)			// с удалением ребер
{
	CMatrix ViewCart = SphereToCart(PView);		// сферические координаты в декарторы
	CMatrix XV = CreateViewCoord(PView(0), PView(1), PView(2));		// получаем матрицу пересчета
	CMatrix ViewVert = XV * this->Vertices;							// Kview * PIR
	CRectD RectView;
	GetRect(this->Vertices, RectView);								// задание области

	CMatrix MW = SpaceToWindow(RectView, RW);
	CPoint MasVert[6], a1b1c1[3], abc[3];
	CMatrix V(3);
	V(2) = 1;
	for (int i = 0; i < 6; i++)
	{
		V(0) = ViewVert(0, i);
		V(1) = ViewVert(1, i);
		V = MW * V;
		MasVert[i].x = (int)V(0);
		MasVert[i].y = (int)V(1);
	}
	abc[0] = MasVert[0];
	abc[1] = MasVert[2];
	abc[2] = MasVert[4];
	a1b1c1[0] = MasVert[1];
	a1b1c1[1] = MasVert[3];
	a1b1c1[2] = MasVert[5];

	CPen Pen(PS_SOLID, 2, RGB(0, 0, 0));
	CPen *pOldPen = dc.SelectObject(&Pen);
	CBrush BottopBrush(RGB(34, 245, 206));				// цвет нижней грани
	CBrush TopBrush(RGB(245, 192, 34));					// цвет верхней грани
	CBrush BaseBrush(RGB(251, 217, 255));				// цвет остальных граней
	CBrush *pOldBrush = dc.SelectObject(&BottopBrush);
	dc.SelectObject(&BaseBrush);
	CPoint ab[4];
	ab[0] = abc[0];
	ab[1] = abc[1];
	ab[2] = a1b1c1[1];
	ab[3] = a1b1c1[0];

	CPoint bc[4];
	bc[0] = abc[1];
	bc[1] = abc[2];
	bc[2] = a1b1c1[2];
	bc[3] = a1b1c1[1];

	CPoint ac[4];
	ac[0] = abc[2];
	ac[1] = abc[0];
	ac[2] = a1b1c1[0];
	ac[3] = a1b1c1[2];

	if (PView(1) < 90)				// проверка по углу поворота вокруг оси Z
	{
		dc.Polygon(ab, 4);
		dc.Polygon(ac, 4);
	}
	else
	{
		if (PView(1) < 180)
		{
			dc.Polygon(ac, 4);
			dc.Polygon(bc, 4);
		}
		else
			if (PView(1) < 270)
			{
				dc.Polygon(ab, 4);
				dc.Polygon(bc, 4);
			}
			else
			{
				dc.Polygon(ac, 4);
				dc.Polygon(ab, 4);
			}
	}
	if (PView(2) <= 10 && PView(1) > 180 && PView(1) < 270)
	{
		dc.Polygon(ac, 4);
		dc.Polygon(ab, 4);
		dc.Polygon(bc, 4);
	}

	if (PView(2) <= 90)
	{
		dc.SelectObject(&TopBrush);
		dc.Polygon(a1b1c1, 3);
	}
	else
	{
		dc.SelectObject(&BottopBrush);
		dc.Polygon(abc, 3);
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}

void CPyramid::Draw(CDC &dc, CMatrix&PView, CRect &RW)			// без удаления граней
{
	CMatrix ViewCart = SphereToCart(PView);
	CMatrix XV = CreateViewCoord(PView(0), PView(1), PView(2));
	CMatrix ViewVert = XV * this->Vertices;
	CRectD RectView;
	GetRect(Vertices, RectView);
	CMatrix MW = SpaceToWindow(RectView, RW);
	CPoint MasVert[6], a1b1c1[3], abc[3];
	CMatrix V(3);
	V(2) = 1;
	for (int i = 0; i < 6; i++)
	{
		V(0) = ViewVert(0, i);
		V(1) = ViewVert(1, i);
		V = MW * V;
		MasVert[i].x = (int)V(0);
		MasVert[i].y = (int)V(1);
	}
	abc[0] = MasVert[0];
	abc[1] = MasVert[2];
	abc[2] = MasVert[4];
	a1b1c1[0] = MasVert[1];
	a1b1c1[1] = MasVert[3];
	a1b1c1[2] = MasVert[5];
	CPen Pen(PS_SOLID, 2, RGB(0, 0, 0));
	CPen *pOldPen = dc.SelectObject(&Pen);
	dc.MoveTo(abc[0]);
	dc.LineTo(abc[1]);
	dc.MoveTo(abc[0]);
	dc.LineTo(abc[2]);
	dc.MoveTo(abc[2]);
	dc.LineTo(abc[1]);

	dc.MoveTo(a1b1c1[0]);
	dc.LineTo(a1b1c1[1]);
	dc.MoveTo(a1b1c1[0]);
	dc.LineTo(a1b1c1[2]);
	dc.MoveTo(a1b1c1[2]);
	dc.LineTo(a1b1c1[1]);

	dc.MoveTo(abc[0]);
	dc.LineTo(a1b1c1[0]);
	dc.MoveTo(abc[1]);
	dc.LineTo(a1b1c1[1]);
	dc.MoveTo(abc[2]);
	dc.LineTo(a1b1c1[2]);

	dc.SelectObject(pOldPen);
}

void  CPyramid::GetRect(CMatrix& Vert, CRectD&RectView)
{
	RectView.top = Vert.GetRow(2).MinElement();
	RectView.bottom = Vert.GetRow(2).MaxElement();
	RectView.left = Vert.GetRow(0).MinElement();
	RectView.right = Vert.GetRow(0).MaxElement();
}

CMatrix VectorMult(CMatrix& V1, CMatrix& V2)
{
	if (V1.rows() != V2.rows() || V1.cols() > 1 || V2.cols() > 1) // Число столбцов больше одного 
	{
		char* error = "CMatrix VectorMult(CMatrix& V1, CMatrix& V2) объект не вектор - число столбцов больше 1 ";
		MessageBoxA(NULL, error, "Ошибка", MB_ICONSTOP);
		exit(1);
	}

	CMatrix Temp = V1;
	Temp(0) = V1(1)*V2(2) - V1(2)*V2(1);
	Temp(1) = V1(2)*V2(0) - V1(0)*V2(2);
	Temp(2) = V1(0)*V2(1) - V1(1)*V2(0);

	return Temp;
}

double ScalarMult(CMatrix& V1, CMatrix& V2)
{
	if (V1.rows() != V2.rows() || V1.cols() > 1 || V2.cols() > 1) // Число столбцов больше одного 
	{
		char* error = "double ScalarMult(CMatrix& V1, CMatrix& V2) объект не вектор - число столбцов больше 1 ";
		MessageBoxA(NULL, error, "Ошибка", MB_ICONSTOP);
		exit(1);
	}

	return V1(0)*V2(0) + V1(1)*V2(1) + V1(2)*V2(2);
}