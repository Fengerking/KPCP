/*******************************************************************************
	File:		CCheckSelectResult.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "stdio.h"
#include "stdlib.h"
#include "tchar.h"

#include "CCheckSelectResult.h"

CCheckSelectResult::CCheckSelectResult(void) {
	m_nWidth = 640;
	m_nHeight = 480;
	m_nBValue = 80;

	m_pFileBuff = NULL;
	m_ppBlackPixel = NULL;
	m_nLinePCount = 0;
	m_ppLinePixel = NULL;
	m_nLinePCount = NULL;

	m_nFindLines = 0;
}

CCheckSelectResult::~CCheckSelectResult(void) {
	releaseBuffs();
}

int	CCheckSelectResult::releaseBuffs(void) {
	if (m_pFileBuff != NULL)
		delete[]m_pFileBuff;
	m_pFileBuff = NULL;
	if (m_ppBlackPixel != NULL) {
		for (int i = 0; i < m_nLinePCount; i++) {
			if (m_ppBlackPixel[i] != NULL)
				delete m_ppBlackPixel[i];
		}
		delete[]m_ppBlackPixel;
	}
	m_ppLinePixel = NULL;
	if (m_ppLinePixel != NULL) {
		for (int i = 0; i < m_nLinePCount; i++) {
			if (m_ppLinePixel[i] != NULL)
				delete m_ppLinePixel[i];
		}
		delete[]m_ppLinePixel;
	}
	m_ppLinePixel = NULL;
	return 0;
}

int comparPixelX(const void *arg1, const void *arg2) {
	pixelInfo * pItem1 = *(pixelInfo **)arg1;
	pixelInfo * pItem2 = *(pixelInfo **)arg2;
	return (int)(pItem1->nX - pItem2->nX);
}

int comparPixelY(const void *arg1, const void *arg2) {
	pixelInfo * pItem1 = *(pixelInfo **)arg1;
	pixelInfo * pItem2 = *(pixelInfo **)arg2;
	return (int)(pItem1->nY - pItem2->nY);
}

int CCheckSelectResult::OpenFile(const char * pYUVFile) {
	FILE * hFile = NULL;
	fopen_s(&hFile, pYUVFile, "r");
	if (hFile == NULL)
		return -1;
	releaseBuffs();

	int nSize = m_nWidth * m_nHeight;
	int i, w, h;
	int nValue = 0;
	int nIndex = 0;

	m_pFileBuff = new unsigned char[nSize];
	fread(m_pFileBuff, 1, nSize, hFile);
	fclose(hFile);

	char	szDebugTxt[256];
	// statistic every value pixels
	int nPixelCount[256];
	memset(nPixelCount, 0, sizeof(nPixelCount));
	for (i = 0; i < nSize; i++) {
		nPixelCount[m_pFileBuff[i]]++;
	}
	// statistic total black pixels
	m_nBlackCount = 0;
	for (i = 0; i < m_nBValue; i++) {
		m_nBlackCount += nPixelCount[i];
	}

	// find out the every black pixels and sort by X pos
	m_ppBlackPixel = new pixelInfo*[m_nBlackCount];
	nIndex = 0;
	for (w = 0; w < m_nWidth; w++) {
		for (h = 0; h < m_nHeight; h++) {
			nValue = m_pFileBuff[h * m_nWidth + w];
			if (nValue < m_nBValue) {
				m_ppBlackPixel[nIndex] = new pixelInfo();
				m_ppBlackPixel[nIndex]->nValue = nValue;
				m_ppBlackPixel[nIndex]->nX = w;
				m_ppBlackPixel[nIndex]->nY = h;
				nIndex++;
			}
		}
	}
	qsort(m_ppBlackPixel, m_nBlackCount, sizeof(pixelInfo *), comparPixelX);

	m_nLinePCount = m_nBlackCount;
	m_ppLinePixel = new pixelInfo*[m_nLinePCount];
	for (i = 0; i < m_nLinePCount; i++) {
		m_ppLinePixel[i] = new pixelInfo();
	}

	for (i = 0; i < 64; i++) {
		m_rcLines[i].nX = m_nWidth; m_rcLines[i].nR = 0;
		m_rcLines[i].nY = m_nHeight; m_rcLines[i].nB = 0;
	}
	m_nFindLines = 0;
	int			nFoundX = 0;
	int			nFoundY = 0;
	int			nStartX = 0;
	int			nStartI = 0;
	for (i = 0; i < m_nBlackCount - 1; i++) {
		if (nStartX == 0) {
			nStartI = i;
			nStartX = m_ppBlackPixel[i]->nX;
		}
		if (m_ppBlackPixel[i]->nX - nStartX > 2) {
			if (i - nStartI < 20) {
				nStartX = 0;
				nFoundX = 0;
			}
		}

		if (m_ppBlackPixel[i + 1]->nX - m_ppBlackPixel[i]->nX > 1) {
			if (nFoundX < 150) {
				nFoundX = 0;
				continue;
			}

			checkLines(m_ppLinePixel, nFoundX);

			nStartX = 0;
			nFoundX = 0;
		}
		memcpy(m_ppLinePixel[nFoundX], m_ppBlackPixel[i], sizeof(pixelInfo));
		nFoundX++;
	}

	if (nFoundX > 150) {
		checkLines(m_ppLinePixel, nFoundX);
	}

	checkResult();

	return 1;
}

int CCheckSelectResult::checkResult(void) {
	memset(&m_nSelResult[0][0], 0, sizeof(m_nSelResult));
	int nJudgePixels = 20;
	for (int i = 0; i < m_nFindLines - 1; i++) {
		int nFindPixels = 0;
		for (int w = m_rcLines[i].nR + 2; w < m_rcLines[i+1].nX - 2; w++) {
			int nH = m_rcLines[i].nY + (m_rcLines[i].nB - m_rcLines[i].nY) / 2 - 2;
			for (int h = m_rcLines[i].nY + 2; h <nH; h++) {
				if (*(m_pFileBuff + h * m_nWidth + w) < m_nBValue) {
					nFindPixels++;
				}
			}
		}
		if (nFindPixels > nJudgePixels) {
			m_nSelResult[i][1] = 1;
		}

		nFindPixels = 0;
		for (int w = m_rcLines[i].nR + 2; w < m_rcLines[i + 1].nX - 2; w++) {
			int nH = m_rcLines[i].nY + (m_rcLines[i].nB - m_rcLines[i].nY) / 2 + 2;
			for (int h = nH; h < m_rcLines[i].nB - 2; h++) {
				if (*(m_pFileBuff + h * m_nWidth + w) < m_nBValue) {
					nFindPixels++;
				}
			}
		}
		if (nFindPixels > nJudgePixels) {
			m_nSelResult[i][0] = 1;
		}
	}
	return 1;
}

int	CCheckSelectResult::checkLines(pixelInfo ** ppInfo, int nFoundX) {
	qsort(ppInfo, nFoundX, sizeof(pixelInfo *), comparPixelY);
	int nFoundY = 0;
	for (int j = 0; j < nFoundX - 1; j++) {
		if (m_ppLinePixel[j + 1]->nY - m_ppLinePixel[j]->nY > 3) {
			if (nFoundY > nFoundX * 6 / 10) {
				break;
			}
			m_rcLines[m_nFindLines].nX = m_nWidth; m_rcLines[m_nFindLines].nR = 0;
			m_rcLines[m_nFindLines].nY = m_nHeight; m_rcLines[m_nFindLines].nB = 0;
			nFoundY = 0;
			if (nFoundY < nFoundX / 10) {
				continue;
			}
		}
		if (m_rcLines[m_nFindLines].nX > m_ppLinePixel[j]->nX)
			m_rcLines[m_nFindLines].nX = m_ppLinePixel[j]->nX;
		if (m_rcLines[m_nFindLines].nR < m_ppLinePixel[j]->nX)
			m_rcLines[m_nFindLines].nR = m_ppLinePixel[j]->nX;
		if (m_rcLines[m_nFindLines].nY > m_ppLinePixel[j]->nY)
			m_rcLines[m_nFindLines].nY = m_ppLinePixel[j]->nY;
		if (m_rcLines[m_nFindLines].nB < m_ppLinePixel[j]->nY)
			m_rcLines[m_nFindLines].nB = m_ppLinePixel[j]->nY;
		nFoundY++;
	}
	if (nFoundY > nFoundX * 6 / 10) {
		m_nFindLines++;
	}
	return 1;
}


