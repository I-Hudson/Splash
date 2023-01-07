project "SplashScreenEXE"  
    language "C++"
    cppdialect "C++17"
    configurations { "Debug", "Debug_DLL", "Release", "Release_DLL" } 
    location "./"
    kind "ConsoleApp"
    
    targetname ("%{prj.name}")
    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    debugdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")

    dependson
    {
        "SplashScreen"
    }

    includedirs
    {
        "inc",
        "../Splash/inc"
    }

    files 
    { 
        "inc/**.hpp", 
        "inc/**.h", 
        "src/**.cpp",
    }

    filter "configurations:Debug"
        defines { "DEBUG" }  
        symbols "On"
        buildoptions "/MTd"
        libdirs
        {
            "%{wks.location}deps/".. outputdir..  "/lib/"
        }
        links
        {
            "SplashScreend.lib"
        }

    filter "configurations:Debug_DLL"
        defines { "DEBUG", "SPLASH_DLL" }  
        symbols "On"
        buildoptions "/MDd"
        libdirs
        {
            "%{wks.location}deps/".. outputdir..  "/lib/"
        }
        links
        {
            "SplashScreend.lib"
        }

    filter "configurations:Release"  
        defines { "NDEBUG" }    
        optimize "On"
        buildoptions "/MT"
        libdirs
        {
            "%{wks.location}deps/".. outputdir..  "/lib/"
        }
        links
        {
            "SplashScreen.lib"
        }

    filter "configurations:Release_DLL"  
        defines { "NDEBUG", "SPLASH_DLL" }    
        optimize "On" 
        buildoptions "/MD"
        libdirs
        {
            "%{wks.location}deps/".. outputdir..  "/lib/"
        }
        links
        {
            "SplashScreen.lib"
        }

    filter "system:Windows"
        system "windows"
        toolset("msc-v142")
        defines
        {
            "SPLASH_PLATFORM_WINDOWS"
        }