workspace "Pong"
    configurations
    {
        "Debug",
        "Release"
    }

project "Pong"
    kind "ConsoleApp"
    language "C"
    cdialect "c99"
    staticruntime "on"

    targetdir "bin/%{cfg.buildcfg}"

    includedirs
    {
        "src/"
    }

    files 
    { 
        "src/**.h", 
        "src/**.c" 
    }

    links
    {
        "SDL2"
    }

    filter "configurations:Debug"      
        defines { "DEBUG" }      
        symbols "On"
    filter "configurations:Release"      
        defines { "NDEBUG" }      
        optimize "On"