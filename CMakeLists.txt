cmake_minimum_required(VERSION 3.10)

project(arm-mali-project)

# driver 폴더 빌드
add_subdirectory(driver)

# app 폴더 빌드
add_subdirectory(app)