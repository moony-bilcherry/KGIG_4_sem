#pragma once

#include "CMATRIX.H"
#include "LibChart2D.h"

// ���� CChildView

class CChildView : public CWnd
{
	// ��������
public:
	CChildView();

	// ��������
public:
	CSolarSystem SolarSystem;
	int DtTimer;
	CRect RectWindow;
	CRectD RectWorld;
	bool Start = false;

	// ��������
public:

	// ���������������
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	// ����������
public:
	virtual ~CChildView();

	// ��������� ������� ����� ���������
protected:
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDraw();
};