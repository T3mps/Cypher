workspace "Cypher"
    architecture "x64"
    startproject "CypherTest"
    configurations { "Debug", "Release" }
    flags { "MultiProcessorCompile" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    IncludeDir = {}
    IncludeDir["Cypher"] = "Cypher/src"
    IncludeDir["jemalloc"] = "Vendor/jemalloc/include"
    IncludeDir["spdlog"] = "Vendor/spdlog/include"
    IncludeDir["glm"] = "Vendor/glm/include"
    IncludeDir["entt"] = "Vendor/entt/single_include"
    IncludeDir["box2d"] = "Vendor/box2d/include"

    project "Cypher"
        location "Cypher"
        kind "StaticLib"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files {
            "%{IncludeDir.Cypher}/**.h",
            "%{IncludeDir.Cypher}/**.cpp",
            "%{IncludeDir.Cypher}/**.hpp"
        }

        includedirs {
            "%{IncludeDir.Cypher}",
            "%{IncludeDir.jemalloc}",
            "%{IncludeDir.spdlog}",
            "%{IncludeDir.glm}",
            "%{IncludeDir.entt}",
            "%{IncludeDir.box2d}"
        }

        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "CYPHER_EXPORTS"
        }

        postbuildcommands {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Loom")
        }

        filter "configurations:Debug"
            defines { "CYPHER_DEBUG" }
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines { "CYPHER_RELEASE" }
            runtime "Release"
            optimize "on"

    project "CypherTest"
        location "CypherTest"
        kind "SharedLib"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files {
            "%{prj.location}/src/**.h",
            "%{prj.location}/src/**.cpp",
            "%{prj.location}/src/**.hpp"
        }  

        includedirs {
            "%{IncludeDir.Cypher}",
            "%{IncludeDir.jemalloc}",
            "%{IncludeDir.spdlog}",
            "%{IncludeDir.glm}",
            "%{IncludeDir.entt}",
            "%{IncludeDir.box2d}"
        }

        links {
            "Cypher"
        }

        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "CYPHERTEST_EXPORTS"
        }

        postbuildcommands {
            ("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Loom")
        }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"

    project "Loom"
        location "Loom"
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++20"
        staticruntime "on"

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files {
            "%{prj.location}/src/**.h",
            "%{prj.location}/src/**.cpp",
            "%{prj.location}/src/**.hpp"
        }  

        includedirs {
            "%{IncludeDir.Cypher}"
        }

        links {
            "Cypher"
        }

        defines { "_CRT_SECURE_NO_WARNINGS" }

        filter "configurations:Debug"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            runtime "Release"
            optimize "on"