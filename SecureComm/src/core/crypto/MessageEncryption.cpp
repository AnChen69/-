#include "MessageEncryption.h"
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <stdexcept>
#include <iostream>
#include "../../../include/CommonDefines.h"

using namespace std;

MessageEncryption::MessageEncryption() {
    if (!initialize()) {
        cerr << "警告: MessageEncryption 初始化失败" << endl;
    }
}

MessageEncryption::~MessageEncryption() {
    cleanup();
}

void MessageEncryption::cleanup() {
    if (!aesKey.empty()) {
        OPENSSL_cleanse(aesKey.data(), aesKey.size());
        aesKey.clear();
    }
    if (!iv.empty()) {
        OPENSSL_cleanse(iv.data(), iv.size());
        iv.clear();
    }
    initialized = false;
}

bool MessageEncryption::initialize() {
    try {
        cleanup();

        aesKey.resize(AES_KEY_SIZE);
        iv.resize(IV_SIZE);

        cout << "正在生成AES密钥 (" << AES_KEY_SIZE << "字节)..." << endl;

        if (RAND_bytes(aesKey.data(), aesKey.size()) != 1) {
            cerr << "生成AES密钥失败" << endl;
            return false;
        }

        cout << "正在生成IV (" << IV_SIZE << "字节)..." << endl;

        if (RAND_bytes(iv.data(), iv.size()) != 1) {
            cerr << "生成IV失败" << endl;
            return false;
        }

        initialized = true;
        cout << "AES密钥生成成功!" << endl;
        return true;
    }
    catch (const exception& e) {
        cerr << "初始化错误: " << e.what() << endl;
        return false;
    }
}

vector<unsigned char> MessageEncryption::encryptAES(const string& plaintext) {
    if (!initialized) {
        throw runtime_error("MessageEncryption 未初始化");
    }
    return encryptAES(plaintext, aesKey, iv);
}

string MessageEncryption::decryptAES(const vector<unsigned char>& ciphertext) {
    if (!initialized) {
        throw runtime_error("MessageEncryption 未初始化");
    }
    return decryptAES(ciphertext, aesKey, iv);
}

vector<unsigned char> MessageEncryption::encryptAES(const string& plaintext,
    const vector<unsigned char>& key,
    const vector<unsigned char>& iv) {
    if (key.size() != AES_KEY_SIZE) {
        throw runtime_error("密钥长度必须为 " + to_string(AES_KEY_SIZE) + " 字节");
    }
    if (iv.size() != IV_SIZE) {
        throw runtime_error("IV长度必须为 " + to_string(IV_SIZE) + " 字节");
    }

    AES_KEY enc_key;
    if (AES_set_encrypt_key(key.data(), key.size() * 8, &enc_key) != 0) {
        throw runtime_error("设置AES加密密钥失败");
    }

    size_t plaintext_len = plaintext.length();
    size_t padded_len = ((plaintext_len + AES_BLOCK_LENGTH) / AES_BLOCK_LENGTH) * AES_BLOCK_LENGTH;

    vector<unsigned char> plaintext_padded(padded_len);
    copy(plaintext.begin(), plaintext.end(), plaintext_padded.begin());

    unsigned char pad_value = static_cast<unsigned char>(padded_len - plaintext_len);
    for (size_t i = plaintext_len; i < padded_len; ++i) {
        plaintext_padded[i] = pad_value;
    }

    vector<unsigned char> ciphertext(padded_len);
    vector<unsigned char> iv_copy = iv;

    AES_cbc_encrypt(
        plaintext_padded.data(),
        ciphertext.data(),
        padded_len,
        &enc_key,
        iv_copy.data(),
        AES_ENCRYPT
    );

    OPENSSL_cleanse(&enc_key, sizeof(enc_key));
    OPENSSL_cleanse(plaintext_padded.data(), plaintext_padded.size());

    return ciphertext;
}

string MessageEncryption::decryptAES(const vector<unsigned char>& ciphertext,
    const vector<unsigned char>& key,
    const vector<unsigned char>& iv) {
    if (key.size() != AES_KEY_SIZE) {
        throw runtime_error("密钥长度必须为 " + to_string(AES_KEY_SIZE) + " 字节");
    }
    if (iv.size() != IV_SIZE) {
        throw runtime_error("IV长度必须为 " + to_string(IV_SIZE) + " 字节");
    }
    if (ciphertext.size() % AES_BLOCK_LENGTH != 0) {
        throw runtime_error("密文长度必须是AES块大小的倍数");
    }

    AES_KEY dec_key;
    if (AES_set_decrypt_key(key.data(), key.size() * 8, &dec_key) != 0) {
        throw runtime_error("设置AES解密密钥失败");
    }

    vector<unsigned char> plaintext_padded(ciphertext.size());
    vector<unsigned char> iv_copy = iv;

    AES_cbc_encrypt(
        ciphertext.data(),
        plaintext_padded.data(),
        ciphertext.size(),
        &dec_key,
        iv_copy.data(),
        AES_DECRYPT
    );

    if (plaintext_padded.empty()) {
        throw runtime_error("解密后数据为空");
    }

    unsigned char pad_value = plaintext_padded.back();
    if (pad_value == 0 || pad_value > AES_BLOCK_LENGTH) {
        throw runtime_error("无效的PKCS7填充");
    }

    for (size_t i = plaintext_padded.size() - pad_value; i < plaintext_padded.size(); ++i) {
        if (plaintext_padded[i] != pad_value) {
            throw runtime_error("PKCS7填充验证失败");
        }
    }

    size_t plaintext_len = plaintext_padded.size() - pad_value;
    string plaintext(plaintext_padded.begin(), plaintext_padded.begin() + plaintext_len);

    OPENSSL_cleanse(&dec_key, sizeof(dec_key));
    OPENSSL_cleanse(plaintext_padded.data(), plaintext_padded.size());

    return plaintext;
}

void MessageEncryption::generateAESKey() {
    initialize();
}

void MessageEncryption::setAESKey(const vector<unsigned char>& key) {
    if (key.size() != AES_KEY_SIZE) {
        throw runtime_error("密钥长度必须为 " + to_string(AES_KEY_SIZE) + " 字节");
    }
    aesKey = key;
    initialized = true;
}

const vector<unsigned char>& MessageEncryption::getAESKey() const {
    if (!initialized) {
        throw runtime_error("MessageEncryption 未初始化");
    }
    return aesKey;
}

const vector<unsigned char>& MessageEncryption::getIV() const {
    if (!initialized) {
        throw runtime_error("MessageEncryption 未初始化");
    }
    return iv;
}