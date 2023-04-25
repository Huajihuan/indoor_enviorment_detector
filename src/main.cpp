#include <Arduino.h>
#include <network.h>
#include <display.h>

const int resetPin = 0; // 设置重置按键引脚,用于删除WiFi信息
const char *ap_ssid = "ESP-AP";
const char *ap_password = "88888888";
struct tm local_time;

void setup()
{
  Serial.begin(9600); // 波特率
  while (!Serial)
  {
  } // 检查是否正常启动，若是没正常启动就重新尝试
  Serial.println();
  unsigned long time_begin = millis();
  unsigned long time_now = millis();
  while (!WiFi.isConnected())
  {
    WiFi.mode(WIFI_STA);
    WiFi.begin(); // 重启之后用之前的参数连接WiFi
    Serial.println("正在连接网络...");
    delay(3000);
    if (WiFi.isConnected())
    {
      Serial.println("连接成功");
      break;
    }
    time_now = millis();
    if (time_now >= time_begin + 8 * 1000)
    {
      break;
    }
  }

  if (!WiFi.isConnected()) // 之前没有连过wifi或者已经重置
  {
    Serial.println("开始配网...");
    String ap_ip;
    ap_ip = set_AP(ap_ssid, ap_password); // 开启热点，并且返回ip地址
    initWebServer();                      // 开启http服务器
    lcd_init();
  }
}

void loop()
{
  server_on(); // 等待用户端响应
  local_time = get_time_for_internet();
  display_time(local_time);

  //检测按键是否被按下
  if (!digitalRead(resetPin)) // 长按5秒(P0)清除网络配置信息
  {
    delay(5000); // 哈哈哈哈，这样不准确
    if (!digitalRead(resetPin))
    {
      Serial.println("\n按键已长按5秒,正在清空网络连保存接信息.");
      reset_network();               // 删除保存的wifi信息
      ESP.restart();                 // 重启复位esp32
      Serial.println("已重启设备."); // 有机会读到这里吗？
    }
  }
}
