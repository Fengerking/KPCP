/*******************************************************************************
	File:		CMusicPage.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CMusicPage_H__
#define __CMusicPage_H__

#include "CNodeList.h"

class CWndBase;

struct MusicLine {
	int		m_nLeft;
	int		m_nTop;
	int		m_nWidth;
	int		m_nHeight;
};

struct MusicNote {
	int		m_nNote;
	int		m_nHighLevel;
	int		m_nLength;

	int		m_nLeft;
	int		m_nTop;
	int		m_nWidth;
	int		m_nHeight;
};

class CMusicPage
{
public:
	CMusicPage(void);
	virtual ~CMusicPage(void);

	virtual void		SetSize(int nW, int nH) { m_nWidth = nW; m_nHeight = nH; }

public:
	CObjectList<MusicLine>	m_lstLine;
	CObjectList<MusicNote>	m_lstNote;

	int			m_nWidth;
	int			m_nHeight;

protected:
	CWndBase *	m_pWndResult;


};
#endif //__CMusicPage_H__