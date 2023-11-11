// wifi_info.h 的内容
#include <WiFiManager.h>  // WiFi管理库
#include <homekit/homekit.h>


WiFiManager wifiManager;
const char *ssid = "SmartLock-AP";  // 定义SSID

void wifi_connect() {
    wifiManager.autoConnect(ssid);  // 自动连接WiFi
}


// SmartLock.ino 的主要代码
#include <Arduino.h>
#include <arduino_homekit_server.h>
#include <Servo.h>

// 步进电机配置
const int ledPin = LED_BUILTIN;  // 内置LED灯引脚
#define MOTOR_PIN D6              // 步进电机控制引脚
#define LOCKED_ANGLE 0            // 锁定角度
#define UNLOCKED_ANGLE 90         // 解锁角度
unsigned long lockTimer = 0; // 用于非阻塞延时的计时器
bool lockTimerActive = false; // 计时器是否激活的标志
#define OPEN_LOCK_DELAY 500      // 开锁后自动复位的延迟时间（毫秒）


Servo action;

// 函数原型声明
void open_lock();

void reset_lock();

void my_homekit_setup();

void my_homekit_loop();

void set_lock(const homekit_value_t value);

void report_lock_state(uint8_t state);



void setup() {
    wifi_connect();              // 连接WiFi
    my_homekit_setup();          // 设置HomeKit
    action.attach(MOTOR_PIN);    // 绑定步进电机引脚
    action.write(LOCKED_ANGLE);  // 初始设为锁定状态
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);   // 关闭LED灯
}

void loop() {
    my_homekit_loop();           // HomeKit循环处理

    // 检查计时器是否激活且是否已经过去了预定的时间
    if (lockTimerActive && millis() - lockTimer > OPEN_LOCK_DELAY) {
        reset_lock();              // 执行复位锁定操作
        lockTimerActive = false;   // 关闭计时器
    }
}


//==============================
// HomeKit setup and loop
//==============================

// access lock-mechanism HomeKit characteristics defined in my_accessory.c
extern "C" homekit_server_config_t config;

extern "C" homekit_characteristic_t cha_lock_current_state;
extern "C" homekit_characteristic_t cha_lock_target_state;

void my_homekit_setup() {
    cha_lock_target_state.setter = set_lock;
    arduino_homekit_setup(&config);  // HomeKit初始化配置
}

void my_homekit_loop() {
    arduino_homekit_loop();  // HomeKit循环处理
}

void set_lock(const homekit_value_t value) {
    uint8_t state = value.int_value;
    if (state == 0) {
        open_lock();  // 执行开锁操作
    }
}

void open_lock() {
    action.write(UNLOCKED_ANGLE);  // 舵机转动到解锁位置
    lockTimer = millis();          // 记录当前时间
    lockTimerActive = true;        // 激活计时器
    report_lock_state(0);  // 上报锁的状态
}

void reset_lock() {
    action.write(LOCKED_ANGLE);  // 舵机复位到锁定位置
    report_lock_state(1);        // 上报锁定状态
}

void report_lock_state(uint8_t state) {
    cha_lock_current_state.value.int_value = state;  // 更新锁的当前状态
    homekit_characteristic_notify(&cha_lock_current_state, cha_lock_current_state.value);  // 通知HomeKit状态变化
}