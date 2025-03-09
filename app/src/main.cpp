#include <iostream>
#include "MaliDriverAPI.h"

int main() {
    using namespace MaliDriver;

    // 1) 드라이버 초기화
    auto status = initDriver();
    if (status != MaliStatus::SUCCESS) {
        std::cerr << "initDriver failed\n";
        return 1;
    }

    // 2) 셰이더 컴파일 (정상 케이스)
    {
        std::string vsCode = "#version 310 es\nvoid main() { /* no error */ }";
        MaliShaderBinary vsBin;
        status = compileShader(vsCode, MaliShaderType::VERTEX, vsBin);
        if (status == MaliStatus::SUCCESS) {
            std::cout << "Vertex shader compiled, bin size=" << vsBin.binaryData.size() << "\n";
        } else {
            std::cout << "Vertex shader compile FAIL\n";
        }
    }

    // 3) 셰이더 컴파일 (에러 케이스)
    {
        std::string fsCode = "error"; // 일부러 "error" 라고 써서 실패 유도
        MaliShaderBinary fsBin;
        status = compileShader(fsCode, MaliShaderType::FRAGMENT, fsBin);
        if (status == MaliStatus::SUCCESS) {
            std::cout << "Fragment shader compiled, bin size=" << fsBin.binaryData.size() << "\n";
        } else {
            std::cout << "Fragment shader compile FAIL, status=" << (int)status << "\n";
        }
    }

    // 4) 버퍼 할당 / 해제
    {
        uint64_t bufHandle = 0;
        status = allocateBuffer(1024, bufHandle);
        if (status == MaliStatus::SUCCESS) {
            debugPrint("Allocated buffer handle=" + std::to_string(bufHandle));
            // free
            freeBuffer(bufHandle);
        }
    }

    // 5) 드라이버 종료
    shutdownDriver();

    return 0;
}