#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>
#include <stdlib.h>

// Simule edilen "ViewState payload" yapısı
typedef struct {
    char command[32];
    char user[32];
} ViewStatePayload;

// Basit serialize: struct -> string
void serialize_payload(ViewStatePayload *payload, char *out, size_t out_size) {
    snprintf(out, out_size, "[COMMAND=%s][USER=%s]", payload->command, payload->user);
}

// Basit deserialize: string -> struct
void deserialize_payload(const char *in, ViewStatePayload *payload) {
    sscanf(in, "[COMMAND=%31[^]]][USER=%31[^]]]", payload->command, payload->user);
}

int main() {
    // 1️⃣ Gizli MAC key (server side)
    const unsigned char *key = (unsigned char *)"secret-key-12345";

    // 2️⃣ Kullanıcıdan gelen ViewState payload
    ViewStatePayload payload;
    strcpy(payload.command, "PING");
    strcpy(payload.user, "1234");

    char serialized[128];
    serialize_payload(&payload, serialized, sizeof(serialized));

    printf("Serialized ViewState: %s\n", serialized);

    // 3️⃣ HMAC hesapla (MAC ile payload imzalanıyor)
    unsigned char *hmac_result;
    unsigned int hmac_len = 0;
    hmac_result = HMAC(EVP_sha256(), key, strlen((const char*)key),
                       (unsigned char*)serialized, strlen(serialized),
                       NULL, &hmac_len);

    printf("HMAC-SHA256: ");
    for (unsigned int i = 0; i < hmac_len; i++)
        printf("%02x", hmac_result[i]);
    printf("\n");

    // 4️⃣ Payload ve HMAC doğrulama (server side)
    unsigned char verify[EVP_MAX_MD_SIZE];
    unsigned int verify_len = 0;
    HMAC(EVP_sha256(), key, strlen((const char*)key),
         (unsigned char*)serialized, strlen(serialized),
         verify, &verify_len);

    if (verify_len != hmac_len || memcmp(hmac_result, verify, hmac_len) != 0) {
        printf("ViewState verification FAILED\n");
        return 1;
    } else {
        printf("ViewState verification OK\n");
    }

    // 5️⃣ Simule edilen "payload execution"
    ViewStatePayload executed;
    deserialize_payload(serialized, &executed);
    printf("Executing payload -> Command: %s, User: %s\n", executed.command, executed.user);

    return 0;
}
