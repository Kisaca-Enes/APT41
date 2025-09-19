#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>

int main(void) {
    const unsigned char *key = (unsigned char *)"secret-key-98988";
    const unsigned char *data = (unsigned char *)"[COMMAND=PING][USERID=1234]";
    unsigned char *result;
    unsigned int result_len = 0;

    // Compute HMAC-SHA256
    result = HMAC(EVP_sha256(), key, strlen((const char*)key), data, strlen((const char*)data), NULL, &result_len);
    if (!result) {
        fprintf(stderr, "HMAC failed\n");
        return 1;
    }

    // Print HMAC hex
    printf("HMAC-SHA256: ");
    for (unsigned int i = 0; i < result_len; ++i)
        printf("%02x", result[i]);
    printf("\n");

    // Verification example
    unsigned char verify[EVP_MAX_MD_SIZE];
    unsigned int verify_len = 0;
    HMAC(EVP_sha256(), key, strlen((const char*)key), data, strlen((const char*)data), verify, &verify_len);

    if (verify_len != result_len || memcmp(result, verify, result_len) != 0) {
        printf("Verification failed\n");
    } else {
        printf("Verification OK\n");
    }

    return 0;
}
