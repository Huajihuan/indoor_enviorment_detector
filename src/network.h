#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>  //用于设备域名 MDNS.begin("esp32")
#include <esp_wifi.h> //用于重置网络
#include <time.h>

//网络部分
String set_AP(const char *ap_ssid, const char *ap_password);  //设置esp32为AP模式
String wifi_scan();  //显示附近的WiFi
void connect_to_wifi(String wifi_ssid, String wifi_pass, unsigned long time_interval);  //连接到wifi
void initWebServer();    //初始化服务器
void server_on();
void reset_network();
void check_newwork();    //检查网络状态,断网了就尝试重连，led变闪烁

//时间部分
struct tm get_time_for_internet();   //使用NTP服务获取网络时间
