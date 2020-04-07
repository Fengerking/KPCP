/*******************************************************************************
	File:		CMusicPlay.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CMusicPlay_H__
#define __CMusicPlay_H__

#include "CMusicPage.h"
#include "CWaveOutRnd.h"

class CMusicPlay
{
public:
	CMusicPlay(void);
	virtual ~CMusicPlay(void);

	// 设置简谱信息
	virtual void	SetMusicPage(CMusicPage * pPage);
	// 播放简谱
	virtual int		Play(void);
	// 暂停播放
	virtual int		Pause(void);
	// 指定位置开始播放
	virtual int		Seek(int nPos);

	// 填充PCM数据
	virtual int		FillPcmData(void);
	// 利用教程渐变拼接数据
	virtual int		SmoothPCMData(void);

protected:
	int				testNotePlay(void);

protected:
	CMusicPage *	m_pPage;
	CWaveOutRnd *	m_pRnd;

	FILE *			m_hFile;
	int				m_nOffset;
	int				m_nStep;
	int				m_nLowLen;

	int				m_aOffPos[15];

	int				m_nBuffSize;
	unsigned char	m_szBuff[4096];

};
#endif //__CMusicPlay_H__