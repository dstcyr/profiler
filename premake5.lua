workspace "Profiler"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release"
    }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    project "Profiler"
        location "Profiler"
        kind "StaticLib"
        language "C++"
        characterset ("MBCS")

        targetdir("build/" ..outputdir .. "/%{prj.name}")
        objdir("build-int/" ..outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/includes/**.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "%{prj.name}/includes"
        }

        links
        {
			Sandbox
        }

        filter "system:windows"
        cppdialect "C++17"
        staticruntime "on"
        systemversion "latest"

        defines
        {
        }

        filter "configurations:Debug"
        defines "_DEBUG"
        symbols "on"

        filter "configurations:Release"
        defines "_RELEASE"
        optimize "on"

    project "Sandbox"
        location "Sandbox"
        kind "ConsoleApp"
        language "C++"
        characterset ("MBCS")

        targetdir("build/" ..outputdir .. "/%{prj.name}")
        objdir("build-int/" ..outputdir .. "/%{prj.name}")

        files
        {
            "%{prj.name}/includes/**.h",
            "%{prj.name}/src/**.cpp"
        }

        includedirs
        {
            "Profiler/includes",
			"%{prj.name}/includes"
        }

        links
        {
            "Profiler"
        }

        filter "system:windows"
        cppdialect "C++17"
        staticruntime "on"
        systemversion "latest"

        defines
        {
        }

        filter "configurations:Debug"
        defines "_DEBUG"
        symbols "on"

        filter "configurations:Release"
        defines "_RELEASE"
        optimize "on"
