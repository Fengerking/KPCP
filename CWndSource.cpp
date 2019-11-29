/*******************************************************************************
	File:		CWndSource.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndSource.h"

#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

CWndSource::CWndSource(HINSTANCE hInst)
	: CWndBase (hInst)
{
	m_pImgAnalyse = NULL;
}

CWndSource::~CWndSource(void) {
	if (m_pImgAnalyse != NULL)
		delete m_pImgAnalyse;
}

bool CWndSource::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG) {
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;
	return true;
}

LRESULT CWndSource::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	RECT rcView;
	if (hwnd != NULL)
		GetClientRect (hwnd, &rcView);

	switch (uMsg) {
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		//BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, m_hBmpDC, 0, 0, SRCCOPY);
		if (m_hBitmap != NULL) {
			BITMAP bmpInfo;
			GetObject(m_hBitmap, sizeof (BITMAP), &bmpInfo);		
			SetStretchBltMode(hdc, HALFTONE);
			//StretchBlt(hdc, 0, 0, rcView.right, rcView.bottom, m_hBmpDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
		}
		EndPaint(hwnd, &ps);
		return S_OK;// DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

int	CWndSource::OpenFile(char * pFileName) {
	Mat matImg = imread(pFileName, IMREAD_COLOR);
	if (matImg.empty())
		return -1;
	int nW = matImg.cols;
	int nH = matImg.rows;

	int nSize = nW * nH * 4;
	unsigned char * pImgBuff = new unsigned char[nSize];
	memset(pImgBuff, 0, nSize);
	uchar * pDst = pImgBuff;
	uchar * pSrc = matImg.data;
	for (int i = 0; i < nH; i++) {
		for (int j = 0; j < nW; j++) {
			*pDst++ = *pSrc++; *pDst++ = *pSrc++;
			*pDst++ = *pSrc++; *pDst++;
		}
	}
	if (m_hBitmap != NULL) {
		SelectObject(m_hBmpDC, m_hBmpOld);
		DeleteObject(m_hBitmap);
	}
	m_hBitmap = CreateBitmap(nW, nH, 1, 32, pImgBuff);
	m_hBmpOld = (HBITMAP)SelectObject(m_hBmpDC, m_hBitmap);
	  
	matImg.release();

	if (m_pImgAnalyse == NULL) {
		m_pImgAnalyse = new CImgAnalyse(m_hParent);
		m_pImgAnalyse->SetMusicPage(m_pMusicPage);
	}
	m_pImgAnalyse->OpenFile(pFileName);

	InvalidateRect(m_hWnd, NULL, FALSE);

	return 0;
}
