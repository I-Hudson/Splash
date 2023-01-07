#include "Splash/Platforms/SplashScreenEmpty.h"

namespace Splash
{
    bool SplashScreenEmpty::Init(int width, int height)
    {
        return true;
    }

    void SplashScreenEmpty::Destroy()
    {
    }

    void SplashScreenEmpty::Update()
    {
    }

    bool SplashScreenEmpty::Show()
    {
        return true;
    }

    bool SplashScreenEmpty::Hide()
    {
        return true;
    }

    bool SplashScreenEmpty::IsShowing() const
    {
        return false;
    }

    void SplashScreenEmpty::SetIcons(const char* filePath)
    { }

    void SplashScreenEmpty::SetBackgroundImage(const char* filePath)
    { }

    void SplashScreenEmpty::SetText(int id, int xPosition, int yPosition, const char* content)
    { }

    void SplashScreenEmpty::SetText(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config)
    { }

    void SplashScreenEmpty::SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content)
    { }

    void SplashScreenEmpty::SetTextAutoPlacement(int id, int xPosition, int yPosition, const char* content, Splash::SplashTextConfig config)
    { }
}
