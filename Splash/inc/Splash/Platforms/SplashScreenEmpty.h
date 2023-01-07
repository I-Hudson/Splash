#pragma once

#include "Splash/ISplashScreen.h"

#ifdef SPLASH_DLL
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif // SPLASH_DLL

namespace Splash
{
	/// @brief Add a default implementation of 'ISplashScreen'. If the user hasn't 
	// defines a platform define like 'SPLASH_PLATFORM_WINDOWS' then this is used to 
	// help reduce/stop compile issues.
	class SPLASH_API SplashScreenEmpty : public Splash::ISplashScreen
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
	};
}

#ifdef SPLASH_DLL
#pragma warning( pop )
#endif // SPLASH_DLL