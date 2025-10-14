/*
 * traffic.h
 */

#ifndef INC_TRAFFIC_H_
#define INC_TRAFFIC_H_

#include "main.h"
#include "global.h"

#include "software_timer.h"
#include "lcd.h"
#include "button.h"

void test_led_traffic(int idx);

void led_clear();
void led_control(int ledth, int color);
void led_blinking(int color);

void show_mode();
void show_freq(int period);
void show_counter();

#endif /* INC_TRAFFIC_H_ */
