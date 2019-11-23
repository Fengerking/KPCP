/*******************************************************************************
	File:		CImgAnalyse.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"

#include "CImgAnalyse.h"

using namespace cv;
using namespace cv::ml;
using namespace std;

#define PI 3.1415926
int g_aNearPos[8][2] = { {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1} };

CImgAnalyse::CImgAnalyse(HWND hWnd) {
	m_hWnd = hWnd;
	m_pMusicPage = NULL;
	memset(m_szFile, 0, sizeof(m_szFile));
	m_nWidth	= 0;
	m_nHeight	= 0;
	m_nBValue	= 128;
	m_nFillSize = 1024;

	char szXML[256];
	memset(szXML, 0, sizeof(szXML));
	GetModuleFileName(NULL, szXML, sizeof (szXML));
	char * pPos = strrchr(szXML, '\\');	*pPos = 0;
	strcat(szXML, "\\numberDetectKNN.xml");
	m_knnModel = KNearest::load(szXML);
}

CImgAnalyse::~CImgAnalyse(void) {
	CloseFile();
}

int	CImgAnalyse::OpenFile(char * pFile) {
	CloseFile();
	strcpy(m_szFile, pFile);
	m_matImg.release();
	m_matImg = imread(m_szFile, IMREAD_GRAYSCALE);
	if (m_matImg.empty())
		return -1;
	m_nWidth = m_matImg.cols;
	m_nHeight = m_matImg.rows;
	m_pMusicPage->SetSize(m_nWidth, m_nHeight);

	for (int i = 0; i < m_matImg.rows; i++) {
		for (int j = 0; j < m_matImg.cols; j++) {
			if (m_matImg.at<uchar>(i, j) > m_nBValue)
				m_matImg.at<uchar>(i, j) = 0;
			else
				m_matImg.at<uchar>(i, j) = 255;
		}
	}

	if (fillLines() < 0)
		return -1;

	fillNotes();

	fillNoteInfo();

	MusicNote * pNote = NULL;
	int nTop = 0;
	int nMaxW = 0;
	int nMaxH = 0;
	NODEPOS pos = m_pMusicPage->m_lstNote.GetHeadPosition();
	while (pos != NULL) {
		pNote = m_pMusicPage->m_lstNote.GetNext(pos);
		if (nMaxW < pNote->m_nWidth)
			nMaxW = pNote->m_nWidth;
		if (nMaxH < pNote->m_nHeight)
			nMaxH = pNote->m_nHeight;
	}
	pos = m_pMusicPage->m_lstNote.GetHeadPosition();
	while (pos != NULL) {
		pNote = m_pMusicPage->m_lstNote.GetNext(pos);
		if (nTop == 0)
			nTop = pNote->m_nTop;
		if (pNote->m_nTop - nTop > pNote->m_nHeight)
			nTop = pNote->m_nTop;
		pNote->m_nTop = nTop;
		pNote->m_nHeight = nMaxH;
	}

	return 0;
}

int CImgAnalyse::CloseFile(void) {
	if (!m_matImg.empty()) {
		m_matImg.release();
	}
	return 0;
}

int	CImgAnalyse::fillLines(void) {
	int nRC = 0;
	int nLineH	= 20;
	int nSX = 0, nSY = 0;
	int nX = 0, nY = 0;

	while (nY < m_nHeight - nLineH) {
		nRC = searchLine(nX, nY, false, nLineH, nSX, nSY);
		if (nRC <= 0) {
			if (m_pMusicPage->m_lstLine.GetCount () <= 0) {
				MessageBox(m_hWnd, "It can't find the music note line.", "Error", MB_OK);
				return -1;
			}
			return 1;
		}
		addLine(nSX, nSY, nLineH);
		if (nSX > m_nWidth / 3) {
			nX = 0;
			nY = nSY + nLineH + 4;
			clearLines(false);
			continue;
		}

		nX = nSX + 30;
		nY = nSY - 4;
		int nFound = 0;
		while (nX < m_nWidth) {
			nLineH = nLineH - 6;
			if (nLineH < 20)
				nLineH = 20;
			nRC = searchLine(nX, nY, true, nLineH, nSX, nSY);
			if (nRC <= 0) {
				break;
			}
			addLine(nSX, nSY, nLineH);
			nX = nSX + 30;
			nFound++;
		}
		clearLines(nFound > 2);

		nX = 0;
		nY = nSY + nLineH + 2;
		nLineH = nLineH - 6;
		if (nLineH < 20)
			nLineH = 20;
	}
	clearLines(false);
	return 0;
}

int	CImgAnalyse::searchLine(int nX, int nY, bool bRow, int & nLineH, int & nSX, int & nSY) {
	if (m_matImg.empty())
		return -1;
	int nW = m_matImg.cols;
	int nH = m_matImg.rows;

	int		nLines = 0;
	uchar * pBuff = m_matImg.data;
	uchar * pLine = NULL;
	for (int i = nY; i <= nH - nLineH; i++) {
		pBuff = m_matImg.data + i * nW;
		for (int j = nX; j < nW; j++) {
			if (*(pBuff + j) > m_nBValue) {
				pLine = pBuff + j;
				nLines = 0;
				while (*pLine > m_nBValue) {
					nLines++;
					pLine = m_matImg.data + (i + nLines) * nW + j;
					if (i + nLines >= nH)
						break;
				}
				if (nLines > nLineH) {
					nLineH = nLines;
					nSX = j;
					nSY = i;
					return 1;
				}
			}
		}
		if (bRow && i > nY + nLineH / 2) {
			break;
		}
	}
	return 0;
}

int	CImgAnalyse::addLine(int nX, int nY, int nH) {
	MusicLine * pLine = new MusicLine();
	pLine->m_nLeft = nX;
	pLine->m_nTop = nY;
	pLine->m_nWidth = 1;
	pLine->m_nHeight = nH;
	m_lstLine.AddTail(pLine);
	return 0;
}
int	CImgAnalyse::clearLines(bool bFill) {
	MusicLine * pLine = m_lstLine.RemoveHead();
	while (pLine != NULL) {
		if (bFill)
			m_pMusicPage->m_lstLine.AddTail(pLine);
		else
			delete pLine;
		pLine = m_lstLine.RemoveHead();
	}
	return 0;
}

int	CImgAnalyse::fillNotes(void) {
	int			nRC = 0;
	int			nX = 0, nY = 0, nW = 0;
	Rect		rcNum;
	MusicNote *	pNote = NULL;
	MusicLine * pPrev = NULL;
	MusicLine * pLine = NULL;
	NODEPOS pos = m_pMusicPage->m_lstLine.GetHeadPosition();
	while (pos != NULL) {
		pLine = m_pMusicPage->m_lstLine.GetNext(pos);
		nY = pLine->m_nTop + pLine->m_nHeight / 2;
		nW = pLine->m_nLeft;

		if (pPrev != NULL) {
			if (pLine->m_nTop > pPrev->m_nTop + pPrev->m_nHeight)
				nX = 0;
			else
				nX = pPrev->m_nLeft + 4;
		}

		nRC = searchNum(nX, nY, nW, &rcNum);
		while (nRC > 0) {
			if (rcNum.height > 4 && rcNum.height < pLine->m_nHeight * 2 / 3) {
				Rect	rcImg(rcNum.x - 2, rcNum.y - 2, rcNum.width + 6, rcNum.height + 6);
				Mat		matNote = m_matImg(rcImg);
				resize(matNote, matNote, Size(24, 24), (0, 0), (0, 0), INTER_AREA);
				normalize(matNote, matNote, 1., 0., cv::NormTypes::NORM_MINMAX, CV_32FC1);
				int nNote = (int)m_knnModel->predict(matNote.reshape(0, 1));

				pNote = new MusicNote();
				pNote->m_nNote = nNote;
				pNote->m_nLeft = rcNum.x;
				pNote->m_nTop = rcNum.y;
				pNote->m_nWidth = rcNum.width;
				pNote->m_nHeight = rcNum.height;
				pNote->m_bPlaying = false;
				pNote->m_pLine = pLine;
				m_pMusicPage->m_lstNote.AddTail(pNote);

				//Mat matNum = m_matImg(rcImg);
				//sprintf(szFile, "c:\\temp\\testResult\\%d_%d__%d.jpg", rcNum.x, rcNum.y, nNote);
				//imwrite(szFile, matNum);
			}

			nX = rcNum.x + rcNum.width + 8;
			nRC = searchNum(nX, nY, nW, &rcNum);
		}

		pPrev = pLine;
	}

	return 0;
}

int	CImgAnalyse::searchNum(int nX, int nY, int nW, Rect * pRect) {
	int nLeft = m_nWidth;
	int nTop = m_nHeight;
	int nRight	= 0;
	int nBottom = 0;

	m_nCheckIndex = 0;
	m_nFillIndex = 1;
	uchar * pData = m_matImg.data;
	for (int w = nX; w < nW; w++) {
		if (*(pData + nY * m_nWidth + w) > m_nBValue) {
			memset(&m_aFillResult[0][0], 0, sizeof(m_aFillResult));
			m_aFillResult[m_nCheckIndex][0] = w;
			m_aFillResult[m_nCheckIndex][1] = nY;
			fillNumPos();
			break;
		}
	}
	if (m_nFillIndex < 10)
		return 0;

	for (int i = 0; i < m_nFillIndex; i++) {
		if (nLeft > m_aFillResult[i][0])
			nLeft = m_aFillResult[i][0];
		if (nRight < m_aFillResult[i][0])
			nRight = m_aFillResult[i][0];
		if (nTop > m_aFillResult[i][1])
			nTop = m_aFillResult[i][1];
		if (nBottom < m_aFillResult[i][1])
			nBottom = m_aFillResult[i][1];
	}
	pRect->x = nLeft;
	pRect->y = nTop;
	pRect->width = nRight - nLeft;
	pRect->height = nBottom - nTop;
	return 1;
}

int	CImgAnalyse::fillNumPos(void) {
	int nX = m_aFillResult[m_nCheckIndex][0];
	int nY = m_aFillResult[m_nCheckIndex][1];

	int nFill = 0;
	uchar * pData = m_matImg.data;
	for (int i = 0; i < 8; i++) {
		if (*(pData + (nY + g_aNearPos[i][1]) * m_nWidth + (nX + g_aNearPos[i][0])) > m_nBValue) {
			for (int j = 0; j < m_nFillSize; j++) {
				if (m_aFillResult[j][0] == 0) {
					m_aFillResult[j][0] = nX + g_aNearPos[i][0];
					m_aFillResult[j][1] = nY + g_aNearPos[i][1];
					m_nFillIndex++;
					nFill++;
					break;
				}
				if (m_aFillResult[j][0] == nX + g_aNearPos[i][0] && m_aFillResult[j][1] == nY + g_aNearPos[i][1]) {
					break;
				}				
			}
		}
	}
	if (nFill > 0 || m_nCheckIndex + 1 < m_nFillIndex) {
		m_nCheckIndex++;
		fillNumPos();
	}
	return 0;
}

int	CImgAnalyse::detectNum(Rect * pRect) {
	if (m_matImg.empty())
		return -1;
	Mat matNum = m_matImg(*pRect);
	//imshow("roi", matNum);
	//for (int i = 0; i < matNum.rows; i++) {
	//	for (int j = 0; j < matNum.cols; j++) {
	//		if (matNum.at<uchar>(i, j) > 100)
	//			matNum.at<uchar>(i, j) = 0;
	//		else
	//			matNum.at<uchar>(i, j) = 255;
	//	}
	//}
	//imshow("color", matNum);
	resize(matNum, matNum, Size(24, 24), (0, 0), (0, 0), INTER_AREA);
	normalize(matNum, matNum, 1., 0., cv::NormTypes::NORM_MINMAX, CV_32FC1);
	int nNum = (int)m_knnModel->predict(matNum.reshape(0, 1));
	return nNum;
}

int	CImgAnalyse::fillNoteInfo(void) {
	int			nX = 0, nY = 0;
	int			nW = 0, nH = 0;
	int			w = 0, h = 0;
	int			nTotal = 0;
	int			nFoundX, nFoundY;
	uchar *		pBuff = m_matImg.data;
	MusicNote * pNote = NULL;
	NODEPOS pos = m_pMusicPage->m_lstNote.GetHeadPosition();
	while (pos != NULL) {
		pNote = m_pMusicPage->m_lstNote.GetNext(pos);

		// top 
		nX = pNote->m_nLeft;
		nW = pNote->m_nLeft + pNote->m_nWidth;
		nY = pNote->m_pLine->m_nTop;
		nH = pNote->m_nTop + 2;
		nFoundX = 0;
		nFoundY = 0;
		for (h = nY; h < nH; h++) {
			nTotal = 0;
			pBuff = m_matImg.data + h * m_nWidth + nX;
			for (w = nX; w < nW; w++) {
				nTotal += *(pBuff++);
			}
			// has dot?
			if (nTotal >= 255) {
				if (h == nFoundY + 1)
					continue;
				for (w = nX; w < nW; w++) {
					pBuff = m_matImg.data + h * m_nWidth + w;
					if (*pBuff > m_nBValue) {
						int nNearDots = getNearDotNum(w, h);
						if (nNearDots > 2 && nNearDots < 9) {
							pNote->m_nHighLevel += 1;
							nFoundY = h;
						}
						while (*pBuff > m_nBValue) {
							w++;
							if (w >= nW)
								break;
							pBuff = m_matImg.data + h * m_nWidth + w;
						}
					}
				}
			}
		}

		// bottom 
		nX = pNote->m_nLeft;
		nW = pNote->m_nLeft + pNote->m_nWidth;
		nY = pNote->m_nTop + pNote->m_nHeight + 1;
		nH = pNote->m_pLine->m_nTop + pNote->m_pLine->m_nHeight;
		nFoundX = 0; nFoundY = 0;
		for (h = nY; h < nH; h++) {
			nTotal = 0;
			pBuff = m_matImg.data + h * m_nWidth + nX;
			for (w = nX; w < nW; w++) {
				nTotal += *(pBuff++);
			}
			// has under line?
			if (nTotal > 255 * (nW - nX) * 8 / 10) {
				pNote->m_nLength -= 1;

				// next line
				while (++h < nH) {
					nTotal = 0;
					pBuff = m_matImg.data + h * m_nWidth + nX;
					for (w = nX; w < nW; w++) {
						nTotal += *(pBuff++);
					}
					if (nTotal < 255 * (nW - nX) * 7 / 10)
						break;
				}
			}
			else if (nTotal > 255) {
				if (h == nFoundY + 1)
					continue;
				for (w = nX; w < nW; w++) {
					pBuff = m_matImg.data + h * m_nWidth + w;
					if (*pBuff > m_nBValue) {
						int nNearDots = getNearDotNum(w, h);
						if (nNearDots > 2 && nNearDots < 9) {
							pNote->m_nHighLevel -= 1;
							nFoundY = h;
						}
						while (*pBuff > m_nBValue) {
							w++;
							if (w >= nW)
								break;
							pBuff = m_matImg.data + h * m_nWidth + w;
						}
					}
				}
			}
		}

		// right
		nX = pNote->m_nLeft + pNote->m_nWidth;
		nW = m_pMusicPage->GetRightWidth (pNote);
		nY = pNote->m_nTop;
		nH = pNote->m_nTop + pNote->m_nHeight;
		nFoundX = 0; nFoundY = 0;
		for (h = nY; h < nH; h++) {
			nTotal = 0;
			pBuff = m_matImg.data + h * m_nWidth + nX;
			for (w = nX; w < nW; w++) {
				nTotal += *(pBuff++);
			}
			// has length line?
			if (nTotal > 255 * pNote->m_nWidth * 3 / 4) {
				pNote->m_nLength += 2;

				w = nX;
				while (w < nW) {
					pBuff = m_matImg.data + h * m_nWidth + w++;
					if (*pBuff > m_nBValue) {
						pNote->m_nLength += 1;
						while (w < nW) {
							pBuff = m_matImg.data + h * m_nWidth + w++;
							if (*pBuff < m_nBValue) {
								break;
							}
						}
					}
				}

				// next line
				while (++h < nH) {
					nTotal = 0;
					pBuff = m_matImg.data + h * m_nWidth + nX;
					for (w = nX; w < nW; w++) {
						nTotal += *(pBuff++);
					}
					if (nTotal < 255 * pNote->m_nWidth / 2)
						break;
				}
			}
			else if (nTotal > 255) {
				for (w = nX; w < nW; w++) {
					pBuff = m_matImg.data + h * m_nWidth + w;
					if (*pBuff > m_nBValue) {
						int nNearDots = getNearDotNum(w, h);
						if (nNearDots > 2 && nNearDots < 9) {
							pNote->m_nLength += 1;
							break;
						}
					}
				}
				break;
			}
		}
	}
	return 0;
}

int	CImgAnalyse::getNearDotNum(int nX, int nY) {
	m_nCheckIndex = 0;
	m_nFillIndex = 1;
	memset(&m_aFillResult[0][0], 0, sizeof(m_aFillResult));
	m_aFillResult[m_nCheckIndex][0] = nX;
	m_aFillResult[m_nCheckIndex][1] = nY;
	fillNumPos();
	return m_nFillIndex;
}
