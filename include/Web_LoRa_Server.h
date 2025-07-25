// WebServer.h
#pragma once
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "LoRa_Handler.h"

extern WebServer server;

/* 启动 HTTP 服务并注册 URI */
void webServer_init();

/* 处理 /api/command 的 POST 请求 */
void handleCommandPost();