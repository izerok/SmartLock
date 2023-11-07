/*
 * wifi_info.h
 *
 *  Created on: 2020-05-15
 *      Author: Mixiaoxiao (Wang Bin)
 */

#ifndef WIFI_INFO_H_
#define WIFI_INFO_H_


#include <WiFiManager.h>  
WiFiManager wifiManager;

const char *ssid = "SmartLock-AP";

void wifi_connect() {
  wifiManager.autoConnect(ssid);
}

#endif
