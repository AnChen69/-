#pragma once

#include <string>
#include <vector>
#include <memory>
#include <openssl/aes.h>
#include <openssl/rsa.h>

using namespace std;

class MessageEncryption {
public:
    MessageEncryption();
    ~MessageEncryption();

    // 初始化加密器
    bool initialize();

    // AES加密/解密
    vector<unsigned char> encryptAES(const string& plaintext);
    string decryptAES(const vector<unsigned char>& ciphertext);

    // 使用指定的密钥和IV
    vector<unsigned char> encryptAES(const string& plaintext,
        const vector<unsigned char>& key,
        const vector<unsigned char>& iv);
    string decryptAES(const vector<unsigned char>& ciphertext,
        const vector<unsigned char>& key,
        const vector<unsigned char>& iv);

    // 密钥管理
    void generateAESKey();
    void setAESKey(const vector<unsigned char>& key);
    const vector<unsigned char>& getAESKey() const;
    const vector<unsigned char>& getIV() const;

    // 获取加密信息
    size_t getKeySize() const { return aesKey.size(); }
    size_t getIVSize() const { return iv.size(); }
    bool isInitialized() const { return initialized; }

private:
    vector<unsigned char> aesKey;
    vector<unsigned char> iv;
    bool initialized = false;

    void cleanup();
};