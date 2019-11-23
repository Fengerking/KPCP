/*******************************************************************************
	File:		CMusicPage.cpp

	Contains:	Window slide pos implement code

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#include "windows.h"
#include "tchar.h"

#include "CMusicPage.h"
int g_aNearPos[8][2] = { {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1} };

CMusicPage::CMusicPage(void) {
	m_nBValue = 128;
	m_nWidth = 0;
	m_nHeight = 0;
}

CMusicPage::~CMusicPage(void) {

}

int	CMusicPage::GetRightWidth(MusicNote * pLeft) {
	int		nRightWidth = 0;
	int		nNoteWidth = pLeft->m_nWidth;
	MusicNote * pNote = NULL;
	NODEPOS pos = m_lstNote.GetHeadPosition();
	while (pos != NULL) {
		pNote = m_lstNote.GetNext(pos);
		if (pNote == pLeft) {
			pNote = m_lstNote.GetNext(pos);
			if (pNote == NULL) {
				nRightWidth = pLeft->m_pLine->m_nLeft;
			}
			else {
				if (pNote->m_pLine == pLeft->m_pLine) {
					nRightWidth = pNote->m_nLeft;
				}
				else {
					nRightWidth = pLeft->m_pLine->m_nLeft;
				}
			}
			break;
		}
	}
	if (nRightWidth < nNoteWidth * 2)
		return nRightWidth - 2;
	else if (nRightWidth < nNoteWidth * 4)
		return nRightWidth - 6;
	else
		return nRightWidth - 10;
}

int	CMusicPage::ParseObject(unsigned char * pData) {
	CImgObject * pImgObj = NULL;

	for (int h = 0; h < m_nHeight; h++) {
		for (int w = 0; w < m_nWidth; w++) {
			if (*(pData + h * m_nWidth + w) > m_nBValue) {
				if (!hadFilled(w, h)) {
					fillObjPixel(pImgObj, pData, w, h);
				}
			}
		}
	}

	return 0;
}

bool CMusicPage::hadFilled(int nX, int nY) {
	CImgObject * pImgObj = NULL;
	NODEPOS pos = m_lstObject.GetHeadPosition();
	while (pos != NULL) {
		pImgObj = m_lstObject.GetNext(pos);
		if (pImgObj->InList(nX, nY))
			return true;
	}
	return false;
}

int	CMusicPage::fillObjPixel(CImgObject * pImgObj, unsigned char * pData, int nX, int nY) {
	if (pImgObj == NULL) {
		pImgObj = new CImgObject();
		m_lstObject.AddTail(pImgObj);
		pImgObj->AddPix(nX, nY);
	}

	for (int i = 0; i < 8; i++) {
		if (*(pData + (nY + g_aNearPos[i][1]) * m_nWidth + (nX + g_aNearPos[i][0])) > m_nBValue) {
			nX = nX + g_aNearPos[i][0];
			nY = nY + g_aNearPos[i][1];
			if (pImgObj->AddPix(nX, nY) > 0)
				fillObjPixel(pImgObj, pData, nX, nY);
		}
	}

	return 1;
}

CImgObject::CImgObject(void) {
	m_nLeft = -1;
	m_nTop = -1;
	m_nRight = -1;
	m_nBottom = -1;
}

CImgObject::~CImgObject(void) {
	PixPoint * pPix = m_lstPixel.RemoveHead();
	while (pPix != NULL) {
		delete pPix;
		pPix = m_lstPixel.RemoveHead();
	}
}

int	CImgObject::AddPix(int nX, int nY) {
	if (InList(nX, nY))
		return 0;

	PixPoint * pPix = new PixPoint();
	pPix->nX = nX;
	pPix->nY = nY;
	m_lstPixel.AddTail(pPix);
	if (m_nLeft < 0) {
		m_nLeft = nX; m_nRight = nX; m_nTop = nY; m_nBottom = nY;
	}
	if (m_nLeft > nX)
		m_nLeft = nX;
	if (m_nRight < nX)
		m_nRight = nX;
	if (m_nTop > nY)
		m_nTop = nY;
	if (m_nBottom < nY)
		m_nBottom = nY;
	return 1;
}

bool CImgObject::InList(int nX, int nY) {
	if (m_lstPixel.GetCount() <= 0)
		return false;
	if (nX < m_nLeft || nX > m_nRight || nY < m_nTop || nY > m_nBottom)
		return false;
	PixPoint * pPix = NULL;
	NODEPOS pos = m_lstPixel.GetHeadPosition();
	while (pos != NULL) {
		pPix = m_lstPixel.GetNext(pos);
		if (pPix->nX == nX && pPix->nY == nY)
			return true;
	}
	return false;
}