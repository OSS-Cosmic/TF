cmake_minimum_required(VERSION 3.26)
cmake_policy(SET CMP0112 NEW)
cmake_policy(SET CMP0141 NEW) # https://cmake.org/cmake/help/latest/policy/CMP0141.html#policy:CMP0141
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

option(TF_SAMPLES "Build TF Samples" OFF)
option(TF_TESTS "Build TF Test cases" OFF)

include(utils)
enable_language(ASM)

add_subdirectory(External)
set(EXTERNAL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/External")
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources")

set(FSL_COMPILER_PY "${CMAKE_CURRENT_SOURCE_DIR}/Common_3/Tools/ForgeShadingLanguage/fsl.py" )


add_library(cpu_features OBJECT 
     Common_3/OS/ThirdParty/OpenSource/cpu_features/src/impl_x86_macos.c
     Common_3/OS/ThirdParty/OpenSource/cpu_features/src/impl_aarch64_iOS.c
     Common_3/OS/ThirdParty/OpenSource/cpu_features/src/impl_x86_windows.c
     Common_3/OS/ThirdParty/OpenSource/cpu_features/src/impl_x86_linux_or_android.c
)

#file(GLOB BASIS_UNVERSAL_SOURCES 
#    Common_3/Utilities/ThirdParty/OpenSource/basis_universal/*.c
#    Common_3/Utilities/ThirdParty/OpenSource/basis_universal/transcoder/*.cpp
#)
#add_library(basis_universal STATIC ${BASIS_UNVERSAL_SOURCES})
#target_include_directories(basis_universal PUBLIC Common_3/Utilities/ThirdParty/OpenSource/basis_universal)

add_library(ags SHARED IMPORTED IMPORTED GLOBAL)
set_property(TARGET ags PROPERTY IMPORTED_LOCATION
         Common_3/Graphics/ThirdParty/OpenSource/ags/ags_lib/lib/amd_ags_x64.dll
        )
set_property(TARGET ags PROPERTY IMPORTED_IMPLIB
         Common_3/Graphics/ThirdParty/OpenSource/ags/ags_lib/lib/amd_ags_x64.lib
        )
target_include_directories(ags INTERFACE  Common_3/Graphics/ThirdParty/OpenSource/ags)

# add_library(tinyEXR STATIC Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/TinyEXR/tinyexr.cc)
#target_include_directories(tinyEXR PRIVATE 
#    Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/TinyEXR)

  
# add_library(astc-encoder OBJECT 
#     Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/astc-encoder/Source/*.c
#     Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/astc-encoder/Source/*.cpp
# )
# target_include_directories(astc-encoder PRIVATE 
#     Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/TinyEXR)
# target_include_directories(astc-encoder  PUBLIC Common_3/Utilities/ThirdParty/OpenSource/zip)
 add_library(lz4 OBJECT 
     Common_3/Utilities/ThirdParty/OpenSource/lz4/lz4.c
     Common_3/Utilities/ThirdParty/OpenSource/lz4/lz4hc.c
 )
 target_include_directories(lz4 PUBLIC  
     Common_3/Utilities/ThirdParty/OpenSource/lz4)

file(GLOB ZSTD_FILESYSTEM_SOURCES
     Common_3/Utilities/ThirdParty/OpenSource/zstd/common/*.c
     Common_3/Utilities/ThirdParty/OpenSource/zstd/compress/*.c
     Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/*.c
     Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/*.S
)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    list(APPEND ZSTD_FILESYSTEM_SOURCES Common_3/Utilities/ThirdParty/OpenSource/zstd/decompress/huf_decompress_amd64.S)
endif()
add_library(zstd_filesystem OBJECT ${ZSTD_FILESYSTEM_SOURCES}) 
target_include_directories(zstd_filesystem   PUBLIC Common_3/Utilities/ThirdParty/OpenSource/zstd)

# VulkanMemoryAllocator
add_library(VulkanMemoryAllocator INTERFACE)
target_include_directories(VulkanMemoryAllocator INTERFACE
    Common_3/Graphics/ThirdParty/OpenSource/VulkanMemoryAllocator)


# cglf
add_library(cgltf INTERFACE)
target_include_directories(cgltf INTERFACE Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/cgltf)

#BStr

# add_library(volk STATIC 
#     Common_3/Graphics/ThirdParty/OpenSource/volk/volk.c)
# target_include_directories(volk PUBLIC Common_3/Graphics/ThirdParty/OpenSource/volk)

add_library(tinyimageformat INTERFACE)
target_include_directories(tinyimageformat INTERFACE 
    Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/tinyimageformat)

#file(GLOB OZZ_ANIMATION_SOURCES 
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/animation/offline/*.cc
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/animation/runtime/*.cc
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/base/*.cc
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/base/containers/*.cc
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/base/io/*.cc
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/base/maths/*.cc
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/base/memory/*.cc
#    Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/src/geometry/runtime/*.cc)
#
#add_library(ozz-animation STATIC ${OZZ_ANIMATION_SOURCES})
#target_include_directories(ozz-animation PUBLIC Common_3/Resources/AnimationSystem/ThirdParty/OpenSource/ozz-animation/include)


#file(GLOB MESH_OPTIMIZER_SOURCES Common_3/Tools/ThirdParty/OpenSource/meshoptimizer/src/*.cpp)
#add_library(meshoptimizer STATIC ${MESH_OPTIMIZER_SOURCES})
#target_include_directories(meshoptimizer PUBLIC Common_3/Tools/ThirdParty/OpenSource/meshoptimizer/src)

add_library(tinydds INTERFACE)
target_include_directories(tinydds INTERFACE 
    Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/tinydds)

add_library(tinyktx INTERFACE)
target_include_directories(tinyktx INTERFACE 
    Common_3/Resources/ResourceLoader/ThirdParty/OpenSource/tinyktx)

file(GLOB THE_FORGE_SOURCES
    Common_3/Graphics/*.cpp
    Common_3/Application/*.cpp
    Common_3/Application/Profiler/*.cpp
    Common_3/Application/Fonts/FontSystem.cpp
    Common_3/Application/UI/UI.cpp
    Common_3/Application/Profiler/*.cpp
    Common_3/OS/WindowSystem/*.cpp
    Common_3/OS/*.cpp

    Common_3/Resources/ResourceLoader/*.cpp

    Common_3/Utilities/FileSystem/FileSystem.c
    Common_3/Utilities/FileSystem/SystemRun.c
    Common_3/Utilities/Log/*.c
    Common_3/Utilities/Math/*.c
    Common_3/Utilities/MemoryTracking/*.c
    Common_3/Utilities/Threading/*.c
    Common_3/Utilities/*.c
   
    Common_3/Tools/Network/*.c
    Common_3/Tools/ReloadServer/*.cpp

    Common_3/Renderer/ShaderServer/Network.c
    Common_3/Renderer/ShaderServer/ShaderServer.cpp

    Common_3/Application/InputSystem.cpp
    

    Common_3/Utilities/ThirdParty/OpenSource/bstrlib/bstrlib.c
    
    Common_3/Tools/ThirdParty/OpenSource/meshoptimizer/src/*.cpp
    
   Source/Forge/Core/TF_String.cpp
   Source/Forge/Core/TF_Core.cpp
   Source/Forge/Core/Mem/TF_Allocators.cpp
   
   Source/Forge/Graphics/TF_GPUConfig.cpp
   Source/Forge/Graphics/TF_Graphics.cpp
   Source/Forge/Graphics/Vulkan/Vulkan.cpp
   Source/Forge/Graphics/Vulkan/VulkanRaytracing.cpp 
   Source/Forge/Graphics/Vulkan/VulkanShaderReflection.cpp
)
add_library(TF STATIC ${THE_FORGE_SOURCES})

# ga input
file(GLOB GAINPUT_SOURCES 
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/builtin/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/dev/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/gestures/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/keyboard/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/mouse/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/pad/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/recorder/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/touch/*.cpp   
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/*.cpp   
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/*.cpp   
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    file(GLOB GAINPUT_SOURCES_MACOS
        Common_3/Application/ThirdParts/OpenSource/gainput/lib/source/hidapi/mac/*.c
        Common_3/Application/ThirdParts/OpenSource/gainput/lib/source/gainput/*.mm)
    list(APPEND GAINPUT_SOURCES ${GAINPUT_SOURCES_MACOS})
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    file(GLOB GAINPUT_SOURCES_WINDOWS
        Common_3/Application/ThirdParts/OpenSource/gainput/lib/source/hidapi/windows/*.c
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/*.cpp
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/gainput/hid/hidparsers/*.cpp
    )
    list(APPEND GAINPUT_SOURCES ${GAINPUT_SOURCES_WINDOWS})
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    file(GLOB GAINPUT_SOURCES_LINUX
        Common_3/Application/ThirdParty/OpenSource/gainput/lib/source/hidapi/linux/*.c)
    list(APPEND GAINPUT_SOURCES ${GAINPUT_SOURCES_LINUX})
    target_link_libraries(TF PRIVATE udev)
endif()

# volk
add_library(volk STATIC 
    Common_3/Graphics/ThirdParty/OpenSource/volk/volk.c)
if(WIN32)
    target_include_directories(volk  PUBLIC Common_3/Graphics/ThirdParty/OpenSource/VulkanSDK/include)
endif()
target_include_directories(volk PUBLIC Common_3/Graphics/ThirdParty/OpenSource/volk)

# SPIRV_Cross
file(GLOB SPIRV_CROSS_SOURCES Common_3/Graphics/ThirdParty/OpenSource/SPIRV_Cross/*.cpp)
add_library(SPIRV_Cross STATIC ${SPIRV_CROSS_SOURCES})
set_property(TARGET SPIRV_Cross PROPERTY CXX_STANDARD 17)

# imgui
file(GLOB IMGUI_SOURCES Common_3/Application/ThirdParty/OpenSource/imgui/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC Common_3/ThirdParty/OpenSource/imgui)

target_compile_definitions(
    TF PRIVATE -DSTB_TRUETYPE_IMPLEMENTATION
)
target_link_libraries(TF PUBLIC cpu_features imgui cgltf lz4 zstd_filesystem tinydds tinyimageformat tinyktx )
target_include_directories(TF PUBLIC ${THE_FORGE_DIR})

target_sources(TF PRIVATE ${GAINPUT_SOURCES})
IF(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    file(GLOB THE_FORGE_OS_DARWIN_SOURCES
        Common_3/OS/Darwin/*.cpp
        Common_3/OS/Darwin/*.c)
    target_sources(TF PRIVATE ${THE_FORGE_OS_DARWIN_SOURCES})
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    file(GLOB THE_FORGE_OS_WINDOWS_SOURCES
        Common_3/OS/Windows/*.cpp
        Common_3/OS/Windows/*.c)
        
    target_sources(TF PRIVATE ${THE_FORGE_OS_WINDOWS_SOURCES})
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    include(FindPkgConfig)

    file(GLOB THE_FORGE_OS_LINUX_SOURCES
        Common_3/OS/Linux/*.cpp
        Common_3/OS/Linux/*.c
        Common_3/Utilities/FileSystem/UnixFileSystem.c)
    target_sources(TF PRIVATE ${THE_FORGE_OS_LINUX_SOURCES})

    pkg_check_modules(GTK REQUIRED gtk+-3.0)

    target_link_libraries(TF PRIVATE ${GTK_LIBRARIES})
    target_include_directories(TF PRIVATE ${GTK_INCLUDE_DIRS})
endif()

file(GLOB GRAPHICS_VULKAN_SOURCE
    "Common_3/Graphics/Vulkan/*.cpp")
target_sources(TF PRIVATE ${GRAPHICS_VULKAN_SOURCE})
target_link_libraries(TF PRIVATE SPIRV_Cross volk)


# WinPixEventRuntime
add_library(WinPixEventRuntime SHARED IMPORTED GLOBAL)
set_property(TARGET WinPixEventRuntime PROPERTY IMPORTED_LOCATION
        Common_3/OS/ThirdParty/OpenSource/winpixeventruntime/bin/WinPixEventRuntime.dll)
set_property(TARGET WinPixEventRuntime PROPERTY IMPORTED_IMPLIB
        Common_3/OS/ThirdParty/OpenSource/winpixeventruntime/bin/WinPixEventRuntime.lib)

# DirectXShaderCompiler 
add_library(DirectXShaderCompiler SHARED IMPORTED GLOBAL)
set_property(TARGET DirectXShaderCompiler PROPERTY IMPORTED_LOCATION
        Common_3/Graphics/ThirdParty/OpenSource/DirectXShaderCompiler/bin/x64/dxcompiler.dll)
set_property(TARGET DirectXShaderCompiler PROPERTY IMPORTED_IMPLIB
    Common_3/Graphics/ThirdParty/OpenSource/DirectXShaderCompiler/lib/x64/dxcompiler.lib)     

# Nvapi
add_library(Nvapi STATIC IMPORTED)
set_property(TARGET Nvapi PROPERTY IMPORTED_LOCATION
    Common_3/Graphics/ThirdParty/OpenSource/nvapi/amd64/nvapi64.lib
    )

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    file(GLOB GRAPHICS_D3D12_SOURCE
        "Common_3/Graphics/Direct3D12/*.cpp"
        "Common_3/Graphics/Direct3D11/*.cpp"
    )
    target_sources(TF PRIVATE ${GRAPHICS_D3D12_SOURCE})

    target_include_directories(TF PRIVATE Common_3/Graphics/ThirdParty/OpenSource/D3D12MemoryAllocator)
    target_link_libraries(TF PRIVATE Nvapi ags)

    #file(GLOB GRAPHICS_D3D11_SOURCE "Common_3/Graphics/Direct3D11/*.cpp")
    #target_sources(TF PRIVATE ${GRAPHICS_D3D11_SOURCE})
    # "dxguid.lib"
    target_link_libraries(TF PRIVATE DirectXShaderCompiler "d3d11.lib")
    target_link_libraries(TF PRIVATE WinPixEventRuntime)
endif()
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(X11 REQUIRED)
    target_link_libraries(TF PRIVATE X11 X11::Xrandr)
endif()

target_include_directories(TF PUBLIC "${EXTERNAL_DIR}/VulkanSDK/include")
target_include_directories(TF PUBLIC "${EXTERNAL_DIR}")
target_include_directories(TF PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Include")
target_include_directories(TF PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(TF PUBLIC "Common_3/Utilities/ThirdParty/OpenSource/bstrlib")
target_include_directories(TF PUBLIC "Common_3/Tools/ThirdParty/OpenSource/meshoptimizer/src")

add_subdirectory(TFSamples)
add_subdirectory(Test)

