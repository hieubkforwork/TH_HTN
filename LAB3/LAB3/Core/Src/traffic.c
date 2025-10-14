

#include "traffic.h"


void off_state(int x, int y_start){
	lcd_draw_circle(x, y_start, LIGHTGRAY, 20, 1);
	lcd_draw_circle(x, y_start + 40, LIGHTGRAY, 20, 1);
	lcd_draw_circle(x, y_start + 80, LIGHTGRAY, 20, 1);
}
void red_state(int x, int y_start){
	lcd_draw_circle(x, y_start, RED, 20, 1);
	lcd_draw_circle(x, y_start + 40, LIGHTGRAY, 20, 1);
	lcd_draw_circle(x, y_start + 80, LIGHTGRAY, 20, 1);
}
void yellow_state(int x, int y_start){
	lcd_draw_circle(x, y_start, LIGHTGRAY, 20, 1);
	lcd_draw_circle(x, y_start + 40, YELLOW, 20, 1);
	lcd_draw_circle(x, y_start + 80, LIGHTGRAY, 20, 1);
}
void green_state(int x, int y_start){
	lcd_draw_circle(x, y_start, LIGHTGRAY, 20, 1);
	lcd_draw_circle(x, y_start + 40, LIGHTGRAY, 20, 1);
	lcd_draw_circle(x, y_start + 80, GREEN, 20, 1);
}
void test_led_traffic(int idx){
	if (idx == 1) lcd_draw_circle(180, 90, BROWN, 20, 1);
	else if (idx == 2) lcd_draw_circle(180, 90, RED, 20, 1);
	else if (idx == 3) lcd_draw_circle(180, 170, BLACK, 20, 1);
}
void led_control(int ledth, int color){
	int num = 0;
	if (ledth == RIGHT) num = 60;
	if (color == CLEAR) off_state(80 + num, 70);
	else if (color == RED_LED) red_state(80 + num, 70);
	else if (color == YELLOW_LED) yellow_state(80 + num, 70);
	else if (color == GREEN_LED) green_state(80 + num, 70);
}
void led_clear(){
	led_control(LEFT, CLEAR);
	led_control(RIGHT, CLEAR);
}
void led_blinking(int color){
	if (blink_state == 0){
		led_clear();
		if (timer3_flag == 1){
			led_clear();
			blink_state = 1;
			timer3_set(500);
		}
	}
	else if (blink_state == 1){
		led_control(LEFT, color);
		led_control(RIGHT, color);
		if (timer3_flag == 1){
			led_clear();
			blink_state = 0;
			timer3_set(500);
		}
	}
}


void show_mode(){
	lcd_show_string(10, 10, "Mode: ", RED, BLUE, 16, 1);
	switch(mode){
		case INIT_FSM:
			lcd_show_string(60, 10, "INIT", BRED, WHITE, 16, 0);
			break;
		case AUTO:
			lcd_show_string(60, 10, "AUTO-TRAFFIC ", BRED, WHITE, 16, 0);
			break;
		case MANUAL_RED:
			lcd_show_string(60, 10, "MODIFY-RED   ", BRED, WHITE, 16, 0);
			break;
		case MANUAL_YELLOW:
			lcd_show_string(60, 10, "MODIFY-YELLOW", BRED, WHITE, 16, 0);
			break;
		case MANUAL_GREEN:
			lcd_show_string(60, 10, "MODIFY-GREEN ", BRED, WHITE, 16, 0);
			break;
		default:
			break;
	}
}
void show_freq(int period){
	if (mode == AUTO || mode == INIT_FSM){
		lcd_show_string(10, 200, "Period: 99", WHITE, WHITE, 16, 0);
	}
	else {
		lcd_show_string(10, 200, "Period: ", BLUE, WHITE, 16, 1);
		lcd_show_int_num(70, 200, period, 2, BRED, WHITE, 16);
	}
}
void show_counter(){
	
}
