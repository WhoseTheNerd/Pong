workspace "Pong"
    configurations
    {
        "Debug",
        "Release"
    }

project "Pong"
    kind "ConsoleApp"
    language "C"
    cdialect "gnu99"
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
        "SDL2",
        "SDL2_ttf"
    }

    buildoptions { "-nostdlib -fno-stack-protector" }
    linkoptions { "-nostdlib -fno-stack-protector" }

    filter "configurations:Debug"      
        defines { "DEBUG" }      
        symbols "On"
    filter "configurations:Release"      
        defines { "NDEBUG" }      
        optimize "On"