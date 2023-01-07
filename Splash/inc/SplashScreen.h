#pragma once

#ifdef SPLASH_PLATFORM_WINDOWS
#include "Splash/Platforms/SplashScreenWindows.h"
	using SplashScreen = Splash::SplashScreenWindows;
#elif SPLASH_PLATFORM_UNIX
	using SplashScreen = SplashScreenUnix;
#else 
#ifndef SPLASH_PLATFORM_NO
#pragma message ( "WARNING: [Splash] No platform define has been defined." ) 
#endif
#include "Splash/Platforms/SplashScreenEmpty.h"
	using SplashScreen = Splash::SplashScreenEmpty;
#endif