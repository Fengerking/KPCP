/*******************************************************************************
	File:		CWndBase.h

	Contains:	the base window header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndBase_H__
#define __CWndBase_H__
#include "CMusicPage.h"

class CWndBase
{
public:
	static LRESULT CALLBACK ViewWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CWndBase(HINSTANCE hInst);
	virtual ~CWndBase(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual void	Close (void);

	virtual HWND	GetWnd (void) {return m_hWnd;}

	virtual void	SetFontColor (COLORREF nColor) {m_nClrFont = nColor;}
	virtual void	SetBGColor (COLORREF nColor);

	virtual void	SetText (TCHAR * pText);
	virtual void	SetMusicPage(CMusicPage * pMusicPage) { m_pMusicPage = pMusicPage; }

	virtual void	UpdateResult(void) { ; }

protected:
	HINSTANCE		m_hInst;
	HWND			m_hParent;
	HWND			m_hWnd;
	RECT			m_rcWnd;

	TCHAR			m_szClassName[64];
	TCHAR			m_szWindowName[64];

	HBRUSH 			m_hBKBrush;

	TCHAR			m_szText[4096];
	COLORREF		m_nClrFont;

	HDC				m_hMemDC;
	HDC				m_hBmpDC;
	HBITMAP			m_hBitmap;
	HBITMAP			m_hBmpOld;

	CMusicPage *	m_pMusicPage;
};
#endif //__CWndBase_H__