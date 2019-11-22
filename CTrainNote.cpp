/*******************************************************************************
	File:		CTrainNote.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "stdio.h"
#include <shellapi.h>

#include "CTrainNote.h"

#include "opencv2/ml.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace cv::ml;
using namespace std;

CTrainNote::CTrainNote(HWND hWnd) {
	m_hWnd = hWnd;
	memset(m_szPath, 0, sizeof(m_szPath));
	GetModuleFileName(NULL, m_szPath, sizeof(m_szPath));
	char * pPos = strrchr(m_szPath, '\\');	*pPos = 0;
	strcat(m_szPath, "\\trainNumPic");
	CreateDirectory(m_szPath, NULL);
}

CTrainNote::~CTrainNote(void) {
	if (m_hFontDC != NULL)
		DeleteDC(m_hFontDC);
	m_hFontDC = NULL;
}

int	CTrainNote::TrainNotePic(void) {
	Mat         trainingImages;
	Mat         trainingLabels(cv::Size(0, 0), CV_32SC1);

	fillTextImage(trainingImages, trainingLabels);
	//return 0;

	//Ptr<SVM> model;
	//Ptr<TrainData> tdata = TrainData::create(trainingImages, ml::ROW_SAMPLE, trainingLabels);
	//model = SVM::create();
	//model->setType(SVM::C_SVC);
	//model->setKernel(SVM::LINEAR);
	//model->setC(1);
	//model->trainAuto(tdata);

	Ptr<KNearest> model;
	Ptr<TrainData> tdata = TrainData::create(trainingImages, ml::ROW_SAMPLE, trainingLabels);
	model = KNearest::create();
	model->setIsClassifier(true);
	model->setAlgorithmType(KNearest::Types::BRUTE_FORCE);
	model->setDefaultK(1);
	model->train(tdata);


	char szXMLFile[256];
	GetModuleFileName(NULL, szXMLFile, sizeof(szXMLFile));
	char * pPos = strrchr(szXMLFile, '\\');	*pPos = 0;
	strcat(szXMLFile, "\\numberDetectSVM.xml");
	model->save(szXMLFile);

	return 0;
}

void CTrainNote::fillTextImage(Mat& trainingImages, Mat& trainingLabels) {
	TCHAR   szFileName[256];
	TCHAR   szFilePath[256];
	int     i = 0;
	for (i = '0'; i <= '7'; i++) {
		sprintf(szFilePath, ("%s\\%c\\*.bmp"), m_szPath, i);
		WIN32_FIND_DATA  data;
		HANDLE  hFind = FindFirstFile(szFilePath, &data);
		if (hFind == INVALID_HANDLE_VALUE)
			continue;
		do {
			if (!strcmp(data.cFileName, (".")) || !strcmp(data.cFileName, ("..")))
				continue;
			if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				continue;
			sprintf(szFileName, ("%s\\%c\\%s"), m_szPath, i, data.cFileName);

			Mat  SrcImage = imread(szFileName, IMREAD_GRAYSCALE);
			if (SrcImage.empty())
				continue;

			resize(SrcImage, SrcImage, Size(24, 24), 0, 0, INTER_AREA);
			normalize(SrcImage, SrcImage, 1.0, 0.0, cv::NormTypes::NORM_MINMAX, CV_32FC1);
			//imshow("001", SrcImage);
			//Mat matShape = SrcImage.reshape(0, 0);
			//imshow("002", matShape);
			trainingImages.push_back(SrcImage.reshape(0, 1));
			//trainingImages.push_back(SrcImage);
			trainingLabels.push_back<int>(i - '0'); 
		} while (FindNextFile(hFind, &data));
		FindClose(hFind);
	}
}

INT CALLBACK NEnumFontNameProc(LOGFONT *plf, TEXTMETRIC* ptm, INT nFontType, LPARAM lParam) {
	CTrainNote *pTrainNote = (CTrainNote *)lParam;
	pTrainNote->createNumImage(plf);
	return TRUE; 
}

int	CTrainNote::CreateNumPic(void) {
	if (m_hFontDC == NULL) {
		HDC hWndDC = GetDC(m_hWnd);
		m_hFontDC = CreateCompatibleDC(hWndDC);
		ReleaseDC(m_hWnd, hWndDC);
	}
	EnumFontFamilies(m_hFontDC, (LPTSTR)NULL, (FONTENUMPROC)NEnumFontNameProc, (LPARAM)this);
	return 0;
}

int CTrainNote::createNumImage(LOGFONT *plf) {
	int nWidth = 24;
	int nHeight = 24;

	plf->lfHeight = nWidth - 0;
	plf->lfWidth = nHeight - 0;
	HFONT hFont = CreateFontIndirect(plf);
	HBRUSH hBrushWhite = CreateSolidBrush(RGB(0, 0, 0));

	HBITMAP hBmpTxt = NULL;
	HBITMAP hBmpOld = NULL;
	HDC     hBmpDC = CreateCompatibleDC(m_hFontDC);

	RECT rcWnd;
	SetRect(&rcWnd, 0, 0, nWidth, nHeight);
	SelectObject(hBmpDC, hFont);
	SetTextColor(hBmpDC, RGB(255, 255, 255));
	SetBkMode(hBmpDC, TRANSPARENT);

	int     nFlt = 0, nDx = 0;
	SIZE    szSize{ 0, 0 };
	TCHAR szFileName[256];
	int     i = 0;
	TCHAR   szText[32];
	for (i = '0'; i <= '9'; i++) {
		szText[0] = i;
		szSize.cx = 0;
		szSize.cy = 0;
		::GetTextExtentExPoint(hBmpDC, szText, 1, 1, &nFlt, &nDx, &szSize);
		hBmpTxt = CreateBitmap(szSize.cx + 4, szSize.cy + 4, 1, 32, NULL);
		hBmpOld = (HBITMAP)SelectObject(hBmpDC, hBmpTxt);
		SetRect(&rcWnd, 0, 0, szSize.cx + 4, szSize.cy + 4);
		FillRect(hBmpDC, &rcWnd, hBrushWhite);
		SetRect(&rcWnd, 2, 2, szSize.cx + 2, szSize.cy + 2);
		DrawText(hBmpDC, szText, 1, &rcWnd, 0);

		sprintf(szFileName, ("%s\\%c"), m_szPath, i);
		CreateDirectory(szFileName, NULL);
		sprintf(szFileName, ("%s\\%c\\%s.bmp"), m_szPath, i, plf->lfFaceName);
		saveBmp(hBmpDC, hBmpTxt, szFileName);

		SelectObject(hBmpDC, hBmpOld);
		DeleteObject(hBmpTxt);
	}

	//BitBlt(hdc, 0, 100, nWidth, 100 + nHeight, hBmpDC, 0, 0, SRCCOPY);

	DeleteDC(hBmpDC);
	DeleteObject(hBrushWhite);
	DeleteObject(hFont);

	return 0;
}

int CTrainNote::saveBmp(HDC hDC, HBITMAP hBitmap, TCHAR * FileName) {
	int                     iBits;
	WORD                    wBitCount;
	DWORD                   dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP                  Bitmap;
	BITMAPFILEHEADER        bmfHdr;
	BITMAPINFOHEADER        bi;
	LPBITMAPINFOHEADER      lpbi;
	HANDLE                  fh, hDib, hPal, hOldPal = NULL;

	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	if (iBits <= 1)
		wBitCount = 1;
	else  if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;

	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;

	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal) {
		hDC = ::GetDC(NULL);
		hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER) + dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	if (hOldPal) {
		::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
		RealizePalette(hDC);
		::ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (fh == INVALID_HANDLE_VALUE)
		return     FALSE;

	bmfHdr.bfType = 0x4D42;     //     "BM"           
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);

	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);

	return 1;
}
