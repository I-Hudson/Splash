project "SplashScreen"  
    configurations { "Debug", "Debug_DLL", "Release", "Release_DLL" } 
    location "./"
    
    language ("C++")
    cppdialect ("C++20")

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")
    debugdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    
    includedirs
    {
        "inc",
    }

    files 
    { 
        "inc/**.hpp", 
        "inc/**.h", 
        "src/**.cpp",
    }

    filter "configurations:Debug"
        targetname ("%{prj.name}d")
        defines { "DEBUG" }  
        symbols "On"
        buildoptions "/MTd"
        kind "StaticLib"
        postbuildcommands
        {
            "{COPY} \"%{cfg.targetdir}/%{prj.name}d.lib\" \"%{wks.location}deps/".. outputdir..  "/lib/\"\n",
        }

    filter "configurations:Debug_DLL"
        targetname ("%{prj.name}d")
        defines { "DEBUG", "SPLASH_DLL", "SPLASH_EXPORT_DLL" }  
        symbols "On"
        buildoptions "/MDd"
        kind "SharedLib"
        postbuildcommands
        {
            "{COPY} \"%{cfg.targetdir}/%{prj.name}d.lib\" \"%{wks.location}deps/".. outputdir..  "/lib/\"\n",
            "{COPY} \"%{cfg.targetdir}/%{prj.name}d.dll\" \"%{wks.location}deps/".. outputdir..  "/dll/\"\n",
        }
        
    filter "configurations:Release"  
        targetname ("%{prj.name}")
        defines { "NDEBUG" }    
        optimize "On"
        buildoptions "/MT"
        kind "StaticLib"
        postbuildcommands
        {
            "{COPY} \"%{cfg.targetdir}/%{prj.name}.lib\" \"%{wks.location}deps/".. outputdir..  "/lib/\"\n",
        }

    filter "configurations:Release_DLL" 
        targetname ("%{prj.name}")
        defines { "NDEBUG", "SPLASH_DLL", "SPLASH_EXPORT_DLL" }    
        optimize "On" 
        buildoptions "/MD"
        kind "SharedLib"
        postbuildcommands
        {
            "{COPY} \"%{cfg.targetdir}/%{prj.name}.lib\" \"%{wks.location}deps/".. outputdir..  "/lib/\"\n",
            "{COPY} \"%{cfg.targetdir}/%{prj.name}.dll\" \"%{wks.location}deps/".. outputdir..  "/dll/\"\n",
        }

    filter "system:Windows"
        system "windows"
        toolset("msc-v142")
        defines
        {
            "SPLASH_PLATFORM_WINDOWS"
        }