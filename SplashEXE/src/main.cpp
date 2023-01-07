#include "SplashScreen.h"

void main(int argc, char** argv)
{
	SplashScreen ss;
	ss.Init(840, 620);

	Splash::SplashTextConfig textConfig;
	textConfig.FontColour = Splash::Colour_While;

	ss.SetBackgroundImage("./SPLASH_SCREEN_PLACE_HOLDER.png");
	ss.SetIcons("./Christmas_Cute_Roadhog.ico");
	ss.SetText(0, 0, 0, "This is some test string for 'TOP LEFT'.", textConfig);
	ss.SetTextAutoPlacement(1, ss.GetWidth(), 0, "This is some test string for 'TOP Right'.");
	ss.SetTextAutoPlacement(2, 0, ss.GetHeight(), "This is some test string for 'BOTTOM LEFT'.", textConfig);

	textConfig.BackgroundTransparent = false;
	ss.SetTextAutoPlacement(3, ss.GetWidth(), ss.GetHeight(), "This is some test string for 'BOTTOM RIGHT'.", textConfig);

	ss.Show();

	while (1)
	{
		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
		{
			break;
		}
	}

	ss.Destroy();
}