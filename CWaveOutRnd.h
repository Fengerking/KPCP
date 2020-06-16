/*******************************************************************************
	File:		CWaveOutRnd.h

	Contains:	The wave out render header file.

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-26		Bangfei			Create file

*******************************************************************************/
#ifndef __CWaveOutRnd_H__
#define __CWaveOutRnd_H__

#include "windows.h"
#include "mmSystem.h"

#include "CNodeList.h"

#define MAXINPUTBUFFERS		3
#define WM_USER_ARND		WM_USER+100

class CWaveOutRnd
{
public:
	typedef struct WAVEHDRINFO
	{
		long long	llTime;
		void *		pData;
	} WAVEHDRINFO;

	static bool CALLBACK BVEWaveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, 
													DWORD dwParam1, DWORD dwParam2);
public:
	CWaveOutRnd(HWND hWnd);
	virtual ~CWaveOutRnd(void);

	virtual int		Init (int nSampleRate, int nChannels);
	virtual int		Uninit (void);

	virtual int		Start (void);
	virtual int		Stop (void);
	virtual int		Flush (void);

	virtual int		Render (unsigned char * pBuff, int nSize, long long llTime);

protected:
	virtual bool	InitDevice (void);
	virtual bool	AllocBuffer (void);
	virtual bool	ReleaseBuffer (void);
	virtual int		UpdateFormat (void);

	virtual int		WaitAllBufferDone (int nWaitTime);

	virtual bool	AudioDone (WAVEHDR * pWaveHeader);

protected:
	HWND				m_hWnd;
	int					m_nSampleRate;
	int					m_nChannels;
	int					m_nOffsetTime;

	unsigned char *		m_pPCMData;
	int					m_nPCMSize;
	unsigned char *		m_pPCMBuff;
	int					m_nPCMLen;

	HWAVEOUT				m_hWaveOut;
	UINT					m_nDeviceID;
	WAVEFORMATEX 			m_wavFormat;
	DWORD					m_dwVolume;
	bool					m_bReseted;
	bool					m_bFirstRnd;
	int						m_nWriteCount;

	CObjectList<WAVEHDR>	m_lstFull;
	CObjectList<WAVEHDR>	m_lstFree;
	unsigned int			m_nBufSize;
	long long				m_llBuffTime;
};

#endif // __CWaveOutRnd_H__
