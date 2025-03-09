#include "MaliDriverAPI.h"
#include <iostream>
#include <random>
#include <unordered_map>

namespace {

// Buffer Handle 관리를 위한 스토리지
    static std::unordered_map<uint64_t, size_t> gBufferMap;
    static bool gDriverInitialized = false;
    static uint64_t gNextBufferHandle = 1;

}

namespace MaliDriver {

    MaliStatus initDriver() {
        if (gDriverInitialized) {
            return MaliStatus::SUCCESS;
        }
        std::cout << "[MaliDriver] Driver Initialized\n";
        gDriverInitialized = true;
        return MaliStatus::SUCCESS;
    }

    MaliStatus shutdownDriver() {
        if (!gDriverInitialized) {
            return MaliStatus::SUCCESS;
        }
        // 혹시 남은 버퍼 있으면 정리
        gBufferMap.clear();
        gDriverInitialized = false;
        std::cout << "[MaliDriver] Driver Shutdown\n";
        return MaliStatus::SUCCESS;
    }

    MaliStatus compileShader(
            const std::string& sourceCode,
            MaliShaderType type,
            MaliShaderBinary& outBinary)
    {
        if (!gDriverInitialized) {
            return MaliStatus::ERROR_UNKNOWN;
        }
        if (sourceCode.empty()) {
            return MaliStatus::ERROR_INVALID_PARAM;
        }

        // 여기서는 단순히 “랜덤 바이트”를 생성해 바이너리를 채운다고 가정
        // 실제 DDK라면 GLSL->SPIR-V->ISA 등 복잡한 과정을 거치겠지만,
        // “compile 성공/실패”만 쓴다. 실제 DDK 시도 했다가 실패함 좀 따 다시 ㄱ
        bool compileSuccess = (sourceCode.find("error") == std::string::npos);
        if (!compileSuccess) {
            return MaliStatus::ERROR_COMPILATION_FAILED;
        }

        outBinary.binaryData.clear();
        outBinary.binaryData.resize(16); // 임의 길이
        std::random_device rd;
        for (auto &b : outBinary.binaryData) {
            b = static_cast<uint8_t>(rd() & 0xFF);
        }

        std::cout << "[MaliDriver] Shader compiled ("
                  << (type == MaliShaderType::VERTEX ? "VS" : "FS")
                  << ") with size=" << outBinary.binaryData.size() << "\n";
        return MaliStatus::SUCCESS;
    }

    MaliStatus allocateBuffer(size_t size, uint64_t& outBufferHandle) {
        if (!gDriverInitialized) {
            return MaliStatus::ERROR_UNKNOWN;
        }
        if (size == 0) {
            return MaliStatus::ERROR_INVALID_PARAM;
        }

        uint64_t handle = gNextBufferHandle++;
        gBufferMap[handle] = size;
        outBufferHandle = handle;

        std::cout << "[MaliDriver] Buffer allocated (handle="
                  << handle << ", size=" << size << ")\n";
        return MaliStatus::SUCCESS;
    }

    MaliStatus freeBuffer(uint64_t bufferHandle) {
        if (!gDriverInitialized) {
            return MaliStatus::ERROR_UNKNOWN;
        }

        auto it = gBufferMap.find(bufferHandle);
        if (it == gBufferMap.end()) {
            return MaliStatus::ERROR_INVALID_PARAM;
        }
        gBufferMap.erase(it);

        std::cout << "[MaliDriver] Buffer freed (handle="
                  << bufferHandle << ")\n";
        return MaliStatus::SUCCESS;
    }

    void debugPrint(const std::string& msg) {
        std::cout << "[MaliDriver Debug] " << msg << std::endl;
    }

} // namespace MaliDriver