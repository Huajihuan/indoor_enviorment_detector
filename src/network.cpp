#include <network.h>
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

IPAddress local_ip(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);
WebServer server(80); // 端口

String wifi_ssid = ""; // 暂时存储wifi账号密码
String wifi_pass = ""; // 暂时存储wifi账号密码

const int LED = 2; // 设置LED引脚

const char *ntpServer = "ntp1.aliyun.com";
const long gmtOffset_sec = 8 * 3600; // 北京时间是UTC+8
const int daylightOffset_sec = 0;    // 中国不用夏令时

#define ROOT_HTML "<!DOCTYPE html><html><head><title>WIFI</title><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head><style type=\"text/css\">.input{display: block; margin-top: 10px;}.input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}.input input{height: 30px;width: 200px;}.btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}</style><body><form method=\"POST\" action=\"configwifi\"><label class=\"input\"><span>WiFi SSID</span><input type=\"text\" name=\"ssid\" value=\"\"></label><label class=\"input\"><span>WiFi PASS</span> <input type=\"text\"  name=\"pass\"></label><input class=\"btn\" type=\"submit\" name=\"submit\" value=\"Submit\"> <p><span> Nearby wifi:</P></form>"
//html存放在代码中时 " 前要加上转义字符 \ 
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

String set_AP(const char *ap_ssid, const char *ap_password) // 设置esp32为AP模式
{
    WiFi.mode(WIFI_AP);                           // 打开WiFi模式
    WiFi.softAPConfig(local_ip, gateway, subnet); // 配置

    while (!WiFi.softAP(ap_ssid, ap_password)) // 启动AP，若没启动成功则重复尝试
    {
        continue;
    }

    Serial.println("AP启动成功");
    Serial.print("热点名称:");
    Serial.println(ap_ssid);
    Serial.print("IP:");
    Serial.println(WiFi.softAPIP());
    Serial.print("MAC:");
    Serial.println(WiFi.softAPmacAddress());

    String ip_address = WiFi.softAPIP().toString();
    return ip_address;
}


String wifi_scan() // 显示附近的WiFi
{
    String scanNetworksID;
    Serial.println("正在搜索网络--------->");
    int n = WiFi.scanNetworks();
    Serial.println("搜索结束");

    if (n == 0)
    {
        Serial.println("没有找到网络");
    }
    else
    {
        Serial.print("找到了");
        Serial.print(n);
        Serial.println("个网络");
        for (int i = 0; i < n; i++)
        {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i)); // 信号强度
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*"); // 三目运算符

            scanNetworksID = scanNetworksID + "<P>" + WiFi.SSID(i) + "</P>"; // 字符串，用于html输出
        }
    }
    return scanNetworksID;
}


void connect_to_wifi(String wifi_ssid, String wifi_pass, unsigned long time_interval) // 连接到wifi
{
    WiFi.mode(WIFI_STA);       // 设置为STA模式并连接WIFI
    WiFi.setAutoConnect(true); // 设置自动连接
    unsigned long time_begin = millis();
    WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str()); // c_str(),
    delay(1000);                                      // 等两秒钟

    while (WiFi.status() != WL_CONNECTED) // 如果在规定时间内没连接成功则返回错误
    {
        int time_now = millis();
        if (time_now <= time_begin + time_interval)
        {
            WiFi.begin(wifi_ssid.c_str(), wifi_pass.c_str()); // c_str(),获取该字符串的指针
            delay(1000);
        }
        else
        {
            Serial.println("连接超时..........");
            return;
        }
    }
    Serial.println("已连接");
}


void handle_root() // 处理访问请求,发送配置页面
{
    String scan_result = wifi_scan();
    server.send(200, "text/html", ROOT_HTML + scan_result + "</body></html>");
}


void handle_config_wifi() // 处理配置WiFi请求
{
    if (server.hasArg("ssid") && server.hasArg("pass")) // 判断是否有账号参数
    {
        Serial.print("got ssid:");
        wifi_ssid = server.arg("ssid"); // 获取html表单输入框name名为"ssid"的内容
        wifi_pass = server.arg("pass");
        Serial.println(wifi_ssid);
    }
    else // 还没有账号参数，返回错误信息继续填
    {
        Serial.println("user input error, please check ssid and password");
        server.send(200, "text/html", "<meta charset='UTF-8'>输入错误,请检查账号密码是否错误");
        return;
    }

    // 用户输入成功,开始连接WiFi
    server.send(200, "text/html", "<meta charset='UTF-8'>SSID: " + wifi_ssid + "<br />password:" + wifi_pass + "<br />已取得WiFi信息,正在尝试连接,请手动关闭此页面。"); // 返回保存成功页面
    // delay(2000);
    // WiFi.softAPdisconnect(true); // 参数设置为true，设备将直接关闭接入点模式，即关闭设备所建立的WiFi网络。
    // server.close();              // 关闭web服务
    // WiFi.softAPdisconnect();     // 在不输入参数的情况下调用该函数,将关闭接入点模式,并将当前配置的AP热点网络名和密码设置为空值.
    Serial.println("WiFi Connect SSID:" + wifi_ssid + "  PASS: " + wifi_pass);
    connect_to_wifi(wifi_ssid, wifi_pass, 30000);
}


void handleNotFound() // 当浏览器请求的网络资源无法在服务器找到时通过此自定义函数处理
{
    server.send(404, "text/plain", "<h1>404: Not found</h1>");
    delay(2000);
    handle_root(); // 访问不存在目录则返回配置页面
}


void initWebServer() // 初始化服务器
{
    // 必须添加第二个参数HTTP_GET，以下面这种格式去写，否则无法强制门户
    server.on("/", HTTP_GET, handle_root);                   //  当浏览器请求服务器根目录(网站首页)时调用自定义函数handleRoot处理，设置主页回调函数，必须添加第二个参数HTTP_GET，否则无法强制门户
    server.on("/configwifi", HTTP_POST, handle_config_wifi); //  当浏览器请求服务器/configwifi(表单字段)目录时调用自定义函数handleConfigWifi处理

    server.onNotFound(handleNotFound); // 当浏览器请求的网络资源无法在服务器找到时调用自定义函数handleNotFound处理

    server.begin(); // 启动TCP SERVER
    server.handleClient();
    Serial.println("WebServer 已开启!");
}


void server_on() // 开启http服务器
{
    server.handleClient(); // 检查客户端(浏览器)http请求
}


void reset_network() // 清除网络配置
{
    delay(500);
    esp_wifi_restore(); // 删除保存的wifi信息
    Serial.println("连接信息已清空,准备重启设备..");
    delay(10);
}


void check_network() // 检查网络状态,断网了就尝试重连，led变闪烁
{
}


struct tm get_time_for_internet() // 获取网络时间
{
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    struct tm time_info;
    if (!getLocalTime(&time_info))
    {
        Serial.println("Failed to obtain time");
    }
    else
    {
        Serial.println(&time_info, "%A, %B %d %Y %H:%M:%S");
    }

    return time_info;
}


