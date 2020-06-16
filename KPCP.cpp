#include "stdafx.h"
#include <commctrl.h>
#include <Commdlg.h>
#include <winuser.h>
#include <shellapi.h>

#include "KPCP.h"

#include "CWndSource.h"
#include "CWndResult.h"
#include "CMusicPage.h"
#include "CTrainNote.h"
#include "CMusicPlay.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE	hInst;										// current instance
char		szTitle[MAX_LOADSTRING];					// The title bar text
char		szWindowClass[MAX_LOADSTRING];				// the main window class name

CWndSource *	g_wndSource = NULL;
CWndResult *	g_wndResult = NULL;
CMusicPage 		g_musicPage;
CMusicPlay *	g_pMusicPlay = NULL;

NODEPOS			g_posNote = NULL;
MusicNote *		g_musicNote = NULL;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_KPCP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KPCP));
    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KPCP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_GRAYTEXT);
    wcex.lpszMenuName   = MAKEINTRESOURCE(IDC_KPCP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassEx(&wcex);
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL OpenNusicPageFile(HWND hWnd, char * pFile) {
	int					nFlag = 0;
	char				szFile[1024] = { 0 };
	DWORD				dwID = 0;
	OPENFILENAME		ofn;
	if (pFile != NULL) {
		strcpy(szFile, pFile);
	}
	else {
		memset(szFile, 0, sizeof(szFile));
		memset(&(ofn), 0, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFilter = TEXT("JianPu File (*.*)\0*.*\0");
		if (_tcsstr(szFile, _T(":/")) != NULL)
			_tcscpy(szFile, _T("*.*"));
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrTitle = TEXT("Open JianPu File");
		ofn.Flags = OFN_EXPLORER;
		if (!GetOpenFileName(&ofn))
			return FALSE;
	}
	g_wndSource->OpenFile(szFile);
	g_wndResult->UpdateResult();
	return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable
	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		return FALSE;
	}

	RECT rcWnd;
	GetClientRect(hWnd, &rcWnd);
	RECT rcView;
	SetRect(&rcView, 0, 0, rcWnd.right / 2, rcWnd.bottom);
	g_wndSource = new CWndSource(hInst);
	g_wndSource->CreateWnd(hWnd, rcView, RGB(100, 100, 100));
	g_wndSource->SetMusicPage(&g_musicPage);

	SetRect(&rcView, rcWnd.right / 2, 0, rcWnd.right, rcWnd.bottom);
	g_wndResult = new CWndResult(hInst);
	g_wndResult->CreateWnd(hWnd, rcView, RGB(128, 128, 128));
	g_wndResult->SetMusicPage(&g_musicPage);

	char szFile[256];
	strcpy(szFile, "./res/001.jpg");
	OpenNusicPageFile(hWnd, szFile);
	//g_pMusicPlay = new CMusicPlay();
	//g_pMusicPlay->SetMusicPage(&g_musicPage);
	//g_pMusicPlay->Play();
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;

		case ID_FILE_OPEN:
			OpenNusicPageFile(hWnd, NULL);
			break;

		case ID_FILE_CREATE: {
			CTrainNote trainNote(hWnd);
			trainNote.CreateNumPic();
			break;
		}

		case ID_FILE_TRAIN: {
			CTrainNote trainNote(hWnd);
			trainNote.TrainNotePic();
			break;
		}

		case ID_FILE_PLAY: {
			if (g_pMusicPlay == NULL) {
				g_pMusicPlay = new CMusicPlay(hWnd);
				g_pMusicPlay->SetMusicPage(&g_musicPage);
			}
			//g_pMusicPlay->Play();
			g_posNote = g_musicPage.m_lstNote.GetHeadPosition();
			SetTimer(hWnd, 1001, 10, NULL);
			break;
		}

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

	case WM_TIMER: {
		if (g_musicNote != NULL)
			g_musicNote->m_bPlaying = false;

		if (g_posNote == NULL) {
			KillTimer(hWnd, 1001);
			if (g_musicNote != NULL)
				g_musicNote->m_bPlaying = false;
			g_musicNote = NULL;
		}
		else {
			MusicNote * pNote = g_musicPage.m_lstNote.GetNext(g_posNote);
			g_pMusicPlay->PlayNote(pNote);
		}
		break;
	}

	case WM_USER_ARND: {
		MusicNote * pNote = (MusicNote *)wParam;
		if (g_musicNote == pNote)
			break;
		if (g_musicNote != NULL)
			g_musicNote->m_bPlaying = false;
		pNote->m_bPlaying = true;
		g_musicNote = pNote;
		g_wndResult->UpdateResult();
		break;
	}

    case WM_PAINT:  {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;

    case WM_DESTROY:
		g_wndSource->Close();
		delete g_wndSource;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


