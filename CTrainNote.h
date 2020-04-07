/*******************************************************************************
	File:		CTrainNote.h

	Contains:	the window view header file

	Written by:	Bangfei Jin

	Change History (most recent first):
	2016-12-29		Bangfei			Create file

*******************************************************************************/
#ifndef __CTrainNote_H__
#define __CTrainNote_H__

#include "opencv2/core.hpp"

class CTrainNote
{
public:
	CTrainNote(HWND hWnd);
	virtual ~CTrainNote(void);

	// 创建各种字体的数字图片，训练材料
	virtual int		CreateNumPic(void);
	// 用OpenCV KNN 算法训练数字识别模型
	virtual int		TrainNotePic(void);

	virtual int		createNumImage(LOGFONT *plf);
	virtual int		saveBmp(HDC hDC, HBITMAP hBitmap, char * FileName);

protected:
	void			fillTextImage(cv::Mat& trainingImages, cv::Mat& trainingLabels);

protected:
	HWND		m_hWnd;
	HDC			m_hFontDC;
	char		m_szPath[256];




};
#endif //__CTrainNote_H__