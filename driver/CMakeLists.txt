cmake_minimum_required(VERSION 3.10)

project(MaliDriverLib)

# 소스 파일 지정
set(DRIVER_SOURCES
        src/MaliDriverAPI.cpp
)

add_library(MaliDriver SHARED ${DRIVER_SOURCES})

target_include_directories(MaliDriver
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 필요 시 타겟에 컴파일 플래그, 라이브러리 링크 등 추가