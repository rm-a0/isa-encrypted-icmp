#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <vector>
#include <string>

/**
 * @namespace encoder
 * @brief Namespace for encoding and decoding data
 */
namespace encoder {
    /**
     * @brief Encrypts data using key and stores cipher inside cipherData
     * @param data Bytes to be encrypted
     * @param key AES key
     * @param iv Random bytes
     * @param cipherData Encrypted data
     * @return True if no issues, False if error occurred
     */
    bool encrypt(std::vector<uint8_t>& data, 
                 std::vector<uint8_t>& key, 
                 std::vector<uint8_t>& iv, 
                 std::vector<uint8_t>& cipherData);

    /**
     * @brief Decrypts data using key and stores encrypted data inside data
     * @param cipherData Encrypted data
     * @param key AES key
     * @param iv Random bytes
     * @param data Decrypted data
     * @return True if no issues, False if error occurred
     */
    bool decrypt(std::vector<uint8_t>& cipherData,
                 std::vector<uint8_t>& key,
                 std::vector<uint8_t>& iv,
                 std::vector<uint8_t>& data);
    
    /**
     * @brief Derives 256-bit key from login string
     * @param login login string
     * @return Derived key stored in vector
     */
    std::vector<uint8_t> deriveKey(const std::string& login);

    /**
     * @brief Generates random 128-bit IV
     * @return Generated IV
     */
    std::vector<uint8_t> generateIV(void);
}

#endif // ENCODER_HPP