#include "Splash/Platforms/SplashScreenWindows.h"

#ifdef SPLASH_PLATFORM_WINDOWS

#include "strsafe.h"
#pragma comment( lib, "windowscodecs.lib" )

constexpr wchar_t* c_ClassName = L"SplashScreenClassName";

namespace Splash
{
	SplashImageAreaWindows::SplashImageAreaWindows(int id, int xPosition, int yPosition, int width, int height, std::string filePath)
		: SplashImageArea(id, xPosition, yPosition, width, height, std::move(filePath))
	{ }

	unsigned int WINAPI ThreadLoop(LPVOID arg)
	{
		MSG message;
		bool closeSplashScreen = false;
		SplashScreenWindows* splashScreenWindows = static_cast<SplashScreenWindows*>(arg);
		while (!closeSplashScreen)
		{
			if (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&message);
				DispatchMessageW(&message);

				if (message.message == WM_QUIT)
				{
					closeSplashScreen = true;
				}
			}
		}
		return 0;
	}

	bool SplashScreenWindows::Init(int width, int height)
	{
		std::unique_lock lock(m_mutex);
		if (m_initialized)
		{
			return true;
		}

		m_width = width;
		m_height = height;

		HRESULT hr = CoInitialize(NULL);
		if (FAILED(hr))
		{
			Destroy();
			return m_initialized;
		}
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_imageFactory));
		if (FAILED(hr))
		{
			Destroy();
			return m_initialized;
		}
		lock.unlock();

		if (CreateWindowClass() == 0)
		{
			m_class = { };
			Destroy();
			return m_initialized;
		}

		if (CreateNativeWindowGuard() == 0)
		{
			Destroy();
			return m_initialized;
		}

		if (CreateNativeWindow() == 0)
		{
			Destroy();
			return m_initialized;
		}

		if (CreateFontObjects() == 0)
		{
			Destroy();
			return m_initialized;
		}

		lock.lock();
		int xPos = (GetSystemMetrics(SM_CXFULLSCREEN) / 2) - (m_width / 2);
		int yPos = (GetSystemMetrics(SM_CYFULLSCREEN) / 2) - (m_height / 2);
		SetWindowPos(m_window, 0, xPos, yPos, m_width, m_height, SWP_NOZORDER);
		ShowWindow(m_window, SW_HIDE);
		UpdateWindow(m_window);

		m_windowThread = CreateThread(NULL, 128 * 1024, (LPTHREAD_START_ROUTINE)ThreadLoop, (LPVOID)this, STACK_SIZE_PARAM_IS_A_RESERVATION, &m_windowThreadId);

		m_initialized = true;

		return m_initialized;
	}

	void SplashScreenWindows::Destroy()
	{
		std::lock_guard lock(m_mutex);
		if (m_window)
		{
			DestroyWindow(m_window);
		}

		if (m_windowThread)
		{
			WaitForSingleObject(m_windowThread, INFINITE);
			CloseHandle(m_windowThread);
		}

		m_windowThread = nullptr;
		m_window = nullptr;
		m_windowThreadId = 0;

		if (m_windowGuard)
		{
			PostMessageW(m_windowGuard, WM_DESTROY, 0, 0);
			DestroyWindow(m_windowGuard);
			m_windowGuard = NULL;
		}

		if (m_class.lpszClassName == c_ClassName)
		{
			UnregisterClassW(m_class.lpszClassName, m_class.hInstance);
			m_class = { };
		}


		if (m_imageFactory)
		{
			m_imageFactory->Release();
			m_imageFactory = nullptr;
		}
		CoUninitialize();

		if (m_backgroundImage.Bitmap)
		{
			DeleteObject(m_backgroundImage.Bitmap);
			m_backgroundImage.Bitmap = nullptr;
		}

		if (m_iconSmall)
		{
			DestroyIcon(m_iconSmall);
			m_iconSmall = nullptr;
		}
		if (m_iconBig)
		{
			DestroyIcon(m_iconBig);
			m_iconBig = nullptr;
		}

		for (size_t i = 0; i < m_imageAreas.size(); ++i)
		{
			DeleteObject(m_imageAreas.at(static_cast<int>(i)).Bitmap);
		}

		m_initialized = false;
	}

	void SplashScreenWindows::Update()
	{
		UpdateSplashWindow();
	}

	bool SplashScreenWindows::Show()
	{
		{
			std::lock_guard lock(m_mutex);
			if (!m_initialized)
			{
				return false;
			}
			ShowWindow(m_windowGuard, SW_SHOW);
			ShowWindow(m_window, SW_SHOW);
		}
		UpdateSplashWindow();

		return true;
	}

	bool SplashScreenWindows::Hide()
	{
		std::lock_guard lock(m_mutex);
		if (!m_initialized)
		{
			return false;
		}
		ShowWindow(m_windowGuard, SW_HIDE);
		ShowWindow(m_window, SW_HIDE);
		return true;
	}

	bool SplashScreenWindows::IsShowing() const
	{
		std::lock_guard lock(m_mutex);
		return m_windowThread != nullptr;
	}

	void SplashScreenWindows::SetIcons(const char* filePath)
	{
		if (m_iconSmall)
		{
			DestroyIcon(m_iconSmall);
			m_iconSmall = nullptr;
		}
		if (m_iconBig)
		{
			DestroyIcon(m_iconBig);
			m_iconBig = nullptr;
		}

		m_iconSmall = (HICON)LoadImageA(NULL, filePath, IMAGE_ICON, 16, 16, LR_LOADFROMFILE);
		m_iconBig = (HICON)LoadImageA(NULL, filePath, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);

		PostThreadMessageA(m_windowThreadId, WM_SETICON, ICON_SMALL, (LPARAM)m_iconSmall);
		PostThreadMessageA(m_windowThreadId, WM_SETICON, ICON_BIG, (LPARAM)m_iconBig);
		UpdateSplashWindow();
	}

	void SplashScreenWindows::SetBackgroundImage(const char* filePath)
	{
		{
			std::unique_lock lock(m_mutex);
			if (!m_initialized)
			{
				return;
			}
			std::string	strFilePath = filePath;
			m_backgroundImage.FilePath = strFilePath;
			lock.unlock();

			HBITMAP bitmap = LoadImageFromFile(strFilePath);

			lock.lock();
			m_backgroundImage.Bitmap = bitmap;
			lock.unlock();

			SetIcons(filePath);
		}

		UpdateSplashWindow();
	}

	void SplashScreenWindows::SetText(int id, int xPosition, int yPosition, const char* content)
	{
		SetText(id, xPosition, yPosition, content, Splash::SplashTextConfig());
	}

	void SplashScreenWindows::SetText(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config)
	{
		{
			std::lock_guard lock(m_mutex);
			if (!m_initialized)
			{
				return;
			}

			// Out of bounds check for the rect. Make sure the rect fits. At least for x, y.
			xPosition = max(0, min(m_width, xPosition));
			yPosition = max(0, min(m_height, yPosition));
			m_textAreas[id] = SplashTextArea(id, xPosition, yPosition, std::move(content), std::move(config));

			SIZE textSize = { };
			HDC hdcScreen = GetDC(NULL);
			GetTextExtentPoint32A(hdcScreen, m_textAreas[id].Content.c_str(), static_cast<int>(m_textAreas[id].Content.size()), &textSize);
			const int textWidth = (int)textSize.cx;
			const int textHeight = (int)textSize.cy;

			RECT newRect = RECT
			{
				xPosition,
				yPosition,
				xPosition + textWidth,
				yPosition + textHeight
			};
			m_textRectAreas[id] = newRect;
		}
		UpdateSplashWindow();
	}

	void SplashScreenWindows::SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content)
	{ 
		SetTextAutoPlacement(id, xPosition, yPosition, content, Splash::SplashTextConfig()); 
	}

	void SplashScreenWindows::SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config)
	{
		{
			std::lock_guard lock(m_mutex);
			if (!m_initialized)
			{
				return;
			}
		}
		SetText(id, xPosition, yPosition, content, std::move(config));
		
		{
			std::lock_guard lock(m_mutex);
			SplashTextArea& splashTextArea = m_textAreas[id];
			RECT& rect = m_textRectAreas[id];

			const int widthDelta = rect.right - m_width;
			const int heightDelta = rect.bottom - m_height;

			if (widthDelta > 0)
			{
				// We are out of bounds on the x axis.
				rect.left = max(0, rect.left - widthDelta);
				rect.right = max(0, rect.right - widthDelta);
			}

			if (heightDelta > 0)
			{
				// We are out of bounds on the x axis.
				rect.bottom = max(0, rect.bottom - heightDelta);
				rect.top = max(0, rect.top - heightDelta);
			}
		}
		UpdateSplashWindow();
	}

	void SplashScreenWindows::UpdateSplashWindow()
	{
		RedrawWindow(m_window, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	int SplashScreenWindows::CreateWindowClass()
	{
		std::lock_guard lock(m_mutex);
		m_class.cbSize = sizeof(WNDCLASSEXW);
		m_class.style = CS_HREDRAW | CS_VREDRAW;
		m_class.lpfnWndProc = SplashScreenMsgProc;
		m_class.cbClsExtra = 0;
		m_class.cbWndExtra = 0;
		m_class.hInstance = GetModuleHandle(NULL);
		m_class.hIcon = 0;
		m_class.hIconSm = 0;
		m_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		m_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		m_class.lpszMenuName = NULL;
		m_class.lpszClassName = c_ClassName;
		return RegisterClassExW(&m_class);
	}

	int SplashScreenWindows::CreateNativeWindowGuard()
	{
		std::lock_guard lock(m_mutex);
		m_windowGuard = CreateWindowA(
			"STATIC",
			"SplashScreenWindow",
			0,
			0,
			0,
			0,
			0,
			HWND_MESSAGE,
			(HMENU)0,
			GetModuleHandle(NULL),
			(LPVOID)0);
		if (m_windowGuard)
		{
			ShowWindow(m_windowGuard, SW_SHOW);
			return 1;
		}
		return 0;
	}

	int SplashScreenWindows::CreateNativeWindow()
	{
		std::lock_guard lock(m_mutex);
		const DWORD exStyle = WS_EX_TOOLWINDOW;
		const DWORD style = WS_POPUP | WS_BORDER | WS_DISABLED;
		m_window = CreateWindowExW(
			exStyle,
			m_class.lpszClassName,
			L"SplashScreen",
			style,
			0, 0, m_width, m_height,
			NULL,
			NULL,
			m_class.hInstance,
			this);
		return m_window != nullptr;
	}

	int SplashScreenWindows::CreateFontObjects()
	{
		std::lock_guard lock(m_mutex);
		// Create font for the text on the window.
		HFONT systemFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		LOGFONTW fontSpecs;
		GetObjectW(systemFont, sizeof(LOGFONTW), &fontSpecs);
		fontSpecs.lfHeight = 16;
		fontSpecs.lfQuality = CLEARTYPE_QUALITY;
		m_defaultFont = CreateFontIndirectW(&fontSpecs);
		if (m_defaultFont == nullptr)
		{
			m_defaultFont = systemFont;
			m_usingSystemFont = true;
		}
		return m_defaultFont != nullptr;
	}

	HBITMAP SplashScreenWindows::LoadImageFromFile(const std::string& filePath)
	{
		IWICBitmapDecoder* decoder = nullptr;
		std::wstring wFilePath = WStringFromAString(filePath);
		HRESULT hr = m_imageFactory->CreateDecoderFromFilename(wFilePath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
		if (FAILED(hr))
		{
			if (decoder)
				decoder->Release();
			return 0;
		}

		IWICBitmapFrameDecode* frame;
		if (SUCCEEDED(hr))
		{
			hr = decoder->GetFrame(0, &frame);
		}

		IWICBitmapSource* originalBitmapSource = nullptr;
		if (SUCCEEDED(hr))
		{
			hr = frame->QueryInterface(IID_IWICBitmapSource, reinterpret_cast<void**>(&originalBitmapSource));
		}

		IWICFormatConverter* converter = nullptr;
		if (SUCCEEDED(hr))
		{
			hr = m_imageFactory->CreateFormatConverter(&converter);
		}

		if (SUCCEEDED(hr))
		{
			hr = converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeCustom);
		}

		IWICBitmapSource* bitmapSource = nullptr;
		if (SUCCEEDED(hr))
		{
			hr = converter->QueryInterface(IID_PPV_ARGS(&bitmapSource));
		}

		HBITMAP resultBitmap = 0;
		if (SUCCEEDED(hr))
		{
			resultBitmap = CreawteBitmapFromIWICBitmap(bitmapSource);
		}

		if (originalBitmapSource)
			originalBitmapSource->Release();
		if (bitmapSource)
			bitmapSource->Release();
		if (converter)
			converter->Release();
		if (frame)
			frame->Release();
		if (decoder)
			decoder->Release();

		return resultBitmap;
	}

	HBITMAP SplashScreenWindows::CreawteBitmapFromIWICBitmap(IWICBitmapSource*& source)
	{
		HBITMAP hBitmap = 0;
		UINT width = 0;
		UINT height = 0;;
		void* imageData = nullptr;

		WICPixelFormatGUID pixelFormat;
		HRESULT hr = source->GetPixelFormat(&pixelFormat);

		if (SUCCEEDED(hr))
		{
			//hr = (pixelFormat == GUID_WICPixelFormat32bppRGB) ? S_OK : E_FAIL;
		}

		if (SUCCEEDED(hr))
		{
			hr = source->GetSize(&width, &height);
		}

		IWICBitmapScaler* scaler = nullptr;
		if (SUCCEEDED(hr) && (width != (UINT)m_width || height != (UINT)m_height))
		{
			hr = m_imageFactory->CreateBitmapScaler(&scaler);
			if (SUCCEEDED(hr))
			{
				hr = scaler->Initialize(source, m_width, m_height, WICBitmapInterpolationModeFant);
				source->Release();

				hr = scaler->QueryInterface(IID_IWICBitmapSource, reinterpret_cast<void**>(&source));
				hr = source->GetSize(&width, &height);
				scaler->Release();
			}
		}

		if (SUCCEEDED(hr))
		{
			BITMAPINFO info = { };
			ZeroMemory(&info, sizeof(info));
			info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			info.bmiHeader.biWidth = width;
			info.bmiHeader.biHeight = -(LONG)height;
			info.bmiHeader.biPlanes = 1;
			info.bmiHeader.biBitCount = 32;
			info.bmiHeader.biCompression = BI_RGB;

			HDC hdcScreen = GetDC(NULL);
			hr = hdcScreen ? S_OK : E_FAIL;

			if (SUCCEEDED(hr))
			{
				if (hBitmap)
				{
					DeleteObject(hBitmap);
				}

				hBitmap = CreateDIBSection(hdcScreen, &info, DIB_RGB_COLORS, &imageData, NULL, 0);
				ReleaseDC(NULL, hdcScreen);
				hr = hBitmap ? S_OK : E_FAIL;
			}

			UINT cbStride = 0;
			if (SUCCEEDED(hr))
			{
				// Size of a scan line represented in bytes: 4 bytes each pixel
				hr = UIntMult(width, sizeof(DWORD), &cbStride);
			}

			UINT cbBufferSize = 0;
			if (SUCCEEDED(hr))
			{
				// Size of the image, represented in bytes
				hr = UIntMult(cbStride, height, &cbBufferSize);
			}

			if (SUCCEEDED(hr))
			{
				// Extract the image into the GDI+ Bitmap
				hr = source->CopyPixels(
					NULL,
					cbStride,
					cbBufferSize,
					reinterpret_cast<BYTE*>(imageData));
			}

			if (FAILED(hr) && hBitmap)
			{
				DeleteObject(hBitmap);
				hBitmap = 0;
			}
		}
		return hBitmap;
	}

	void SplashScreenWindows::DisplayLastWindowsError(LPTSTR lpszFunction)
	{
		// Retrieve the system error message for the last-error code

		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		DWORD dw = GetLastError();

		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0, NULL);

		// Display the error message and exit the process

		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
			(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
		StringCchPrintf((LPTSTR)lpDisplayBuf,
			LocalSize(lpDisplayBuf) / sizeof(TCHAR),
			TEXT("%s failed with error %d: %s"),
			lpszFunction, dw, lpMsgBuf);
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
	}

	LRESULT CALLBACK SplashScreenWindows::SplashScreenMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		HDC hdc;
		PAINTSTRUCT ps;
		SplashScreenWindows* splashScreenWindows = nullptr;

		if (message == WM_CREATE)
		{
			splashScreenWindows = static_cast<SplashScreenWindows*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);

			SetLastError(0);
			if (!SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(splashScreenWindows)))
			{
				if (GetLastError() != 0)
					return FALSE;
			}
		}
		else
		{
			splashScreenWindows = reinterpret_cast<SplashScreenWindows*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		switch (message)
		{
		case WM_PAINT:
		{
			hdc = BeginPaint(hWnd, &ps);

			//std::unique_lock lock(splashScreenWindows->m_mutex, std::defer_lock);
			//if (lock.try_lock())
			{
				if (splashScreenWindows->m_backgroundImage.Bitmap != 0)
				{
					DrawStateW(hdc, DSS_NORMAL, NULL, (LPARAM)splashScreenWindows->m_backgroundImage.Bitmap, 0, 0, 0, 0, 0, DST_BITMAP);
				}

				for (size_t i = 0; i < (int)splashScreenWindows->m_imageAreas.size(); ++i)
				{
					DrawStateW(
						hdc, 
						DSS_NORMAL, 
						NULL, 
						(LPARAM)splashScreenWindows->m_imageAreas.at(static_cast<int>(i)).Bitmap,
						0,
						splashScreenWindows->m_imageAreas.at(static_cast<int>(i)).XPosition, 
						splashScreenWindows->m_imageAreas.at(static_cast<int>(i)).YPosition, 
						splashScreenWindows->m_imageAreas.at(static_cast<int>(i)).Width, 
						splashScreenWindows->m_imageAreas.at(static_cast<int>(i)).Height, 
						DST_BITMAP);
				}

				RECT rc;
				GetClientRect(hWnd, &rc);

				SelectObject(hdc, splashScreenWindows->m_defaultFont);

				for (int i = 0; i < (int)splashScreenWindows->m_textAreas.size(); ++i)
				{
					Splash::SplashTextConfig const& textConfig = splashScreenWindows->m_textAreas[i].Config;

					SetBkMode(hdc, textConfig.BackgroundTransparent ? TRANSPARENT : OPAQUE);
					SetBkColor(hdc, RGB(textConfig.BackgroundColour.R, textConfig.BackgroundColour.G, textConfig.BackgroundColour.B));

					SetTextColor(hdc, RGB(textConfig.FontColour.R, textConfig.FontColour.G, textConfig.FontColour.B));
					DrawTextA(hdc, splashScreenWindows->m_textAreas[i].Content.c_str(), (int)splashScreenWindows->m_textAreas[i].Content.size(), &splashScreenWindows->m_textRectAreas[i], DT_NOCLIP | DT_SINGLELINE);
				}
				//lock.unlock();

			}
			EndPaint(hWnd, &ps);
			break;
		}
		case WM_DESTROY:
		{
			PostThreadMessageA(splashScreenWindows->m_windowThreadId, WM_QUIT, NULL, NULL);
			break;
		}
		};

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
#endif