#pragma once

namespace Splash
{
	/// @brief Define a single Colour.
	struct Colour
	{

		unsigned char R;
		unsigned char G;
		unsigned char B;
	};
	constexpr static Colour Colour_While = Colour{ 255, 255, 255 };
	constexpr static Colour Colour_Black = Colour{ 0,   0,   0 };
	constexpr static Colour Colour_Red   = Colour{ 255, 0,   0 };
	constexpr static Colour Colour_Green = Colour{ 0,   255, 0 };
	constexpr static Colour Colour_Blue  = Colour{ 0,   0,   0 };
}