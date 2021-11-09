workspace "PBRApp"
	architecture "x86_64"

	startproject "PBRApp"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile",
		"ShadowedVariables"
	}

	filter "system:windows"
		systemversion "latest"
		defines
		{
			"WIN32_LEAN_AND_MEAN",
			"NOMINMAX"
		}

	filter "configurations:Debug"
		defines "CONF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "CONF_RELEASE"
		runtime "Release"
		optimize "Speed"
		inlining "Auto"
		flags
		{
			"LinkTimeOptimization"
		}

	filter {}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

	IncludeDir = {}
	IncludeDir["GLFW"] = "%{wks.location}/PBRApp/vendor/GLFW/include"
	IncludeDir["Glad"] = "%{wks.location}/PBRApp/vendor/Glad/include"
	IncludeDir["glm"] = "%{wks.location}/PBRApp/vendor/glm"
	IncludeDir["stb_image"] = "%{wks.location}/PBRApp/vendor/stb_image"
	IncludeDir["ImGUI"] = "%{wks.location}/PBRApp/vendor/imgui"

	group "Dependencies"
		include "PBRApp/vendor/GLFW"
		include "PBRApp/vendor/Glad"
		include "PBRApp/vendor/imgui"
	group ""

	include "PBRApp"
