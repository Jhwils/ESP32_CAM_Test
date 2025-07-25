// WebServer.cpp
#include "Web_LoRa_server.h"
#include "LoRa_Handler.h"

WebServer server(80);

void webServer_init()
{
    server.on("/api/command", HTTP_POST, handleCommandPost);
    server.begin();
    Serial.println("[HTTP] Web server started");
}

/* 发送唤醒帧到 LoRa */
void lora_send_wakeup(uint16_t targetAddr)
{
    uint8_t frame[4] = {0xAA, 0x55, (uint8_t)(targetAddr >> 8), (uint8_t)targetAddr};
    lora_send_data(frame, sizeof(frame));
}

/* 解析 JSON 并执行指令 */
void handleCommandPost()
{
    if (!server.hasArg("plain"))
    {
        server.send(400, "application/json", "{\"error\":\"no body\"}");
        return;
    }

    String body = server.arg("plain");
    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, body);

    if (err)
    {
        server.send(400, "application/json", "{\"error\":\"json invalid\"}");
        return;
    }

    const char *action = doc["action"] | "";
    uint16_t target = doc["target"] | 0;

    if (strcmp(action, "wakeup") == 0)
    {
        Serial.printf("[HTTP] 收到唤醒指令，目标地址=0x%04X\n", target);
        lora_send_wakeup(target);
        server.send(200, "application/json", "{\"result\":\"wakeup sent\"}");
    }
    else
    {
        server.send(400, "application/json", "{\"error\":\"unknown action\"}");
    }
}