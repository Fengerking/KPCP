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
extern int g_aNearPos[8][2];

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

	bool	m_bPlaying;

	MusicLine * m_pLine;
};

struct PixPoint {
	int nX;
	int nY;
};

class CImgObject
{
public:
	CImgObject(void);
	virtual ~CImgObject(void);
	virtual int		AddPix(int nX, int nY);
	virtual bool	InList(int nX, int nY);

public:
	CObjectList<PixPoint>	m_lstPixel;
	int						m_nLeft;
	int						m_nRight;
	int						m_nTop;
	int						m_nBottom;

};

class CMusicPage
{
public:
	CMusicPage(void);
	virtual ~CMusicPage(void);

	virtual void	SetSize(int nW, int nH) { m_nWidth = nW; m_nHeight = nH; }
	virtual int		GetRightWidth(MusicNote * pLeft);
	virtual int		ParseObject(unsigned char * pData);

protected:
	virtual bool	hadFilled(int nX, int nY);
	virtual int		fillObjPixel(CImgObject * pImgObj, unsigned char * pData, int nX, int nY);

public:
	CObjectList<MusicLine>	m_lstLine;
	CObjectList<MusicNote>	m_lstNote;
	CObjectList<CImgObject>	m_lstObject;

	int						m_nBValue;
	int						m_nWidth;
	int						m_nHeight;

protected:
	CWndBase *				m_pWndResult;

};
#endif //__CMusicPage_H__