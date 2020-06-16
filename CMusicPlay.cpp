/*******************************************************************************
	File:		CMusicPlay.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "math.h"
#include "tchar.h"

#include "CMusicPlay.h"

CMusicPlay::CMusicPlay(HWND hWnd) {
	m_hWnd = hWnd;
	m_pPage = NULL;
	m_pRnd = NULL;
	m_nOffset = 44100 * 4 * 4;
	m_nStep = 505672;
	m_nLowLen = m_nStep * 5;
	m_nBuffSize = 4096;
	char szFile[256];
	memset(szFile, 0, sizeof(szFile));
	GetModuleFileName(NULL, szFile, sizeof(szFile));
	char * pFolder = strrchr(szFile, '\\');
	if (pFolder != NULL)
		*pFolder = 0;
	strcat(szFile, "\\yinseku.pcm");
	m_hFile = fopen(szFile, "rb");
	testNotePlay();

	m_pRnd = new CWaveOutRnd(m_hWnd);
	m_pRnd->Init(44100, 2);
	m_pRnd->Start();
}

CMusicPlay::~CMusicPlay(void) {
	if (m_pRnd != NULL)
		delete m_pRnd;
	fclose(m_hFile);
}

void CMusicPlay::SetMusicPage(CMusicPage * pPage) {
	m_pPage = pPage;
}

int	CMusicPlay::Play(void) {
	MusicNote * pNote = NULL;
	NODEPOS		pos = m_pPage->m_lstNote.GetHeadPosition();
	while (pos != NULL) {
		pNote = m_pPage->m_lstNote.GetNext(pos);
		PlayNote(pNote);
	}
	return 0;
}

int	CMusicPlay::PlayNote(MusicNote * pNote) {
	int nRead = m_nBuffSize;
	int nPlayLen = 0;
	int nRC = 0;
	int nOffset = 0;
	int nLenght = 0;

	if (pNote->m_nHighLevel == 0) {
		nOffset = m_aOffPos[5 + pNote->m_nNote - 1];
	}
	else if (pNote->m_nHighLevel == 1) {
		nOffset = m_aOffPos[12];
	}
	else if (pNote->m_nHighLevel == -1) {
		nOffset = m_aOffPos[pNote->m_nNote - 3];
	}
	else {
		nOffset = m_aOffPos[5 + pNote->m_nNote - 1];
	}
	if (pNote->m_nNote == 0) {
		nOffset = 0;
	}
	int nNum = (int)(pow(2, 4 - pNote->m_nLength));
	nLenght = m_nStep * 4 / nNum;
	nLenght = nLenght * 4 / 4;
	if (nLenght > m_nStep)
		nLenght = m_nStep;

	fseek(m_hFile, nOffset, SEEK_SET);
	nPlayLen = 0;
	while (nPlayLen < nLenght) {
		nRead = fread(m_szBuff, 1, m_nBuffSize, m_hFile);
		while (nRead > 0) {
			nRC = m_pRnd->Render((unsigned char *)m_szBuff, nRead, (long long)pNote);
			if (nRC != 0) {
				Sleep(2);
			}
			else {
				nPlayLen += nRead;
				break;
			}
		}
	}
	return 0;
}

int	CMusicPlay::Pause(void) {
	return 0;
}

int CMusicPlay::Seek(int nPos) {
	return 0;
}

int CMusicPlay::FillPcmData(void) {
	return 0;
}

int CMusicPlay::SmoothPCMData(void) {
	return 0;
}

int CMusicPlay::testNotePlay(void) {
	int		nFileSize = 7279324;
	char *	pFileBuff = new char[nFileSize];
	int		nRead = fread(pFileBuff, 1, nFileSize, m_hFile);
	short * pData = (short *)pFileBuff;
	int		nIndex = 0;
	bool bFound = false;
	while ((char *)pData - pFileBuff < nFileSize) {
		if (abs(*pData) > 4000) {
			if (!bFound) {
				bFound = true;
				m_aOffPos[nIndex++] = (char *)pData - pFileBuff;
				pData += m_nStep / 2;
			}
		}
		else {
			bFound = false;
		}
		pData++;
	}
	for (int i = 0; i < 15; i++) {
		m_aOffPos[i] = m_aOffPos[i] * 4 / 4;
	}


	//int nOffset = 0; 
	//int nLength = 0;
	//int nPlayLen = 0;
	//int nRC = 0;

	//int nNote = 5;
	//nOffset = m_aOffPos[nNote - 3];
	//nLength = m_nStep / 4;
	//nLength = nLength * 4 / 4;

	//fseek(m_hFile, nOffset, SEEK_SET);
	//nPlayLen = 0;
	//while (nPlayLen < nLength) {
	//	nRead = fread(m_szBuff, 1, m_nBuffSize, m_hFile);
	//	while (nRead > 0) {
	//		nRC = m_pRnd->Render((unsigned char *)m_szBuff, nRead, 0);
	//		if (nRC != 0) {
	//			Sleep(2);
	//		}
	//		else {
	//			nPlayLen += nRead;
	//			break;
	//		}
	//	}
	//}

	return 0;
}