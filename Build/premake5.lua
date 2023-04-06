outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
outputdir_target = "%{wks.location}bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
outputdir_obj = "%{wks.location}bin-int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

workspace "Splash"
    architecture "x64"
    startproject "SplashScreen"
    staticruntime "off"
    location "../"
    
    language ("C++")
    cppdialect ("C++20")

    configurations
    {
        "Debug",
        "Debug_DLL",
        "Release",
        "Release_DLL",
    }
    platforms 
    { 
        "Win64", 
        "Linux" 
    }

    flags
    {
    	"MultiProcessorCompile"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
    }


    filter "configurations:Debug"
        defines { "DEBUG" }

    filter "configurations:Debug_DLL"
        defines { "DEBUG" }

    filter "configurations:Release"  
        defines { "NDEBUG" }

    filter "configurations:Release_DLL"  
        defines { "NDEBUG" }


    filter "system:Windows"
    	system "windows"
    	toolset("msc-v143")
        defines
        {
            "SPLASH_PLATFORM_WINDOWS"
        }

    filter "system:Unix"
    	system "linux"
    	toolset("clang")
        defines
        {
            "SPLASH_PLATFORM_UNIX"
        }

group ""
    include "../Splash/Splash.lua"
    include "../SplashEXE/SplashEXE.lua"

newaction{
    trigger = "clean",
    description = "Remove all binaries and intermediate binaries, and vs files.",
    execute = function()
        print("Removeing binaries")
        os.rmdir("../bin")
        print("Removeing internediate binaries")
        os.rmdir("../bin-int")
        print("Removing dependencies")
        os.rmdir("../deps")
        print("Removeing project files")
        os.rmdir("../.vs")
        os.remove("../Splash/**.sln")
        os.remove("../Splash/**.vcxproj")
        os.remove("../Splash/**.vcxproj.filters")
        os.remove("../Splash/**.vcxproj.user")
        print("Done")
    end
} 