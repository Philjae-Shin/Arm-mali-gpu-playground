#pragma once
#include <cstdint>
#include <string>
#include <vector>

// 에러코드
enum class MaliStatus {
    SUCCESS = 0,
    ERROR_INVALID_PARAM,
    ERROR_COMPILATION_FAILED,
    ERROR_UNKNOWN
};

// 셰이더 종류
enum class MaliShaderType {
    VERTEX,
    FRAGMENT
};

// 컴파일 결과 구조체
struct MaliShaderBinary {
    std::vector<uint8_t> binaryData;
    // 실제론 GPU ISA가 들어가지만, 여기선 가상 데이터
};

// DDK API
namespace MaliDriver {

    // 드라이버 초기화 / 종료
    MaliStatus initDriver();
    MaliStatus shutdownDriver();

    // 셰이더 컴파일
    MaliStatus compileShader(
            const std::string& sourceCode,
            MaliShaderType type,
            MaliShaderBinary& outBinary);

    // 버퍼 할당
    MaliStatus allocateBuffer(
            size_t size,
            uint64_t& outBufferHandle);

    // 버퍼 해제
    MaliStatus freeBuffer(uint64_t bufferHandle);

    // 디버그 메시지
    void debugPrint(const std::string& msg);

} // namespace MaliDriver