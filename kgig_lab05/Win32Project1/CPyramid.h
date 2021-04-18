
#include "afxwin.h"
#include "CMatrix.h"

class CPyramid
{
	CMatrix Vertices;		// ���������� ������
	CMatrix Nabcd;          // ������ ������� �������
	void GetRect(CMatrix& Vert, CRectD&RectView);
	// ��������� ���������� ��������������,������������� �������� 
	// �������� �� ��������� XY � ������� ������� ���������
	// Vert - ���������� ������ (� ��������)
	// RectView - �������� - ������������ �������������
public:
	CPyramid(); // ����������������������
	void Draw(CDC &dc, CMatrix &P, CRect &RW);	// ������ �������� � ��������� ��������� �����
												// ��������������� �������� ��������� �� ������� � ������� (MM_TEXT)
												// dc - ������ �� ����� CDC MFC
												// P - ���������� ����� ���������� � ������� ����������� ������� ���������
												// (r,fi(����.), q(����.))
												// RW - ������� � ���� ��� �����������

	void Draw1(CDC &dc, CMatrix&P, CRect &RW);	// ������ �������� ��� �������� ��������� �����
												// ��������������� �������� ��������� �� ������� � ������� (MM_TEXT)
												// dc - ������ �� ����� CDCMFC
												// P - ���������� ����� ���������� � ������� ����������� ������� ���������
												// (r,fi(����.), q(����.))
												// RW - ������� � ���� ��� �����������		
};

CMatrix CreateViewCoord(double r, double fi, double q);
// ������� ������� ��������� ����� �� ������� ������� ��������� � �������
// (r,fi,q)- ���������� ����� ����������(������ ������� ������� ���������)
// � ������� ����������� ������� ���������( ����fi � q � ��������)

CMatrix VectorMult(CMatrix & V1, CMatrix & V2);
// ��������� ��������� ������������ �������� V1 (3x1) � V2(3x1)

double ScalarMult(CMatrix& V1, CMatrix& V2);
// ��������� ��������� ������������ �������� V1(3x1) � V2(3x1)