#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <time.h>

void lcd_init();
void lcd_print(const char* text, uint8_t column, uint8_t row);
void lcd_print(int text, uint8_t column, uint8_t row);
void lcd_print(float text, uint8_t column, uint8_t row);
void lcd_clear();
void display_time(struct tm local_time); // 显示时间