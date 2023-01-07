#pragma once

#ifdef SPLASH_PLATFORM_WINDOWS
#include "Splash/Platforms/SplashScreenWindows.h"
	using SplashScreen = Splash::SplashScreenWindows;
#elif SPLASH_PLATFORM_UNIX
	using SplashScreen = SplashScreenUnix;
#endif