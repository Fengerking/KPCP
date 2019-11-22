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
	if (m_pMusicPage->m_nWidth <= 0)
		return;
	int nW = m_pMusicPage->m_nWidth;
	int nH = m_pMusicPage->m_nHeight;
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
	SelectObject(m_hBmpDC, hPenOld);
	DeleteObject(hPen);

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
	pos = m_pMusicPage->m_lstNote.GetHeadPosition();
	while (pos != NULL) {
		pNote = m_pMusicPage->m_lstNote.GetNext(pos);
		szText[0] = pNote->m_nNote + 0X30;
		SetRect(&rcText, pNote->m_nLeft - 4, pNote->m_nTop - 4, pNote->m_nLeft + pNote->m_nWidth + 8, pNote->m_nTop + pNote->m_nHeight + 8);
		DrawText(m_hBmpDC, szText, 1, &rcText, 0);
	}
	SelectObject(m_hBmpDC, hFontOld);
	DeleteObject(hFont);
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

