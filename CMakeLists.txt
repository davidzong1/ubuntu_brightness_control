cmake_minimum_required(VERSION 3.10)  # 可根据项目需求调整版本号
project(MyProject VERSION 1.0.0 LANGUAGES C)  # 替换为实际项目名和版本信息
set(CMAKE_C_STANDARD 11)
add_executable(davidzong_brightness_control 
                ./main.c)  # 添加可执行文件
install(TARGETS davidzong_brightness_control DESTINATION /usr/bin)  # 安装可执行文件到指定目录