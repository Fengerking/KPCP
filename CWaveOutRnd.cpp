/*******************************************************************************
	File:		CWaveOutRnd.cpp

	Contains:	The wave out render implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-26		Bangfei			Create file

*******************************************************************************/
#include "CWaveOutRnd.h"

CWaveOutRnd::CWaveOutRnd(HWND hWnd)
	: m_hWaveOut (NULL)
	, m_nDeviceID (WAVE_MAPPER)
	, m_dwVolume (0XFF)
	, m_bReseted (true)
	, m_bFirstRnd (true)
	, m_nWriteCount (0)
	, m_nBufSize (0)
	, m_llBuffTime (0)
	, m_pPCMData(NULL)
	, m_nPCMSize(0)
	, m_pPCMBuff(NULL)
	, m_nPCMLen(0)
{
	m_hWnd = hWnd;
	memset (&m_wavFormat, 0, sizeof (m_wavFormat));
	m_nSampleRate = 0;
	m_nChannels = 0;
	m_nOffsetTime = 0;
}

CWaveOutRnd::~CWaveOutRnd(void) {
	Uninit ();
	if (m_pPCMData != NULL)
		delete[]m_pPCMData;
}

int CWaveOutRnd::Init (int nSampleRate, int nChannels) {
	Uninit();
	m_nSampleRate = nSampleRate;
	m_nChannels = nChannels;
	return 0;
}

bool CWaveOutRnd::InitDevice (void) {
	if (m_nSampleRate == 0)
		return false;
	if (UpdateFormat () == 0) {
		if (m_hWaveOut != NULL)
			return true;
	}
	Uninit ();
	if (m_hWaveOut != NULL) {
		return false;
	}
	MMRESULT mr = waveOutOpen (&m_hWaveOut, m_nDeviceID, &m_wavFormat, (DWORD)BVEWaveOutProc, (DWORD)this, CALLBACK_FUNCTION);
	if (mr != MMSYSERR_NOERROR)
	{
		m_hWaveOut = NULL;
		return false;
	}
	if (!AllocBuffer ())
		return false;
	if (m_dwVolume != 0XFF)
		waveOutSetVolume (m_hWaveOut, m_dwVolume);
	m_llBuffTime = 0;
	m_bFirstRnd = true;
	return true;
}

int CWaveOutRnd::Uninit (void) {
	if (m_hWaveOut == NULL)
		return 0;
	Stop ();
	ReleaseBuffer();
	MMRESULT mr = 0;
	int nTryTimes = 0;
	do {
		mr = waveOutClose (m_hWaveOut);
		if (mr == WAVERR_STILLPLAYING)
			Sleep(10);
		nTryTimes++;
		if (nTryTimes > 100) {
			break;
		}
	} while (mr == WAVERR_STILLPLAYING);
	m_hWaveOut = NULL;
	if (mr != MMSYSERR_NOERROR)
		return -1;
	else
		return 0;
}

int	CWaveOutRnd::Start (void) {
	if (m_hWaveOut == NULL)
		InitDevice ();
	if (m_hWaveOut == NULL)
		return -1;
	MMRESULT mr = waveOutRestart (m_hWaveOut);
	if (mr != MMSYSERR_NOERROR) {
		return -1;
	}
	m_bReseted = false;
	m_nWriteCount = 0;
	return 0;
}

int	CWaveOutRnd::Stop (void) {
	if (m_bReseted)
		return 0;
	if (m_hWaveOut == NULL) {
		return -1;
	}
	WaitAllBufferDone (1200);
	MMRESULT mr = waveOutReset (m_hWaveOut);
	if (mr != MMSYSERR_NOERROR) {
		return -1;
	}
	m_bReseted = true;
	return 0;
}

int CWaveOutRnd::Flush (void) {
	WaitAllBufferDone (1200);
	m_nPCMLen = 0;
	m_nWriteCount = 0;

	WAVEHDR *	pHead = NULL;
	NODEPOS	pos = m_lstFree.GetHeadPosition ();
	while (pos != NULL) {
		pHead = m_lstFree.GetNext (pos);
		if (pHead != NULL) {
			((WAVEHDRINFO *)pHead->dwUser)->llTime = -1;
			pHead->dwBufferLength = 0;
		}
	}
	m_bFirstRnd = true;
	return 0;
}

int CWaveOutRnd::Render (unsigned char * pBuff, int nBuffSize, long long llTime) {
	if (pBuff == NULL || nBuffSize <= 0)
		return -1;
	if (m_lstFree.GetCount () <= 0)	{
		Sleep (5);
		return -1;
	}

	WAVEHDR *		pHead = m_lstFree.GetHead ();
	unsigned char *	pData = NULL;
	int				nSize = 0;

	// Render the PCM data first.
	if (m_pPCMBuff != NULL && m_nPCMLen > 0) {
		if (pHead->dwBufferLength + m_nPCMLen > m_nBufSize) {
			int nCopySize = m_nBufSize - pHead->dwBufferLength;
			memcpy (pHead->lpData + pHead->dwBufferLength, m_pPCMBuff, nCopySize);
			pHead->dwBufferLength += nCopySize;
			m_pPCMBuff += nCopySize;
			m_nPCMLen -= nCopySize;
			m_llBuffTime += (nCopySize * 1000) / m_wavFormat.nAvgBytesPerSec;
		}
		else {
			memcpy (pHead->lpData + pHead->dwBufferLength, m_pPCMBuff, m_nPCMLen);
			pHead->dwBufferLength += m_nPCMLen;
			m_pPCMBuff = m_pPCMData;
			m_nPCMLen = 0;
		}
	}	

	if (pHead->dwBufferLength < m_nBufSize) {
		m_llBuffTime = llTime;
		pData = pBuff;
		nSize = nBuffSize;
		if (pHead->dwBufferLength + nSize > m_nBufSize) {
			int nCopySize = m_nBufSize - pHead->dwBufferLength;
			memcpy (pHead->lpData + pHead->dwBufferLength, pData, nCopySize);
			if (m_nPCMSize < nSize) {
				m_nPCMSize = nSize;
				delete[](m_pPCMData);
			}
			if (m_pPCMData == NULL) {
				m_nPCMSize = m_wavFormat.nSamplesPerSec;
				if (m_nPCMSize < nSize)
					m_nPCMSize = nSize;
				m_pPCMData = new unsigned char[m_nPCMSize];
				m_pPCMBuff = m_pPCMData;
				m_nPCMLen = 0;
			}
			if (m_nPCMLen == 0) {
				m_nPCMLen = pHead->dwBufferLength + nSize - m_nBufSize;
				memcpy (m_pPCMBuff, pData + nCopySize, m_nPCMLen);
			}
			else {
				m_pPCMBuff += nCopySize;
				m_nPCMLen -= nCopySize;
			}
			pHead->dwBufferLength = m_nBufSize;
		}
		else {
			memcpy (pHead->lpData + pHead->dwBufferLength, pData, nSize);
			pHead->dwBufferLength += nSize;
			if (m_nPCMLen > 0) {
				m_pPCMBuff += nSize;
				m_nPCMLen -= nSize;
			}
		}
	}
	if (((WAVEHDRINFO *)pHead->dwUser)->llTime == -1)
		((WAVEHDRINFO *)pHead->dwUser)->llTime = m_llBuffTime;
	if (pHead->dwBufferLength == m_nBufSize) {
		// lock in short time
		if (pHead != NULL) {
			pHead = m_lstFree.RemoveHead ();
		}
		//BVELOGI ("Time % 8d  % 8d", (int)((WAVEHDRINFO *)pHead->dwUser)->llTime, (int)pBuff->llTime);
		MMRESULT mr = waveOutWrite (m_hWaveOut, pHead, sizeof (WAVEHDR));
		if (mr != MMSYSERR_NOERROR) {
			m_lstFree.AddTail (pHead);
			return -1;
		}
		if (pHead != NULL) {
			m_lstFull.AddTail (pHead);
		}
		m_bFirstRnd = false;
	}
	if (m_nPCMLen >= (int)m_nBufSize)
		return -1;
	return 0;
}

bool CWaveOutRnd::AllocBuffer (void) {
	ReleaseBuffer ();

	WAVEHDR * pWaveHeader = NULL;
	for (int i = 0; i < MAXINPUTBUFFERS; i++) {
		pWaveHeader = new WAVEHDR;
		memset (pWaveHeader, 0, sizeof (WAVEHDR));
		WAVEHDRINFO * pWavInfo = new WAVEHDRINFO ();
		pWaveHeader->dwUser = (DWORD_PTR)pWavInfo;
		m_lstFree.AddTail (pWaveHeader);
	}

	m_nBufSize = m_wavFormat.nAvgBytesPerSec / 10;
	if (m_nBufSize < 1600)
		m_nBufSize = 1600;
	m_nBufSize = (m_nBufSize + m_wavFormat.nBlockAlign - 1) / m_wavFormat.nBlockAlign * m_wavFormat.nBlockAlign;
	m_nOffsetTime = (m_nBufSize * 1000 / m_wavFormat.nAvgBytesPerSec) * MAXINPUTBUFFERS;

	char *		pBuffer = NULL;
	NODEPOS pos = m_lstFree.GetHeadPosition ();
	while (pos != NULL) {
		pBuffer = new char[m_nBufSize];
		if (pBuffer == NULL)
			return false;
		memset (pBuffer, 0, m_nBufSize);

		pWaveHeader = m_lstFree.GetNext (pos);
		if(pWaveHeader->lpData != NULL)
			delete []pWaveHeader->lpData;
		pWaveHeader->lpData = pBuffer;
		pWaveHeader->dwBufferLength = m_nBufSize;
		pWaveHeader->dwFlags = WHDR_ENDLOOP;//WHDR_INQUEUE | WHDR_DONE;
		pWaveHeader->reserved = 0;

		MMRESULT mr = MMSYSERR_NOMEM;
		mr = waveOutPrepareHeader (m_hWaveOut, pWaveHeader, sizeof (WAVEHDR));
		if (mr != MMSYSERR_NOERROR) {
		}
		pWaveHeader->dwBufferLength = 0;
		((WAVEHDRINFO *)pWaveHeader->dwUser)->llTime = -1;
	}
	return true;
}

bool CWaveOutRnd::ReleaseBuffer (void) {
	if (m_lstFull.GetCount () > 0 || m_lstFree.GetCount ())
		WaitAllBufferDone (1000);
	WAVEHDR * pWaveHeader = NULL;
	while (m_lstFull.GetCount () > 0) {
		pWaveHeader = m_lstFull.RemoveHead ();
		m_lstFree.AddTail (pWaveHeader);
	}

	NODEPOS pos = m_lstFree.GetHeadPosition ();
	while (pos != NULL) {
		pWaveHeader = m_lstFree.GetNext (pos);
		if (pWaveHeader->lpData != NULL) {
			MMRESULT mr = MMSYSERR_NOMEM;
			mr = waveOutUnprepareHeader(m_hWaveOut, pWaveHeader, sizeof (WAVEHDR));
			if (mr != MMSYSERR_NOERROR) {
			}
			delete []pWaveHeader->lpData;
			pWaveHeader->lpData = NULL;
		}
		//memset (pWaveHeader, 0, sizeof(WAVEHDR));
	}
	while (m_lstFree.GetCount () > 0) {
		pWaveHeader = m_lstFree.RemoveHead ();
		WAVEHDRINFO * pWavInfo = (WAVEHDRINFO *)pWaveHeader->dwUser;
		delete pWavInfo;
		delete pWaveHeader;
	}
	return true;
}

int CWaveOutRnd::WaitAllBufferDone (int nWaitTime) {
	int nStartTime = GetTickCount ();
	while (m_lstFree.GetCount () < MAXINPUTBUFFERS) {
		if (m_lstFull.GetCount () == 0)
			break;
		Sleep (2);
		if ((int)(GetTickCount () - nStartTime) >= nWaitTime) {
			return nWaitTime;
		}
	}
	return GetTickCount () - nStartTime;
}

int CWaveOutRnd::UpdateFormat (void) {
	memset(&m_wavFormat, 0, sizeof (WAVEFORMATEX));		
	// cbSize(extra information size) should be 0!!
	// m_wavFormat.cbSize = sizeof (WAVEFORMATEX);
	m_wavFormat.nSamplesPerSec = m_nSampleRate;
	m_wavFormat.nChannels = (WORD)m_nChannels;
	m_wavFormat.wBitsPerSample = 16;
	if (m_wavFormat.wBitsPerSample == 0)
		m_wavFormat.wBitsPerSample = 16;

	if (m_wavFormat.nSamplesPerSec <= 8000)
		m_wavFormat.nSamplesPerSec = 8000;
	else if (m_wavFormat.nSamplesPerSec <= 11025)
		m_wavFormat.nSamplesPerSec = 11025;
	else if (m_wavFormat.nSamplesPerSec <= 12000)
		m_wavFormat.nSamplesPerSec = 12000;
	else if (m_wavFormat.nSamplesPerSec <= 16000)
		m_wavFormat.nSamplesPerSec = 16000;
	else if (m_wavFormat.nSamplesPerSec <= 22050)
		m_wavFormat.nSamplesPerSec = 22050;
	else if (m_wavFormat.nSamplesPerSec <= 24000)
		m_wavFormat.nSamplesPerSec = 24000;
	else if (m_wavFormat.nSamplesPerSec <= 32000)
		m_wavFormat.nSamplesPerSec = 32000;
	else if (m_wavFormat.nSamplesPerSec <= 44100)
		m_wavFormat.nSamplesPerSec = 44100;
	else if (m_wavFormat.nSamplesPerSec <= 48000)
		m_wavFormat.nSamplesPerSec = 48000;
	if (m_wavFormat.nChannels > 2)
		m_wavFormat.nChannels = 2;
	else if (m_wavFormat.nChannels <= 0)
		m_wavFormat.nChannels = 1;
	m_wavFormat.nBlockAlign = (WORD)(m_wavFormat.nChannels * 2);
	m_wavFormat.nAvgBytesPerSec = m_wavFormat.nSamplesPerSec * m_wavFormat.nBlockAlign;
	m_wavFormat.wFormatTag = 1;
	return 0;
}

bool CWaveOutRnd::AudioDone (WAVEHDR * pWaveHeader) {
	if (pWaveHeader != NULL) {
		if (((WAVEHDRINFO *)pWaveHeader->dwUser)->llTime >= 0) {
			//m_llRendTime = ((WAVEHDRINFO *)pWaveHeader->dwUser)->llTime;
			//m_llRendTime = m_llRendTime + pWaveHeader->dwBufferLength * 1000 / m_wavFormat.nAvgBytesPerSec;
			//if (!m_bFlushing)
			//	m_pInst->m_pClock->SetTime (m_llRendTime);
			PostMessage(m_hWnd, WM_USER_ARND, (WPARAM)((WAVEHDRINFO *)pWaveHeader->dwUser)->llTime, 0);
		}
	}
	m_lstFull.Remove (pWaveHeader);
	pWaveHeader->dwBufferLength = 0;
	((WAVEHDRINFO *)pWaveHeader->dwUser)->llTime = -1;
	m_lstFree.AddTail (pWaveHeader);
	return true;
}

bool CALLBACK CWaveOutRnd::BVEWaveOutProc(HWAVEOUT hwo, UINT uMsg,  DWORD dwInstance, 
										  DWORD dwParam1, DWORD dwParam2)
{
	CWaveOutRnd * pRender = (CWaveOutRnd *)dwInstance;
	switch (uMsg) {
	case WOM_CLOSE:
		break;
	case WOM_OPEN:
		break;
	case WOM_DONE:
		pRender->AudioDone ((WAVEHDR *)dwParam1);
		break;
	default:
		break;
	}
	return true;
}
