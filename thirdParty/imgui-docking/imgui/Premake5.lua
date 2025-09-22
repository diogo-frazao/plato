workspace "AIFramework"
	architecture "x64"

	configurations{
		"Debug",
		"Release"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "AIFramework"
	location "AIFramework"
	kind "StaticLib"
	language "C++"

	targetdir("Binaries/" .. outputdir .. "/%{prj.name}")
	objdir("Intermediates/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs{
		"%{prj.name}/Source"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "10.0"

		defines{

		}

		postbuildcommands{
			--"{RMDIR} ../Binaries/" .. outputdir .. "/Sample",
			"{MKDIR} ../Binaries/" .. outputdir .. "/Sample",
			"{COPYFILE} %{cfg.buildtarget.relpath} ../Binaries/" .. outputdir .. "/Sample",

			--"{RMDIR} ../Binaries/" .. outputdir .. "/Test",
			"{MKDIR} ../Binaries/" .. outputdir .. "/Test",
			"{COPYFILE} %{cfg.buildtarget.relpath} ../Binaries/" .. outputdir .. "/Test",
			
			--"{RMDIR} ../Binaries/" .. outputdir .. "/Editor",
			"{MKDIR} ../Binaries/" .. outputdir .. "/Editor",
			"{COPYFILE} %{cfg.buildtarget.relpath} ../Binaries/" .. outputdir .. "/Editor"
		}

	filter "configurations:Debug"
		defines "AI_FRAMEWORK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "AI_FRAMEWORK_RELEASE"
		optimize "On"



project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"

	targetdir("Binaries/" .. outputdir .. "/%{prj.name}")
	objdir("Intermediates/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp",
		
		"ThirdParty/imgui/imconfig.h", 
		"ThirdParty/imgui/imgui.h", 
		"ThirdParty/imgui/imgui.cpp", 
		
		"ThirdParty/imgui/imgui_demo.cpp", 
		"ThirdParty/imgui/imgui_draw.cpp", 
		"ThirdParty/imgui/imgui_internal.h", 
		"ThirdParty/imgui/imgui_tables.cpp", 
		"ThirdParty/imgui/imgui_widgets.cpp",

		"ThirdParty/imgui/misc/cpp/imgui_stdlib.h",
		"ThirdParty/imgui/misc/cpp/imgui_stdlib.cpp",
			
		"ThirdParty/imgui/backends/imgui_impl_sdl2.h", 
		"ThirdParty/imgui/backends/imgui_impl_sdl2.cpp", 
		"ThirdParty/imgui/backends/imgui_impl_sdlrenderer2.h", 
		"ThirdParty/imgui/backends/imgui_impl_sdlrenderer2.cpp" 
	}

	includedirs{
		"%{prj.name}/Source",
		"%{AIFramework}/AIFramework/Source",
		"ThirdParty/imgui",
		"ThirdParty/imgui/backends",
		"ThirdParty/imgui/misc/cpp",
		"ThirdParty/SDL2/include",
		"ThirdParty/SDL2_image/include"
	}
	
	libdirs{
		"ThirdParty/SDL2/lib/%{cfg.architecture}",
		"ThirdParty/SDL2_image/lib/%{cfg.architecture}"
	}
	
	links{
		"AIFramework",
		"SDL2",
		"SDL2main",
		"SDL2_image"
	}
	
	postbuildcommands { 
		"{COPYFILE} ../ThirdParty/SDL2/lib/%{cfg.architecture}/SDL2.dll %{cfg.targetdir}" ,
		"{COPYFILE} ../ThirdParty/SDL2_image/lib/%{cfg.architecture}/SDL2_image.dll %{cfg.targetdir}"
	}

	filter "system:windows"
		cppdialect "C++20"
		systemversion "10.0"

		defines{

		}

	filter "configurations:Debug"
		defines "AI_FRAMEWORK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "AI_FRAMEWORK_RELEASE"
		optimize "On"




project "Sample"
	location "Sample"
	kind "ConsoleApp"

	language "C++"

	targetdir ("Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediates/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}

	includedirs{
		"%{prj.name}/Source",
		"%{AIFramework}/AIFramework/Source",
	}

	links{
		"AIFramework"
	}
	
	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "10.0"

	filter "system:windows"
		cppdialect "C++20"
		systemversion "10.0"

		defines{

		}

	filter "configurations:Debug"
		defines "AI_FRAMEWORK_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "AI_FRAMEWORK_RELEASE"
		optimize "On"



project "Test"
	location "Test"
	kind "ConsoleApp"
	language "C++"

	targetdir ("Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("Intermediates/" .. outputdir .. "/%{prj.name}")

	files{
		"%{prj.name}/Source/**.h",
		"%{prj.name}/Source/**.cpp"
	}
	
	includedirs{
		"%{prj.name}/Source",
		"%{AIFramework}/AIFramework/Source"
	}
	
	nuget{
		"Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn:1.8.1.7"
	}
	
	links{
		"AIFramework", 
	}
	
	filter "system:windows"
		cppdialect "C++20"
		systemversion "10.0"
		
	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"