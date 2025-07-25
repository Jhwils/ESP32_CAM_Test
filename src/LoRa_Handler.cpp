#include "LoRa_Handler.h"
#include "LoRa_Config.h"

// 初始化LoRa通信
void lora_comm_init()
{
    // 配置LoRa串口
    LORA_SERIAL.begin(LORA_BAUDRATE, SERIAL_8N1, LORA_RX_PIN, LORA_TX_PIN);
    Serial.println("[LoRa] Serial communication initialized");
    Serial.printf("  RX Pin: %d, TX Pin: %d, Baudrate: %d\n",
                  LORA_RX_PIN, LORA_TX_PIN, LORA_BAUDRATE);
}

// 处理接收到的LoRa数据
void process_lora_data()
{
    Serial.println("[LoRa] Processing received data...");

    // 检查是否有数据可用
    if (!lora_has_data())
    {
        Serial.println("[LoRa] No data available in buffer");
        return;
    }

    // 添加数据验证，过滤配置回传数据
    uint8_t firstByte = LORA_SERIAL.peek();
    if (firstByte == 0xC0 || firstByte == 0xE0)
    {
        Serial.println("[LoRa] Detected configuration response, clearing buffer");
        while (LORA_SERIAL.available() > 0)
        {
            LORA_SERIAL.read();
        }
        return;
    }

    // 读取所有可用数据
    Serial.println("Received LoRa data:");
    Serial.println("-------------------");

    int available = LORA_SERIAL.available();
    uint8_t buffer[256];
    size_t bytesRead = 0;

    // 读取数据，但不超过缓冲区大小
    while (LORA_SERIAL.available() > 0 && bytesRead < sizeof(buffer))
    {
        buffer[bytesRead] = LORA_SERIAL.read();
        bytesRead++;
    }

    // 打印十六进制格式
    Serial.println("Hex format:");
    for (size_t i = 0; i < bytesRead; i++)
    {
        if (buffer[i] < 0x10)
            Serial.print("0");
        Serial.print(buffer[i], HEX);
        Serial.print(" ");
        if ((i + 1) % 16 == 0)
            Serial.println();
    }
    Serial.println();

    // 打印ASCII格式
    Serial.println("ASCII format:");
    for (size_t i = 0; i < bytesRead; i++)
    {
        if (buffer[i] >= 32 && buffer[i] <= 126)
        {
            Serial.write(buffer[i]);
        }
        else
        {
            Serial.print(".");
        }
        if ((i + 1) % 16 == 0)
            Serial.println();
    }
    Serial.println();

    Serial.println("-------------------");
    Serial.printf("[LoRa] Processed %d bytes of data\n", bytesRead);
}

// 发送LoRa数据
void lora_send_data(const uint8_t *data, size_t length)
{
    if (length == 0)
        return;

    Serial.printf("[LoRa] Sending %d bytes of data\n", length);

    // 发送数据
    LORA_SERIAL.write(data, length);

    // 确保数据发送完成
    LORA_SERIAL.flush();

    Serial.println("[LoRa] Data sent successfully");
}

// 检查是否有可用的LoRa数据
bool lora_has_data()
{
    return LORA_SERIAL.available() > 0;
}

// 检查LoRa数据可用性（为lora_has_data的别名）
bool lora_data_available()
{
    return lora_has_data();
}

// 发送字符串数据的便捷函数
// 函数：lora_send_string
// 功能：发送字符串
// 参数：const String &message，要发送的字符串
void lora_send_string(const String &message)
{
    // 如果字符串长度为0，则直接返回
    if (message.length() == 0)
        return;

    // 打印发送的字符串
    Serial.printf("[LoRa] Sending string: \"%s\"\n", message.c_str());
    // 调用lora_send_data函数发送数据
    lora_send_data((const uint8_t *)message.c_str(), message.length());
}

// 处理Serial0输入并通过LoRa发送
void handle_serial_input()
{
    // 定义一个静态字符串变量，用于存储串口输入
    static String inputBuffer = "";

    // 检查是否有串口输入
    while (Serial.available() > 0)
    {
        // 读取串口输入的字符
        char inChar = (char)Serial.read();

        // 如果收到回车键，处理输入
        if (inChar == '\n' || inChar == '\r')
        {
            // 如果缓冲区中有内容，处理命令
            if (inputBuffer.length() > 0)
            {
                // 打印输入的命令
                Serial.println("[Serial Input] Processing command: " + inputBuffer);

                // 检查是否是特殊命令
                if (inputBuffer.startsWith("LORA:"))
                {
                    // 提取要发送的消息（去掉"LORA:"前缀）
                    String message = inputBuffer.substring(5);
                    message.trim(); // 去掉首尾空白字符

                    // 如果消息不为空，发送消息
                    if (message.length() > 0)
                    {
                        // 切换到正常发送模式
                        Serial.println("[LoRa] Switching to WOR mode for transmission");
                        setLoRaMode(MODE_WOR);
                        delay(100); // 等待模式切换完成

                        // 发送数据
                        lora_send_string(message);

                        // 发送完成后切换回WOR模式
                        delay(100); // 等待发送完成
                        Serial.println("[LoRa] Switching back to WOR mode");
                        setLoRaMode(MODE_WOR);

                        Serial.println("[LoRa] Message sent successfully!");
                    }
                    else
                    {
                        Serial.println("[Error] Empty message after LORA: command");
                    }
                }
                // 如果输入的是HELP命令，显示帮助信息
                else if (inputBuffer == "HELP" || inputBuffer == "help")
                {
                    // 显示帮助信息
                    Serial.println("\n=== LoRa Test Commands ===");
                    Serial.println("LORA:<message>  - Send message via LoRa");
                    Serial.println("HELP           - Show this help");
                    Serial.println("Example: LORA:Hello World");
                    Serial.println("========================\n");
                }
                // 如果输入的是其他命令，显示未知命令信息
                else
                {
                    Serial.println("[Info] Unknown command. Type 'HELP' for available commands.");
                }

                // 清空缓冲区
                inputBuffer = "";
            }
        }
        // 如果输入的不是回车符，将字符添加到缓冲区
        else if (inChar != '\r') // 忽略回车符，只处理换行符
        {
            // 将字符添加到缓冲区
            inputBuffer += inChar;

            // 防止缓冲区溢出
            if (inputBuffer.length() > 200)
            {
                Serial.println("[Warning] Input buffer overflow, clearing...");
                inputBuffer = "";
            }
        }
    }
}
