#include "Splash/ISplashScreen.h"

namespace Splash
{
	SplashTextArea::SplashTextArea(int id, int xPosition, int yPosition, std::string content)
		: Id(id)
		, XPosition(xPosition)
		, YPosition(yPosition)
		, Content(std::move(content))
	{ }

	SplashTextArea::SplashTextArea(int id, int xPosition, int yPosition, std::string content, SplashTextConfig config)
		: Id(id)
		, XPosition(xPosition)
		, YPosition(yPosition)
		, Content(std::move(content))
		, Config(std::move(config))
	{ }

	SplashImageArea::SplashImageArea(int id, int xPosition, int yPosition, int width, int height, std::string filePath)
		: Id(id)
		, XPosition(xPosition)
		, YPosition(yPosition)
		, Width(width)
		, Height(height)
		, FilePath(std::move(filePath))
	{ }

	std::wstring ISplashScreen::WStringFromAString(const std::string& str)
	{
		std::wstring ws(str.size(), L' '); // Overestimate number of code points.
		ws.resize(std::mbstowcs(&ws[0], str.c_str(), str.size())); // Shrink to fit.
		return ws;
	}
}