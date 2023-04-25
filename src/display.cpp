#include <display.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void lcd_init()
{
    lcd.init();
    lcd.backlight();
}

void lcd_print(const char *text, uint8_t column, uint8_t row)
{
    lcd.setCursor(column, row);
    lcd.print(text);
}

void lcd_print(int text, uint8_t column, uint8_t row)
{
    lcd.setCursor(column, row);
    lcd.print(text);
}

void lcd_print(float text, uint8_t column, uint8_t row)
{
    lcd.setCursor(column, row);
    lcd.print(text);
}

void lcd_clear()
{
    lcd.clear();
}


void display_time(struct tm local_time) // 显示时间
{
    /*在C语言中，struct tm结构体中的年份是从1900年开始计算的，例如2022年应该表示为122（即2022 - 1900）。
    月份从0开始计算，因此4月应该是3而不是4。这个设计可能是为了保持向后兼容性，因为C语言中的日期和时间函数
    都是从早期版本开始就存在的。所以在使用strftime函数将struct tm结构体转换成字符串时，需要进行这些调整，
    否则得到的结果可能是错误的。*/
    lcd_print(local_time.tm_year + 1900, 0, 0);
    if(local_time.tm_mon + 1 >= 10)
    {
        lcd_print(local_time.tm_mon + 1, 5, 0);
    }
    else
    {
        lcd_print(local_time.tm_mon + 1, 5, 0);
        lcd_print(" ", 6, 0);
    }

    if(local_time.tm_mday >= 10)
    {
        lcd_print(local_time.tm_mday, 8, 0);
    }
    else
    {
        lcd_print(local_time.tm_mday, 8, 0);
        lcd_print(" ", 9, 0);
    }

    if(local_time.tm_hour >= 10)
    {
        lcd_print(local_time.tm_hour, 11, 0);
    }
    else
    {
        lcd_print(local_time.tm_hour, 11, 0);
        lcd_print(" ", 12, 0);
    }
    lcd_print(":", 13, 0);

    if(local_time.tm_min >= 10)
    {
        lcd_print(local_time.tm_min, 14, 0);
    }
    else
    {
        lcd_print(local_time.tm_min, 14, 0);
        lcd_print(" ", 15, 0);
    }

    if(local_time.tm_sec >= 10)
    {
        lcd_print(local_time.tm_sec, 0, 1);
    }
    else
    {
        lcd_print(local_time.tm_sec, 0, 1);
        lcd_print(" ", 1, 1);
    }
}


void auto_brightness()
{

}

