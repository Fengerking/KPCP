/*******************************************************************************
	File:		CWndResult.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CWndResult.h"
#include "CCheckSelectResult.h"

CWndResult::CWndResult(HINSTANCE hInst)
	: CWndBase (hInst)
{

}

CWndResult::~CWndResult(void) {

}

bool CWndResult::CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG) {
	if (!CWndBase::CreateWnd (hParent, rcView, clrBG))
		return false;
	return true;
}

void CWndResult::UpdateResult(void) {
	int nW = m_pMusicPage->m_nWidth;
	int nH = m_pMusicPage->m_nHeight;
	//CCheckSelectResult checkSelect;
	//checkSelect.OpenFile("c:/temp/checkResult/1574585752249.yuv");
	//int width = 640;
	//int height = 480;
	//unsigned char * pBmpData = new unsigned char[width * height * 4];
	//memset(pBmpData, 168, width * height * 4);
	//m_hBitmap = CreateBitmap(width, height, 1, 32, pBmpData);
	//m_hBmpOld = (HBITMAP)SelectObject(m_hBmpDC, m_hBitmap);
	//HBRUSH hRCBrush = CreateSolidBrush(RGB(0, 0, 0));
	//for (int i = 0; i < checkSelect.m_nFindLines; i++) {
	//	FillRect(m_hBmpDC, (RECT *)&checkSelect.m_rcLines[i], hRCBrush);
	//}
	//DeleteObject(hRCBrush);
	//return;

	//if (m_pMusicPage->m_nWidth <= 0)
	//	return;


	//unsigned char * pData = new unsigned char[nW * nH * 4];
	//memset(pData, 168, nW * nH * 4);
	//CImgObject *	pObj = NULL;
	//PixPoint *		pPix = NULL;
	//NODEPOS posObj = m_pMusicPage->m_lstObject.GetHeadPosition();
	//while (posObj != NULL) {
	//	pObj = m_pMusicPage->m_lstObject.GetNext(posObj);
	//	if (pObj->m_lstPixel.GetCount() < 30)
	//		continue;
	//	NODEPOS posPix = pObj->m_lstPixel.GetHeadPosition();
	//	while (posPix != NULL) {
	//		pPix = pObj->m_lstPixel.GetNext(posPix);
	//		*((int *)(pData + pPix->nY * nW * 4 + pPix->nX * 4)) = 0;
	//	}
	//	//break;
	//}
	//m_hBitmap = CreateBitmap(nW, nH, 1, 32, pData);
	//m_hBmpOld = (HBITMAP)SelectObject(m_hBmpDC, m_hBitmap);
	//delete[]pData;
	//return;


	if (m_hBitmap != NULL) {
		BITMAP bmpInfo;
		GetObject(m_hBitmap, sizeof(BITMAP), &bmpInfo);
		if (bmpInfo.bmWidth != nW || bmpInfo.bmHeight != nH) {
			SelectObject(m_hBmpDC, m_hBmpOld);
			DeleteObject(m_hBitmap);
			m_hBitmap = NULL;
		}
	}
	if (m_hBitmap == NULL) {
		m_hBitmap = CreateBitmap(nW, nH, 1, 32, NULL);
	}
	m_hBmpOld = (HBITMAP)SelectObject(m_hBmpDC, m_hBitmap);

	HBRUSH	hBrush = CreateSolidBrush(RGB(200, 200, 200));
	RECT	rcBmp;
	SetRect(&rcBmp, 0, 0, nW, nH);
	FillRect(m_hBmpDC, &rcBmp, hBrush);
	DeleteObject(hBrush);

	HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
	HPEN hPenOld = (HPEN)SelectObject(m_hBmpDC, hPen);
	MusicLine * pLine = NULL;
	NODEPOS pos = m_pMusicPage->m_lstLine.GetHeadPosition();
	while (pos != NULL) {
		pLine = m_pMusicPage->m_lstLine.GetNext(pos);
		MoveToEx(m_hBmpDC, pLine->m_nLeft, pLine->m_nTop, NULL);
		LineTo(m_hBmpDC, pLine->m_nLeft, pLine->m_nTop + pLine->m_nHeight);
	}

	int nFontH = 22;
	MusicNote * pNote = m_pMusicPage->m_lstNote.GetHead();
	if (pNote != NULL)
		nFontH = pNote->m_nHeight + 10;
	HFONT hFont = CreateFont(nFontH,
						0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
						CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Times New Roman"));
	HFONT hFontOld = (HFONT)SelectObject(m_hBmpDC, hFont);

	SetBkMode(m_hBmpDC, TRANSPARENT);
	RECT	rcText;
	char	szText[2];
	int		nX, nY;
	pos = m_pMusicPage->m_lstNote.GetHeadPosition();
	while (pos != NULL) {
		pNote = m_pMusicPage->m_lstNote.GetNext(pos);
		szText[0] = pNote->m_nNote + 0X30;
		SetRect(&rcText, pNote->m_nLeft - 4, pNote->m_nTop - 4, pNote->m_nLeft + pNote->m_nWidth + 8, pNote->m_nTop + pNote->m_nHeight + 8);
		DrawText(m_hBmpDC, szText, 1, &rcText, 0);

		if (pNote->m_nHighLevel > 0) {
			int nDotX = pNote->m_nLeft + pNote->m_nWidth / 2 - 1;
			int nDotY = pNote->m_nTop - (pNote->m_nTop - pNote->m_pLine->m_nTop) / 2;
			if (pNote->m_nHighLevel == 2) {
				nDotX = nDotX - 3;
			}
			drawDot(nDotX, nDotY);
			if (pNote->m_nHighLevel == 2) {
				nDotX = nDotX + 6;
				drawDot(nDotX, nDotY);
			}
		}

		int nW = pNote->m_nLeft + pNote->m_nWidth + 2;
		int nH = (pNote->m_pLine->m_nTop + pNote->m_pLine->m_nHeight) - (pNote->m_nTop + pNote->m_nHeight);
		if (pNote->m_nLength < 0) {
			nX = pNote->m_nLeft - 6;
			nY = pNote->m_nTop + pNote->m_nHeight + nH / 2;
			MoveToEx(m_hBmpDC, nX, nY, NULL);
			LineTo(m_hBmpDC, nW , nY);

			if (pNote->m_nLength <= -2) {
				nY = nY + nH / 2;
				MoveToEx(m_hBmpDC, nX, nY, NULL);
				LineTo(m_hBmpDC, nW, nY);
			}
			if (pNote->m_nLength <= -3) {
				nY = nY + nH / 2;
				MoveToEx(m_hBmpDC, nX, nY, NULL);
				LineTo(m_hBmpDC, nW, nY);
			}
		}
		if (pNote->m_nHighLevel < 0) {
			int nDotX = pNote->m_nLeft + pNote->m_nWidth / 2 - 3;
			int nDotY = nY + nH / 2;
			if (pNote->m_nHighLevel == 2) {
				nDotX = nDotX - 3;
			}
			drawDot(nDotX, nDotY);
			if (pNote->m_nHighLevel == 2) {
				nDotX = nDotX + 6;
				drawDot(nDotX, nDotY);
			}
		}

		if (pNote->m_nLength == 1) {
			int nDotX = pNote->m_nLeft + pNote->m_nWidth + 2;
			int nDotY = pNote->m_nTop + pNote->m_nHeight - 2;
			drawDot(nDotX, nDotY);
		}
		if (pNote->m_nLength >= 2) {
			nX = pNote->m_nLeft + pNote->m_nWidth + 2;
			int nRStep = pNote->m_nWidth * 3 / 2;
			nW = nX + nRStep;
			nY = pNote->m_nTop + pNote->m_nHeight * 2 / 3;
			for (int i = 2; i <= pNote->m_nLength; i += 2) {
				MoveToEx(m_hBmpDC, nX, nY, NULL);
				LineTo(m_hBmpDC, nW, nY);

				nX = nW + 2;
				nW = nW + nRStep;
			}
		}
	}
	SelectObject(m_hBmpDC, hFontOld);
	DeleteObject(hFont);
	SelectObject(m_hBmpDC, hPenOld);
	DeleteObject(hPen);
}

void CWndResult::drawDot(int nDotX, int nDotY) {
	MoveToEx(m_hBmpDC, nDotX, nDotY, NULL);
	LineTo(m_hBmpDC, nDotX + 1, nDotY);
	LineTo(m_hBmpDC, nDotX + 1, nDotY + 1);
	LineTo(m_hBmpDC, nDotX, nDotY + 1);
	LineTo(m_hBmpDC, nDotX, nDotY);
}

LRESULT CWndResult::OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
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
			GetObject(m_hBitmap, sizeof(BITMAP), &bmpInfo);
			SetStretchBltMode(hdc, HALFTONE);
			StretchBlt(hdc, 0, 0, rcView.right, rcView.bottom, m_hBmpDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, SRCCOPY);
		}
		EndPaint(hwnd, &ps);
		return S_OK;// DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	default:
		break;
	}

	return	CWndBase::OnReceiveMessage(hwnd, uMsg, wParam, lParam);
}

