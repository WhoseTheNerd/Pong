workspace "Pong"
    configurations
    {
        "Debug",
        "Release"
    }

project "SDL"
    location "lib/SDL"

    kind "StaticLib"
    language "C"
    cdialect "c99"

    targetdir "bin/%{cfg.buildcfg}"

    includedirs
    {
        "include/"
    }

    files 
    {  
        "src/**.c" 
    }

    filter "configurations:Debug"      
        defines { "DEBUG" }      
        symbols "On"
    filter "configurations:Release"      
        defines { "NDEBUG" }      
        optimize "On"

project "Pong"
    kind "ConsoleApp"
    language "C"
    cdialect "c99"

    targetdir "bin/%{cfg.buildcfg}"

    includedirs
    {
        "src/",
        "include/"
    }

    files 
    { 
        "src/**.h", 
        "src/**.c" 
    }

    links
    {
        "SDL"
    }

    filter "configurations:Debug"      
        defines { "DEBUG" }      
        symbols "On"
    filter "configurations:Release"      
        defines { "NDEBUG" }      
        optimize "On"