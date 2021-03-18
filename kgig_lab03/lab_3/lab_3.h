
// lab_3.h: основной файл заголовка для приложения lab_3
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить stdafx.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// Clab3App:
// Сведения о реализации этого класса: lab_3.cpp
//

class Clab3App : public CWinApp
{
public:
	Clab3App();


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация
public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Clab3App theApp;
