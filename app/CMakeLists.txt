cmake_minimum_required(VERSION 3.10)
project(MaliDriverApp)

set(APP_SOURCES
        src/main.cpp
)

add_executable(MaliDriverApp ${APP_SOURCES})

# 드라이버 라이브러리를 링크
# (MaliDriver 타겟을 찾기 위해, 상위 CMake에서 add_subdirectory(driver))
target_include_directories(MaliDriverApp
        PRIVATE
        ${CMAKE_SOURCE_DIR}/driver/include
)

target_link_libraries(MaliDriverApp
        PRIVATE
        MaliDriver
)