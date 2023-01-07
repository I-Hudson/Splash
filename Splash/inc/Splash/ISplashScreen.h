#pragma once

#include "Splash/SplashText.h"

#include <string>
#include <unordered_map>
#include <mutex>

#include <string>

#ifdef SPLASH_DLL
	#ifdef SPLASH_EXPORT_DLL
		#define SPLASH_API __declspec( dllexport )
	#else
		#define SPLASH_API __declspec( dllimport )
	#endif
	#pragma warning( push )
	#pragma warning( disable : 4251 )
#else
	#define SPLASH_API
#endif

namespace Splash
{
	struct SplashRect
	{
		int Left = 0;
		int Top = 0;
		int Right = 0;
		int Bottom = 0;
	};

	struct SplashImageArea
	{
		SplashImageArea() = default;
		SplashImageArea(int id, int xPosition, int yPosition, int width, int height, std::string filePath);

		int Id = -1;
		int XPosition = 0;
		int YPosition = 0;
		int Width = 0;
		int Height = 0;
		std::string FilePath;
	};

	/// <summary>
	/// Pure virtual class to help with development.
	/// </summary>
	class SPLASH_API ISplashScreen
	{
	public:
		/// <summary>
		/// Init underlying resource needed for the splash screen.
		/// </summary>
		virtual bool Init(int width, int height) = 0;
		/// <summary>
		/// Destroy all resource required for the splash screen.
		/// </summary>
		virtual void Destroy() = 0;

		virtual void Update() = 0;
		virtual bool Show() = 0;
		virtual bool Hide() = 0;

		/// <summary>
		/// Is the splash screen currenlty being shown.
		/// </summary>
		/// <returns></returns>
		virtual bool IsShowing() const = 0;
		bool Isinitialized() const { return m_initialized; }

		virtual void SetIcons(const char* filePath) = 0;

		virtual void SetBackgroundImage(const char* filePath) = 0;

		virtual void SetText(int id, int xPosition, int yPosition, const char* content) = 0;
		virtual void SetText(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config) = 0;

		virtual void SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content) = 0;
		virtual void SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config) = 0;

		int GetWidth() const { return m_width; }
		int GetHeight() const { return m_height; }

	protected:
		std::wstring WStringFromAString(const std::string& str);

	protected:
		std::unordered_map<int, SplashTextArea> m_textAreas;
		mutable std::mutex m_mutex;

		unsigned int m_width = 0;
		unsigned int m_height = 0;
		bool m_initialized = false;
	};
}
#ifdef SPLASH_DLL
#pragma warning( pop )
#endif