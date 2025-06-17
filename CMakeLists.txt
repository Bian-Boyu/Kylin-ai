cmake_minimum_required(VERSION 3.16)
project(KylinAiAssistant)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-fPIC)

find_package(PkgConfig REQUIRED)

pkg_check_modules(GIO REQUIRED gio-unix-2.0)
pkg_check_modules(POPPLER REQUIRED poppler-cpp)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# 确保找到 Qt5 的 Core 和 Widgets 组件
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Multimedia)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Multimedia)

pkg_check_modules(KYSDK_AI_VISION REQUIRED kysdk-coreai-vision)
pkg_check_modules(KYSDK_AI_SPEECH REQUIRED kysdk-coreai-speech)
pkg_check_modules(KYAI_EMBEDDING REQUIRED kysdk-coreai-embedding)
pkg_check_modules(KYAI_NLP REQUIRED kysdk-genai-nlp)
pkg_check_modules(KYAI_VISION REQUIRED kysdk-genai-vision)

# 启用 Qt 的自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 添加项目源文件
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
file(GLOB UI_HEADERS "src/*.ui")
qt5_wrap_ui(UI_SOURCES ${UI_HEADERS})

# 添加可执行文件
add_executable(${PROJECT_NAME} ${SOURCES} ${UI_SOURCES})

# 包含麒麟 SDK 和其他库的头文件目录
target_include_directories(${PROJECT_NAME} PRIVATE
    ${GIO_INCLUDE_DIRS}
    ${KYSDK_AI_VISION_INCLUDE_DIRS}
    ${KYSDK_AI_SPEECH_INCLUDE_DIRS}
    ${KYAI_EMBEDDING_INCLUDE_DIRS}
    ${KYAI_NLP_INCLUDE_DIRS}
    ${KYAI_VISION_INCLUDE_DIRS}
    ${POPPLER_CPP_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Multimedia_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src  # 添加 src 目录
)

# 设置编译选项
target_compile_options(${PROJECT_NAME} PRIVATE ${GTK3_CFLAGS_OTHER})

# 设置链接目录
target_link_directories(${PROJECT_NAME} PRIVATE
    ${GTK3_LIBRARY_DIRS}
)

# 链接麒麟 SDK 和其他库的库文件
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Multimedia
    ${GIO_LIBRARIES}
    ${KYSDK_AI_VISION_LIBRARIES}
    ${KYSDK_AI_SPEECH_LIBRARIES}
    ${KYAI_EMBEDDING_LIBRARIES}
    ${KYAI_NLP_LIBRARIES}
    ${KYAI_VISION_LIBRARIES}
    ${POPPLER_CPP_LIBRARIES}
    ${GTK3_LIBRARIES}
    pthread
)