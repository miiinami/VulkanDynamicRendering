cmake_minimum_required (VERSION 3.29)

project ("VulkanDynamicRendering")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Add option to enable/disable C++ 20 module
option(ENABLE_CPP20_MODULE "Enable C++ 20 module support for Vulkan" OFF)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if(ENABLE_CPP20_MODULE)
  set(CMAKE_CXX_SCAN_FOR_MODULES ON)
endif()

find_package (glfw3 REQUIRED)
find_package (Vulkan 1.4.335 REQUIRED)      # Require Vulkan SDK version 1.4.335 or higher
find_package (tinyobjloader REQUIRED)

# set up Vulkan C++ module only if enabled
if(ENABLE_CPP20_MODULE)
  add_library(VulkanCppModule)
  add_library(Vulkan::cppm ALIAS VulkanCppModule)

  target_compile_definitions(VulkanCppModule
          PUBLIC 
          VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 
          VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
  )
  target_include_directories(VulkanCppModule
          PUBLIC
          "${Vulkan_INCLUDE_DIR}"
  )
  target_link_libraries(VulkanCppModule
          PUBLIC
          Vulkan::Vulkan
  )

  set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

  # Add MSVC-specific compiler options for proper C++ module support
  if(MSVC)
    target_compile_options(VulkanCppModule PRIVATE
      /std:c++latest      # Use latest C++ standard for better module support
      /permissive-        # Standards conformance mode
      /Zc:__cplusplus     # Enable correct __cplusplus macro
      /EHsc               # Enable C++ exception handling
      /Zc:preprocessor    # Use conforming preprocessor
      /translateInclude   # Automatically translate #include to import for standard library
    )
  endif()

  target_sources(VulkanCppModule
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS
    "${Vulkan_INCLUDE_DIR}"
    FILES
    "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
  )


  # Add the vulkan.cppm file directly as a source file
  target_sources(VulkanCppModule
          PRIVATE
          "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
  )
else()
  # Create a dummy interface library when C++ 20 module is disabled
  add_library(VulkanCppModule INTERFACE)
  add_library(Vulkan::cppm ALIAS VulkanCppModule)
  target_link_libraries(VulkanCppModule INTERFACE Vulkan::Vulkan)
  target_compile_definitions(VulkanCppModule
          INTERFACE VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
  )
endif()

add_executable(${PROJECT_NAME} 
"Src/main.cpp"
"Src/Core/Core.cpp"
"Src/Core/VulkanInstance/VulkanInstance.cpp"
"Src/Core/GLFWwidnow/GLFWwindowClass.cpp"
"Src/Core/DebugMessenger/DebugMessenger.cpp" 
"Src/Core/VulkanDevice/VulkanDevice.cpp" 
"Src/Core/VulkanSurface/VulkanSurface.cpp" 
"Src/Core/SwapChain/VulkanSwapChain.cpp" 
"Src/Helper/ReadResources/ReadResources.cpp"
"Src/Core/GraphicsPipeline/GraphicsPipeline.cpp" 
"Src/Core/Buffer/CommandBuffer/CommandBuffer.cpp"  "Src/Core/RenderController/RenderController.cpp")

 target_include_directories(${PROJECT_NAME} PRIVATE "Src")

# 收集所有 .vert 和 .frag 文件（可按需扩展）
file(GLOB SHADER_FILES
    "${CMAKE_SOURCE_DIR}/Src/Shader/*.vert"
    "${CMAKE_SOURCE_DIR}/Src/Shader/*.frag"
)

# 输出目录：构建目录下的 shaders 文件夹
set(SHADER_OUTPUT_DIR "${CMAKE_BINARY_DIR}/Shader")
file(MAKE_DIRECTORY ${SHADER_OUTPUT_DIR})

set(SPV_FILES "")

# 为每个着色器文件生成编译命令
foreach(SHADER_FILE ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME_WE)   # 不带扩展名的文件名
    set(SPV_FILE "${SHADER_OUTPUT_DIR}/${SHADER_NAME}.spv")

    message(STATUS "glslc ${SHADER_FILE} -o ${SPV_FILE}")

    add_custom_command(
        OUTPUT ${SPV_FILE}
        COMMAND glslc "${SHADER_FILE}" -o "${SPV_FILE}"
        DEPENDS ${SHADER_FILE}
        COMMENT "Compiling ${SHADER_FILE}"
        VERBATIM
    )
    list(APPEND SPV_FILES ${SPV_FILE})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${SPV_FILES})
add_dependencies(${PROJECT_NAME} CompileShaders)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
)

if(ENABLE_CPP20_MODULE)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_CPP20_MODULES=1)
endif()

target_link_libraries(${PROJECT_NAME}
  Vulkan::cppm
  glfw
)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:NDEBUG>)