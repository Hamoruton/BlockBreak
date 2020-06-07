#include <Windows.h>
#include <crtdbg.h>
#include <tchar.h>
#include <stdio.h>
#include <d2d1.h>

#include "Selector.h"

#pragma comment(lib,"d2d1.lib")

//�}�N���錾
#define SCREEN_WIDTH	600
#define SCREEN_HEIGHT	800

#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }

//�O���[�o���ϐ�
ID2D1Factory			*g_pD2DFactory = NULL;		// D2D1Factory�I�u�W�F�N�g
ID2D1HwndRenderTarget	*g_pRenderTarget = NULL;	// �`��^�[�Q�b�g
DWORD					 g_dwCount = 0;				// �J�E���g�l
CSelector				*g_pSystem = NULL;

 // �v���g�^�C�v�錾
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//================================================================================
// �A�v���P�[�V�����E�G���g���[�֐�
//================================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// �f�o�b�O�p�t���O�̃Z�b�g
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// �ϐ��錾
	WNDCLASSEX wcex;		 //  �E�B���h�E�N���X�\����
	HWND hWnd;				 //  �E�B���h�E�n���h��
	RECT bounds, client;	 //  RECT �\����

	/**
	 * �A�v���P�[�V����������
	 **/

	CoInitialize(NULL);		 // COM�̏�����

	 // �E�B���h�E�N���X�̓o�^
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = (WNDPROC)WndProc; 
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("BlockBreak");
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wcex);

	// �E�B���h�E�̐���
	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, wcex.lpszClassName, _T("BlockBreak"),
		WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;

	// �E�B���h�E�T�C�Y�̒���
	GetWindowRect(hWnd, &bounds);
	GetClientRect(hWnd, &client);
	MoveWindow(hWnd, 0, 0,
		SCREEN_WIDTH * 2 -client.right,
		SCREEN_HEIGHT * 2 - client.bottom,
		false);

	// �E�B���h�E�̍ĕ\��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Direct2D ������
	{
		HRESULT hr;
		// Direct2D Factory �̐���
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
		GetClientRect(hWnd, &client);
		D2D1_SIZE_U size = D2D1::SizeU(
			client.right - client.left,
			client.bottom - client.top
		);

		// Direct2D �`��^�[�Q�b�g�̐���
		hr = g_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			&g_pRenderTarget
		);
	}

	// �Z���N�^�[�N���X����
	g_pSystem = new CSelector(g_pRenderTarget);

	// ���b�Z�[�W���[�v
	MSG        msg;
	while (true)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_dwCount++;
			if (g_pSystem)
				g_pSystem->Anim();
			InvalidateRect(hWnd, NULL, true);	//��ʏ��������w��
		}
	}

	//�I������
	SAFE_DELETE(g_pSystem);
	SAFE_RELEASE(g_pRenderTarget);
	SAFE_RELEASE(g_pD2DFactory);

	CoUninitialize();		//COM�̎g�p�I��

	return (int)msg.wParam;
}

//================================================================================
// �E�B���h�E�v���V�[�W���֐�
//================================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_PAINT:
		if (g_pRenderTarget != NULL) 
		{
			g_pRenderTarget->BeginDraw();
			g_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
			g_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));

			if(g_pSystem)
				g_pSystem->Draw(g_pRenderTarget);

			g_pRenderTarget->EndDraw();
		}
		ValidateRect(hWnd, NULL);

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return    0;
}