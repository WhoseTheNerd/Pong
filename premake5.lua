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

    targetdir "bin/%{cfg.buildcfg}"
    files 
    { 
        "src/**.h", 
        "src/**.c" 
    }
    filter "configurations:Debug"      
        defines { "DEBUG" }      
        symbols "On"
    filter "configurations:Release"      
        defines { "NDEBUG" }      
        optimize "On"