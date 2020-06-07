#include <Windows.h>
#include <crtdbg.h>
#include <tchar.h>
#include <stdio.h>
#include <d2d1.h>

#include "Selector.h"

#pragma comment(lib,"d2d1.lib")

//マクロ宣言
#define SCREEN_WIDTH	600
#define SCREEN_HEIGHT	800

#undef SAFE_RELEASE
#undef SAFE_DELETE
#undef SAFE_DELETE_ARRAY
#define SAFE_RELEASE(o) if (o){ (o)->Release(); o = NULL; }
#define SAFE_DELETE(o)  if (o){ delete (o); o = NULL; }
#define SAFE_DELETE_ARRAY(o) if (o){ delete [] (o); o = NULL; }

//グローバル変数
ID2D1Factory			*g_pD2DFactory = NULL;		// D2D1Factoryオブジェクト
ID2D1HwndRenderTarget	*g_pRenderTarget = NULL;	// 描画ターゲット
DWORD					 g_dwCount = 0;				// カウント値
CSelector				*g_pSystem = NULL;

 // プロトタイプ宣言
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//================================================================================
// アプリケーション・エントリー関数
//================================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// デバッグ用フラグのセット
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	// 変数宣言
	WNDCLASSEX wcex;		 //  ウィンドウクラス構造体
	HWND hWnd;				 //  ウィンドウハンドル
	RECT bounds, client;	 //  RECT 構造体

	/**
	 * アプリケーション初期化
	 **/

	CoInitialize(NULL);		 // COMの初期化

	 // ウィンドウクラスの登録
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

	// ウィンドウの生成
	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, wcex.lpszClassName, _T("BlockBreak"),
		WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, NULL, NULL, hInstance, NULL);
	if (!hWnd)
		return FALSE;

	// ウィンドウサイズの調整
	GetWindowRect(hWnd, &bounds);
	GetClientRect(hWnd, &client);
	MoveWindow(hWnd, 0, 0,
		SCREEN_WIDTH * 2 -client.right,
		SCREEN_HEIGHT * 2 - client.bottom,
		false);

	// ウィンドウの再表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Direct2D 初期化
	{
		HRESULT hr;
		// Direct2D Factory の生成
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
		GetClientRect(hWnd, &client);
		D2D1_SIZE_U size = D2D1::SizeU(
			client.right - client.left,
			client.bottom - client.top
		);

		// Direct2D 描画ターゲットの生成
		hr = g_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			&g_pRenderTarget
		);
	}

	// セレクタークラス生成
	g_pSystem = new CSelector(g_pRenderTarget);

	// メッセージループ
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
			InvalidateRect(hWnd, NULL, true);	//画面書き換え指示
		}
	}

	//終了処理
	SAFE_DELETE(g_pSystem);
	SAFE_RELEASE(g_pRenderTarget);
	SAFE_RELEASE(g_pD2DFactory);

	CoUninitialize();		//COMの使用終了

	return (int)msg.wParam;
}

//================================================================================
// ウィンドウプロシージャ関数
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