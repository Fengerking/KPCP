/*******************************************************************************
	File:		CImgAnalyse.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CImgAnalyse_H__
#define __CImgAnalyse_H__
#include "windows.h"
#include "opencv2/ml.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "CMusicPage.h"

using namespace cv;
using namespace cv::ml;
using namespace std;

class CImgAnalyse
{
public:
	CImgAnalyse(HWND hWnd);
	virtual ~CImgAnalyse(void);

	// 打开简谱图片文件
	virtual int		OpenFile(char * pFile);
	// 关闭文件
	virtual int		CloseFile(void);

	virtual void	SetMusicPage(CMusicPage * pMusicPage) { m_pMusicPage = pMusicPage; }

protected:
	// 查找简谱的分隔线
	virtual int		fillLines(void);
	virtual int		searchLine(int nX, int nY, bool bRow, int & nLineH, int & nSX, int & nSY);
	virtual int		addLine(int nX, int nY, int nH);
	virtual int		clearLines(bool bFill);

	// 查找音符
	virtual int		fillNotes(void);
	virtual int		searchNum(int nX, int nY, int nW, Rect * pRect);
	virtual int		fillNumPos(void);

	// 查找音符附加信息
	virtual int		fillNoteInfo(void);
	virtual int		getNearDotNum(int nX, int nY);

	// 识别音符数字
	virtual int		detectNum(Rect * pRect);

protected:
	HWND			m_hWnd;
	CMusicPage *	m_pMusicPage;
	Ptr<KNearest>	m_knnModel;
	char			m_szFile[256];
	Mat				m_matImg;
	Mat				m_matCanny;

	int				m_nWidth;
	int				m_nHeight;
	uchar			m_nBValue;

	int				m_aNearPos[8][2];
	int				m_aFillResult[1024][2];
	int				m_nFillSize;
	int				m_nCheckIndex;
	int				m_nFillIndex;

	CObjectList<MusicLine>	m_lstLine;
};
#endif //__CImgAnalyse_H__