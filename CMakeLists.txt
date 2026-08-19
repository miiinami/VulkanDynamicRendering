cmake_minimum_required (VERSION 3.29)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VulkanDynamicRendering")

## 设置 C++ 标准（建议 C++20，因为官方模板支持模块）
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ------------------------------------------------------------
# 1. 查找 Vulkan SDK
#    官方推荐使用 FindVulkan.cmake（可从 VulkanSamples 获取）
#    这里假设将 FindVulkan.cmake 放在项目根目录下的 cmake/ 文件夹中
# ------------------------------------------------------------
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(Vulkan REQUIRED)

# 如果 Vulkan 提供了 C++ 模块支持（可选），可以启用
option(ENABLE_CPP20_MODULE "Enable Vulkan C++20 module" OFF)

# ------------------------------------------------------------
# 2. 查找其他依赖（通过 vcpkg 安装）
#    这些包在 vcpkg 中提供 CONFIG 模式
# ------------------------------------------------------------
find_package(GLFW3 REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(tinyobjloader REQUIRED CONFIG)

# ------------------------------------------------------------
# 3. 添加可执行文件
# ------------------------------------------------------------
add_executable(${PROJECT_NAME} main.cpp)

# ------------------------------------------------------------
# 4. 链接库
# ------------------------------------------------------------
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Vulkan::Vulkan          # Vulkan 加载器
        glfw                    # glfw3 目标（vcpkg 提供）
        glm::glm                # GLM 仅头文件库
        tinyobjloader::tinyobjloader
)

# Windows 下可能需要额外链接的系统库（Vulkan::Vulkan 已处理大部分）
if (WIN32)
    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            # 有时需要 ws2_32, winmm 等，Vulkan 加载器通常已包含
    )
endif()

# ------------------------------------------------------------
# 5. 可选：启用 Vulkan C++20 模块（实验性）
# ------------------------------------------------------------
if (ENABLE_CPP20_MODULE)
    # 此部分与官方文档中的模板一致
    find_package(Vulkan REQUIRED) # 已查找过，可复用

    add_library(VulkanCppModule)
    add_library(Vulkan::cppm ALIAS VulkanCppModule)

    target_compile_definitions(VulkanCppModule PUBLIC
        VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
    )

    target_include_directories(VulkanCppModule PRIVATE "${Vulkan_INCLUDE_DIR}")
    target_link_libraries(VulkanCppModule PUBLIC Vulkan::Vulkan)
    set_target_properties(VulkanCppModule PROPERTIES CXX_STANDARD 20)

    target_sources(VulkanCppModule
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS "${Vulkan_INCLUDE_DIR}"
        FILES "${Vulkan_INCLUDE_DIR}/vulkan/vulkan.cppm"
    )

    # 将模块链接到主项目
    target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::cppm)
    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_CPP20_MODULES=1)
endif()