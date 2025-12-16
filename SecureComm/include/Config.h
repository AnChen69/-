#pragma once

#define PROJECT_NAME "SecureComm"
#define VERSION "1.0.0"
#define DEFAULT_SERVER_PORT 8888
#define DEFAULT_SSL_PORT 8889
#define MAX_CONNECTIONS 100

// 服务器配置
constexpr const char* SERVER_IP = "127.0.0.1";
constexpr int BUFFER_SIZE = 4096;
constexpr int HEARTBEAT_INTERVAL = 30000; // 30秒心跳

// 文件传输配置
constexpr int FILE_CHUNK_SIZE = 65536;    // 64KB分片
constexpr int MAX_FILE_SIZE = 1024 * 1024 * 100; // 100MB最大文件