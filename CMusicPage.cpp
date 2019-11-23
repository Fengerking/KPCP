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

CMusicPage::CMusicPage(void) {
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
