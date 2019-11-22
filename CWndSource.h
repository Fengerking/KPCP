/*******************************************************************************
	File:		CWndSource.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndSource_H__
#define __CWndSource_H__
#include "CWndBase.h"

#include "opencv2\core.hpp"
#include "CImgAnalyse.h"

class CWndSource : public CWndBase
{
public:
	CWndSource(HINSTANCE hInst);
	virtual ~CWndSource(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int		OpenFile(char * pFileName);

protected:
	CImgAnalyse	*	m_pImgAnalyse;

};
#endif //__CWndSource_H__