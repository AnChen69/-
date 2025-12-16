#pragma once

// 安全通信工具 - 公共定义
// 文件编码：UTF-8

#include <string>
#include <vector>
#include <QString>

using namespace std;

// === 常量定义 ===
constexpr int MAX_MESSAGE_LENGTH = 4096;
constexpr int MAX_FILENAME_LENGTH = 256;
constexpr int AES_KEY_SIZE = 32;       // AES-256
constexpr int RSA_KEY_SIZE = 2048;     // RSA-2048
constexpr int IV_SIZE = 16;            // AES IV大小
constexpr int AES_BLOCK_LENGTH = 16;     // AES块大小（固定16字节）

// === 枚举定义 ===
enum class EncryptionAlgorithm {
    AES_256_CBC,
    RSA_2048,
    RSA_3072,
    RSA_4096
};

enum class TransferStatus {
    PENDING,
    TRANSFERRING,
    COMPLETED,
    FAILED,
    CANCELLED
};

// === 工具函数 ===
inline QString toQString(EncryptionAlgorithm algo) {
    switch (algo) {
    case EncryptionAlgorithm::AES_256_CBC: return "AES-256-CBC";
    case EncryptionAlgorithm::RSA_2048: return "RSA-2048";
    case EncryptionAlgorithm::RSA_3072: return "RSA-3072";
    case EncryptionAlgorithm::RSA_4096: return "RSA-4096";
    default: return "Unknown";
    }
}