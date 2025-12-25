#include "Win32InitWindow.h"

#ifdef HELIOS_PLATFORM_WINDOWS

#include <gdiplus.h>

namespace Helios

{
	Win32InitWindow::Win32InitWindow()
	{
		Create(400, 280);
	}

	inline void Win32InitWindow::Create(uint32_t width, uint32_t height)
	{
		// Run InternalInit() asynchronously on a separate thread
		std::thread t(&Win32InitWindow::InternalInit, this, width, height);
		t.detach();
	}

	inline void Win32InitWindow::SetTextA(std::string text)
	{
		m_textA = std::wstring(text.begin(), text.end());
		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	inline void Win32InitWindow::SetTextB(std::string text)
	{
		m_textB = std::wstring(text.begin(), text.end());
		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	inline void Win32InitWindow::SetProgressA(float progress)
	{
		m_progressA = progress;
		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	inline void Win32InitWindow::SetProgressB(float progress)
	{
		m_progressB = progress;
		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	inline void Win32InitWindow::ShowMessage(const char* title, const char* text)
	{
		MessageBoxA(m_hwnd, text, title, MB_OK);
	}

	void Win32InitWindow::InternalInit(uint32_t width, uint32_t height)
	{
		using namespace Gdiplus;

		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR           gdiplusToken;

		// Initialize GDI+.
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		WNDCLASSEX wc;
		ZeroMemory(&wc, sizeof(WNDCLASSEX));

		if (!GetClassInfoEx(GetModuleHandle(nullptr), L"HeliosInitWindow", &wc))
		{
			wc.cbSize = sizeof(WNDCLASSEX);
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = StaticWndProc;
			wc.hInstance = GetModuleHandle(nullptr);
			wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = L"HeliosInitWindow";
			RegisterClassEx(&wc);
		}

		RECT wr = { 0, 0, (LONG)width, (LONG)height };

		int screenwidth = GetSystemMetrics(SM_CXSCREEN);
		int screenheight = GetSystemMetrics(SM_CYSCREEN);

		m_hwnd = CreateWindowEx(
			0,
			L"HeliosInitWindow",
			L"Helios Engine",
			WS_POPUP,
			(screenwidth - width) / 2,
			(screenheight - height) / 2,
			wr.right - wr.left,
			wr.bottom - wr.top,
			nullptr,
			nullptr,
			GetModuleHandle(nullptr),
			this
		);

		ShowWindow(m_hwnd, SW_SHOW);
		UpdateWindow(m_hwnd);

		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (GetMessage(&msg, NULL, 0, 0) && !m_quit)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		GdiplusShutdown(gdiplusToken);
	}

	inline void Win32InitWindow::Quit()
	{
		if (m_hwnd)
		{
			DestroyWindow(m_hwnd);
			m_hwnd = nullptr;
			m_quit = true;
		}
	}

	void GetRoundRectPath(Gdiplus::GraphicsPath* pPath, Gdiplus::Rect r, int dia)
	{
		using namespace Gdiplus;
		
		if (dia > r.Width)    dia = r.Width;
		if (dia > r.Height)    dia = r.Height;
		
		Rect Corner(r.X, r.Y, dia, dia);
		
		pPath->Reset();
		pPath->AddArc(Corner, 180, 90);
		
		if (dia == 20)
		{
			Corner.Width += 1;
			Corner.Height += 1;
			r.Width -= 1; r.Height -= 1;
		}

		// top right
		Corner.X += (r.Width - dia - 1);
		pPath->AddArc(Corner, 270, 90);

		// bottom right
		Corner.Y += (r.Height - dia - 1);
		pPath->AddArc(Corner, 0, 90);

		// bottom left
		Corner.X -= (r.Width - dia - 1);
		pPath->AddArc(Corner, 90, 90);

		// end path
		pPath->CloseFigure();
	}

	void FillRoundedRectangle(Gdiplus::Graphics* pGraphics, Gdiplus::Rect r,Gdiplus::Color color, int radius, int width)
	{
		using namespace Gdiplus;

		int dia = 2 * radius;

		// set to pixel mode
		int oldPageUnit = pGraphics->SetPageUnit(UnitPixel);

		// define the pen
		SolidBrush brush(color);

		// get the corner path
		GraphicsPath path;

		// get path
		GetRoundRectPath(&path, r, dia);

		// draw the round rect
		pGraphics->FillPath(&brush, &path);

		//// if width > 1
		//for (int i = 1; i < width; i++)
		//{
		//	// left stroke
		//	r.Inflate(-1, 0);
		//	// get the path
		//	GetRoundRectPath(&path, r, dia);

		//	// draw the round rect
		//	pGraphics->DrawPath(&pen, &path);

		//	// up stroke
		//	r.Inflate(0, -1);

		//	// get the path
		//	GetRoundRectPath(&path, r, dia);

		//	// draw the round rect
		//	pGraphics->DrawPath(&pen, &path);
		//}

		// restore page unit
		pGraphics->SetPageUnit((Unit)oldPageUnit);
	}

	LRESULT Win32InitWindow::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_CREATE)
		{
			LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
			Win32InitWindow* pThis = (Win32InitWindow*)pcs->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);
		}
		else
		{
			Win32InitWindow* pThis = (Win32InitWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (pThis)
			{
				//return pThis->WndProc(hwnd, msg, wParam, lParam);
				PAINTSTRUCT ps;
				HDC hdc;
				
				switch (msg)
				{
				case WM_PAINT:
					{
						using namespace Gdiplus;

						hdc = BeginPaint(hwnd, &ps);
						Graphics graphics(hdc);
						SolidBrush 	bgBrush(Color(255, 37, 36, 46));
						SolidBrush 	whiteBrush(Color(255, 255, 255, 255));
						SolidBrush  progressedBrush(Color(255, 100, 100, 122));
						Image image(L"Resources/Logo.png");
						Font fonta(L"Arial", 14);
						Font fontb(L"Arial", 10);

						RECT rect;
						GetClientRect(hwnd, &rect);

						//graphics.FillRectangle(&bgBrush, RectF(rect.left, rect.top, rect.right, rect.bottom));
						// Draw rounded rectangle using GDI+ arc
						FillRoundedRectangle(&graphics, Rect(0, 0, 400, 280 ), Color(37, 36, 46), 20, 1);

						graphics.DrawImage(&image, 0, 0, 400, 200);
						graphics.DrawString(L"Helios Engine", -1, &fonta, PointF(60, 10), &whiteBrush);

						RectF txtSize;
						graphics.MeasureString(pThis->m_textA.c_str(), -1, &fonta, PointF(0, 200), &txtSize);
						graphics.DrawString((pThis->m_textA).c_str(), -1, &fonta, PointF(200 - txtSize.Width / 2, 200), &whiteBrush);
						graphics.FillRectangle(&whiteBrush, RectF(50, 225, 300, 4));
						graphics.FillRectangle(&progressedBrush, RectF(50, 225, pThis->m_progressA * 300, 4));


						if (!pThis->m_textB.empty())
						{
							graphics.MeasureString(pThis->m_textB.c_str(), -1, &fontb, PointF(0, 230), &txtSize);
							graphics.DrawString((pThis->m_textB + L" : " + std::to_wstring(pThis->m_progressB)).c_str(), -1, &fontb, PointF(200 - txtSize.Width / 2, 235), &whiteBrush);
							graphics.FillRectangle(&whiteBrush, RectF(50, 255, 300, 4));
							graphics.FillRectangle(&progressedBrush, RectF(50, 255, pThis->m_progressB * 300, 4));
						}

						EndPaint(hwnd, &ps);
						break;
					}
					case WM_DESTROY:
					{
						PostQuitMessage(0);
						break;
					}
					case WM_CLOSE:
					{
						return 0;
						break;
					}
				}
			}
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

#endif