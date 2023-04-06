#pragma once

#include "Splash/ISplashScreen.h"

#ifdef SPLASH_PLATFORM_WINDOWS

#ifdef SPLASH_DLL
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wincodec.h>
#include <unordered_map>

namespace Splash
{
	struct SplashImageAreaWindows : public SplashImageArea
	{
		SplashImageAreaWindows() = default;
		SplashImageAreaWindows(int id, int xPosition, int yPosition, int width, int height, std::string filePath);

		HBITMAP Bitmap = 0;
	};

	class SPLASH_API SplashScreenWindows : public Splash::ISplashScreen
	{
	public:
		virtual bool Init(int width, int height) override;
		virtual void Destroy() override;

		virtual void Update() override;
		virtual bool Show() override;
		virtual bool Hide() override;

		virtual bool IsShowing() const override;

		virtual void SetIcons(const char* filePath) override;

		virtual void SetBackgroundImage(const char* filePath) override;

		virtual void SetText(int id, int xPosition, int yPosition, const char* content) override;
		virtual void SetText(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config) override;

		virtual void SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content) override;
		virtual void SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config) override;

	private:
		int CreateWindowClass();
		int CreateNativeWindowGuard();
		int CreateNativeWindow();
		int CreateFontObjects();

		void UpdateSplashWindow();

		HBITMAP LoadImageFromFile(const std::string& filePath);
		HBITMAP CreawteBitmapFromIWICBitmap(IWICBitmapSource*& source);

		void DisplayLastWindowsError(LPTSTR lpszFunction);

		static LRESULT CALLBACK SplashScreenMsgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		std::unordered_map<int, RECT> m_textRectAreas;
		std::unordered_map<int, SplashImageAreaWindows> m_imageAreas;
		SplashImageAreaWindows m_backgroundImage;

		HANDLE m_windowThread = nullptr;
		DWORD m_windowThreadId = 0;
		HWND m_windowGuard = nullptr;
		HWND m_window = nullptr;

		HICON m_iconBig = nullptr;
		HICON m_iconSmall = nullptr;

		WNDCLASSEX m_class = { };
		HFONT m_defaultFont = nullptr;
		bool m_usingSystemFont = false;

		IWICImagingFactory* m_imageFactory = nullptr;
	};
}

#ifdef SPLASH_DLL
#pragma warning( pop )
#endif

#endif