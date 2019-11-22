/*******************************************************************************
	File:		CWndResult.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CWndResult_H__
#define __CWndResult_H__
#include "CWndBase.h"

class CWndResult : public CWndBase
{
public:
	CWndResult(HINSTANCE hInst);
	virtual ~CWndResult(void);

	virtual bool	CreateWnd (HWND hParent, RECT rcView, COLORREF clrBG);
	virtual LRESULT	OnReceiveMessage (HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void	UpdateResult(void);

protected:


};
#endif //__CWndResult_H__