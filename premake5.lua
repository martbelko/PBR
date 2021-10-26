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
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX",
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
	IncludeDir["glm"] = "%{wks.location}/vendor/glm"
	IncludeDir["stb_image"] = "%{wks.location}/vendor/stb_image"
	IncludeDir["ImGUI"] = "%{wks.location}/vendor/imgui"

	group "Dependencies"
		include "vendor/GLFW"
		include "vendor/Glad"
		include "vendor/imgui"
	group ""

project "PBRApp"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.ImGUI}"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"Glad",
		"imgui"
	}

	defines
	{
		"GLFW_INCLUDE_NONE"
	}
