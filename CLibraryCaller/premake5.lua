workspace "CWrapping"
    
    --premake.showhelp()
    configurations { "Debug", "Release" }
    platforms { "x64" }

    filter "configurations:Release"
        symbols "Off"
        optimize "Speed"
        flags { "StaticRuntime" }

    filter "configurations:Debug"
        symbols "On"
        -- optimize "Debug"

    characterset "Unicode"
    flags { "MultiProcessorCompile" }

    project("CWrapper")
        kind "StaticLib"
        language "C++"
        libdirs { "%Mono64%/lib" }
        includedirs { "%Mono64%/include/mono-2.0" }
        links { "mono-2.0-sgen" }
        files { "*.h", "*.c" }
        defines { "MONO_EMBEDDINATOR_STATIC_LIB" }
        -- postbuildcommands { "xcopy \"./bin/x64/Release\"  \".\" /Y" }
        -- flags { "StaticRuntime" }