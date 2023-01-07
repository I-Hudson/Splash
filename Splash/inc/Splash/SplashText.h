#pragma once

#include "Splash/SplashColour.h"

#include <string>

namespace Splash
{
	/// @brief Describe how text should be rendered.
	struct SplashTextConfig
	{
		unsigned int FontSize = 14;
		Colour FontColour = Colour_Black;
		Colour BackgroundColour = Colour_Black;
		bool BackgroundTransparent = true;
	};

	/// @brief Describe an area on the splash screen to be filled with text.
	struct SplashTextArea
	{
		SplashTextArea() = default;
		SplashTextArea(int id, int xPosition, int yPosition, std::string content);
		SplashTextArea(int id, int xPosition, int yPosition, std::string content, SplashTextConfig config);

		int Id = -1;
		int XPosition = 0;
		int YPosition = 0;
		std::string Content;
		SplashTextConfig Config;
	};
}