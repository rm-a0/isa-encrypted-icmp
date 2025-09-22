#include "encoder.hpp"
#include <iostream>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>

#define KEY_SIZE 32
#define IV_SIZE 16

/* source: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption adapted to fit C++ standard*/
bool encoder::encrypt(std::vector<uint8_t>& data, std::vector<uint8_t>& key, 
                      std::vector<uint8_t>& iv, std::vector<uint8_t>& cipherData) {
    if (key.size() != KEY_SIZE || iv.size() != IV_SIZE) {
        std::cerr << "Incorrect key or IV size" << std::endl;
        return false;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "EVP_CIPHER_CTX_new failed" << std::endl;
        return false;
    }

    int len = 0;
    int cipherLen = 0;
    cipherData.clear();
    cipherData.resize(data.size() + EVP_MAX_BLOCK_LENGTH);

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        std::cerr << "EVP_EncryptInit_ex failed" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_EncryptUpdate(ctx, cipherData.data(), &len, data.data(), data.size()) != 1) {
        std::cerr << "EVP_EncryptUpdate failed" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    cipherLen = len;

    if (EVP_EncryptFinal_ex(ctx, cipherData.data() + len, &len) != 1) {
        std::cerr << "EVP_EncryptFinal_ex failed" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    cipherLen += len;
    cipherData.resize(cipherLen);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

/* source: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption adapted to fit C++ standard*/
bool encoder::decrypt(std::vector<uint8_t>& cipherData, std::vector<uint8_t>& key,
             std::vector<uint8_t>& iv, std::vector<uint8_t>& data) {
    if (key.size() != KEY_SIZE || iv.size() != IV_SIZE) {
        std::cerr << "Incorrect key or IV size" << std::endl;
        return false;
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        std::cerr << "EVP_CIPHER_CTX_new failed" << std::endl;
        return false;
    }

    int len = 0;
    int dataLen = 0;
    data.clear();
    data.resize(cipherData.size());

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data()) != 1) {
        std::cerr << "EVP_DecryptInit_ex failed" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    if (EVP_DecryptUpdate(ctx, data.data(), &len, cipherData.data(), cipherData.size()) != 1) {
        std::cerr << "EVP_DecryptUpdate failed" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    dataLen = len;

    if (EVP_DecryptFinal_ex(ctx, data.data() + len, &len) != 1) {
        std::cerr << "EVP_DecryptFinal_ex failed" << std::endl;
        EVP_CIPHER_CTX_free(ctx);
        return false; 
    }
    dataLen += len;
    data.resize(dataLen);

    EVP_CIPHER_CTX_free(ctx);
    return true; 
}

std::vector<uint8_t> encoder::deriveKey(std::string& login) {
    std::vector<uint8_t> key(KEY_SIZE);
    unsigned char hash[SHA256_DIGEST_LENGTH];

    if (!SHA256(reinterpret_cast<const unsigned char*>(login.data()), login.size(), hash)) {
        std::cerr << "SHA256 key derivation failed" << std::endl;
        return {};
    }

    std::copy(hash, hash + 32, key.begin());
    return key;
}

std::vector<uint8_t> encoder::generateIV(void) {
    std::vector<uint8_t> iv(IV_SIZE);
    if (RAND_bytes(iv.data(), static_cast<int>(iv.size())) != 1) {
        std::cerr << "Failed to generate random IV" << std::endl;
        return {};
    }

    return iv;
}