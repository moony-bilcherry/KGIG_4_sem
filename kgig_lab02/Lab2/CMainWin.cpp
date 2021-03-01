#include "stdafx.h"
#include "CMainWin.h"
#include<string.h>
#include <string>

// функция отображает рисунок в заданной позиции окна
int ShowBitMap(HWND hWnd, HBITMAP hBit, int x, int y)
// hWnd - дескриптор окна, куда выводится изображение
// hBit - дескриптор рисунка
// (x,y) - координаты левого верхнего угла
{
	BITMAP BitMap;
	//в структуру BitMap считываются параметры картинки
	GetObject(hBit, sizeof(BITMAP), &BitMap);
	//из ее полей bmHeight и bmWidth получаем размеры для копирования
	int Height = BitMap.bmHeight;
	int Width = BitMap.bmWidth;
	//получаем контекст изображения
	HDC ImageDC = GetDC(hWnd);
	//создаем контекст памяти
	HDC MemoryDC = CreateCompatibleDC(ImageDC);
	//в созданный контекст памяти заносим дескриптор битовой карты
	HBITMAP OldBitmap = (HBITMAP)SelectObject(MemoryDC, hBit);
	// картинка переносится при помощи функции копирования
	BitBlt(ImageDC, x, y, Width, Height, MemoryDC, 0, 0, SRCCOPY);
	//после копирования уничтожаются контексты памяти и изображения
	SelectObject(MemoryDC, OldBitmap);
	ReleaseDC(hWnd, ImageDC);
	DeleteDC(MemoryDC);
	return 0;
}

// сохранение рабочей области окна в файле
int ClientToBmp(HWND hWnd, RECT& rect, CString str)
// hWnd - дескриптор окна, рабочая область которого сохраняется
{
	BITMAPFILEHEADER bitFile;
	BITMAPINFOHEADER bitInfo;
	RECT ImageRect = rect;
	LONG Width = ImageRect.right - ImageRect.left,		//ширина изображения
		Height = ImageRect.bottom - ImageRect.top;		//высота изображения
	int BitToPixel = 16;								//цветовая глубина 16бит

	HDC ImageDC = GetDC(hWnd);							//получаем контекст изображения
	HDC MemoryDC = CreateCompatibleDC(ImageDC);			//создаем контекст памяти

	//создаем битовую карту BitMap по размеру рабочей области окна
	//битовая карта создается на основе контекста устройства hdc, поэтому она хранит изображение DDB-формата
	HBITMAP BitMap = CreateCompatibleBitmap(ImageDC, Width, Height);

	//в созданный контекст памяти заносим дескриптор битовой карты
	HBITMAP OldBitmap = (HBITMAP)SelectObject(MemoryDC, BitMap);

	// картинка переноситсяпри помощи функции копирования
	BitBlt(MemoryDC, 0, 0, Width, Height, ImageDC, ImageRect.left, ImageRect.top, SRCCOPY);

	BitMap = (HBITMAP)SelectObject(MemoryDC, OldBitmap);

	//Это аналог функции memset(), который заполняет заголовок нулями
	ZeroMemory(&bitInfo, sizeof(BITMAPINFOHEADER));

	bitInfo.biSize = sizeof(BITMAPINFOHEADER);
	bitInfo.biWidth = Width;
	bitInfo.biHeight = Height;
	bitInfo.biPlanes = 1;						//плоскостей
	bitInfo.biBitCount = BitToPixel;			//16 бит
	//Для вычисления размера изображения в байтах
	bitInfo.biSizeImage = (Width * BitToPixel + 31) / 32 * 4 * Height;

	DWORD dwWritten;							//количество записанных файлов

	//Открываем файл
	HANDLE FileHandle = CreateFile(str, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (FileHandle == INVALID_HANDLE_VALUE) return 2;
	bitFile.bfType = ('M' << 8) | 'B';			//заполняем заголовок

	bitFile.bfSize = bitInfo.biSizeImage + sizeof(bitFile) + bitInfo.biSize;		//размер файла
	bitFile.bfReserved1 = bitFile.bfReserved2 = 0;
	bitFile.bfOffBits = sizeof(bitFile) + bitInfo.biSize;			//смещение до начала данных

	//Запись заголовка в файл
	WriteFile(FileHandle, (LPSTR)&bitFile, sizeof(bitFile), &dwWritten, NULL);
	//Запись в файл загружаемого заголовка
	WriteFile(FileHandle, (LPSTR)&bitInfo, sizeof(BITMAPINFOHEADER), &dwWritten, NULL);

	//Выделяем место в памяти для того, чтобы функция GetDIBits()перенесла туда коды цвета в DIB-формате
	char *lp = (char *)GlobalAlloc(GMEM_FIXED, bitInfo.biSizeImage);

	//Из карты BitMap строки с нулевой по bi.biHeight функция пересылает в массив lp по формату bi (беспалитровый формат)
	GetDIBits(ImageDC, BitMap, 0, (UINT)Height, lp, (LPBITMAPINFO)&bitInfo, DIB_RGB_COLORS);

	//запись изображения
	WriteFile(FileHandle, lp, bitInfo.biSizeImage, &dwWritten, NULL);

	//Освобождение памяти и закрытие файла
	GlobalFree(GlobalHandle(lp));
	CloseHandle(FileHandle);
	ReleaseDC(hWnd, ImageDC);
	DeleteDC(MemoryDC);
	if (dwWritten == 0) 
		return 2;
	return 0;
}

CMainWin::CMainWin()
{
	this->Create(0, (LPCTSTR)L"KGIG lab 02, hello", WS_OVERLAPPEDWINDOW, rectDefault, 0, (LPCTSTR)IDR_MAINFRAME);
}

void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	if (opened)
	{
		FileDialog = new CFileDialog(true);
		FileDialog->DoModal();
		HBITMAP bit = (HBITMAP)LoadImage(NULL, FileDialog->GetPathName(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
		ShowBitMap(this->m_hWnd, bit, PaintPosition.x, PaintPosition.y);
		delete FileDialog;
		FileDialog = nullptr;
		opened = false;
	} 
	if (saved)
	{
		FileDialog = new CFileDialog(false);
		FileDialog->DoModal();
		ClientToBmp(m_hWnd, SaveRect, FileDialog->GetPathName());
		delete FileDialog;
		FileDialog = nullptr;
		saved = false;
	}
}

void CMainWin::OnLButtonDown(UINT Flags, CPoint Location)
{
	SaveRect.left = Location.x;
	SaveRect.top = Location.y;
	PaintPosition = Location;
}

void CMainWin::OnLButtonUp(UINT Flags, CPoint Location)
{
	SaveRect.right = Location.x;
	SaveRect.bottom = Location.y;
	if (SaveRect.left > SaveRect.right)
	{
		LONG buffer = SaveRect.left;
		SaveRect.left = SaveRect.right;
		SaveRect.right = buffer;
	}
	if (SaveRect.top > SaveRect.bottom)
	{
		LONG buffer = SaveRect.top;
		SaveRect.top = SaveRect.bottom;
		SaveRect.bottom = buffer;
	}
	if (SaveRect.bottom - SaveRect.top > 10 && SaveRect.right - SaveRect.left > 10)
		saved = true;
	OnPaint();
}
void CMainWin::Open()
{
	opened = true;
	InvalidateRect(0);
}
void CMainWin::Clear()
{
	InvalidateRect(0);
}

BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
	ON_COMMAND(ID_CLEAR, &CMainWin::Clear)
	ON_COMMAND(ID_TESTS_IMAGE, &CMainWin::Open)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()