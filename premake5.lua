workspace "Cypher"
    architecture "x64"
    startproject "CypherTest"
    configurations { "Debug", "Release" }
    flags { "MultiProcessorCompile" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    IncludeDir = {}
    IncludeDir["Cypher"] = "Cypher/src"
    IncludeDir["entt"] = "Vendor/entt/"

    group "Dependencies"
        -- Dependency projects would go here

    group ""

    project "Cypher"
        location "Cypher"
        kind "SharedLib"  -- Changed from ConsoleApp to SharedLib
        language "C++"
        cppdialect "C++20"
        staticruntime "off"  -- Usually, DLLs use a dynamic runtime

        targetdir ("bin/" .. outputdir .. "/%{prj.name}")
        objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

        files {
            "%{IncludeDir.Cypher}/**.h",
            "%{IncludeDir.Cypher}/**.cpp",
            "%{IncludeDir.Cypher}/**.hpp"
        }

        includedirs {
            "%{IncludeDir.Cypher}",
            "%{IncludeDir.entt}"
        }

        defines { "_CRT_SECURE_NO_WARNINGS", "CYPHER_EXPORTS" }

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
            "%{IncludeDir.Cypher}",
            "%{IncludeDir.entt}"
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