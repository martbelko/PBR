workspace "PBRApp"
	architecture "x86_64"

	startproject "PBRApp"

	defines
	{
		"ARCH_X64",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "CONF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CONF_RELEASE"
		runtime "Release"
		optimize "full"

	filter {}

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    targetdir("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	IncludeDir = {}
	IncludeDir["GLFW"] = "%{wks.location}/vendor/GLFW/include"
	IncludeDir["Glad"] = "%{wks.location}/vendor/Glad/include"

	group "Dependencies"
		include "vendor/GLFW"
		include "vendor/Glad"
	group ""

project "PBRApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"Glad"
	}

	defines
	{
		"GLFW_INCLUDE_NONE"
	}
