cmake_minimum_required(VERSION 3.16)
project(QtMindMap LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6库
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# 包含当前目录和include目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# 添加可执行文件
add_executable(${PROJECT_NAME}
    main.cpp
    src/Connection.cpp
    src/MainWindow.cpp
    src/MindMapNode.cpp
    src/MindMapScene.cpp
    include/Connection.h
    include/mainwindow.h
    include/MindMapNode.h
    include/MindMapScene.h
)

# 链接Qt库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Widgets
)

# 自动处理moc文件
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

