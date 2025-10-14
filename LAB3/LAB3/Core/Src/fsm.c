
#include "fsm.h"


int green_temp_counter;
int yellow_temp_counter;


void update_led_counter(){
	red_counter = red_duration;
	yellow_counter = yellow_duration;
	green_counter = green_duration;
}

void red_duration_process(){
	red_duration = red_counter;
	green_temp_counter = red_duration - yellow_duration;
	if (green_temp_counter > green_duration)
		green_duration = green_temp_counter;
	else if (green_temp_counter < green_duration){
		yellow_temp_counter = red_duration - green_duration;
		if (yellow_temp_counter > yellow_duration)
			yellow_duration = yellow_temp_counter;
		else red_duration = green_duration + yellow_duration;
	}
}

void yellow_duration_process(){
	yellow_duration = yellow_counter;
	green_temp_counter = red_duration - yellow_duration;
	if (green_temp_counter > green_duration)
		green_duration = green_temp_counter;
	else if (green_temp_counter < green_duration)
		red_duration = yellow_duration + green_duration;
}

void green_duration_process(){
	green_duration = green_counter;
	yellow_temp_counter = red_duration - green_duration;
	if (yellow_temp_counter > yellow_duration)
		yellow_duration = yellow_temp_counter;
	else if (yellow_temp_counter < yellow_duration)
		red_duration = yellow_duration + green_duration;
}

void auto_run(){
	switch(status){
		case INIT_AUTO:
			led_clear();
			timer4_set(green_duration * 1000);
			status = RED_GREEN;
			break;
		case RED_GREEN:
//			test_led_traffic(1);
			led_control(LEFT, RED_LED);
			led_control(RIGHT, GREEN_LED);
//			show_counter(); // not done
			if (timer4_flag == 1){
				timer4_set(yellow_duration * 1000);
				status = RED_YELLOW;
			}
			if (button_count[0] == 1){
				timer2_set(2000);
				status = DUMB;
				mode = MANUAL_RED;
			}
			break;
		case RED_YELLOW:
//			test_led_traffic(2);
			led_control(LEFT, RED_LED);
			led_control(RIGHT, YELLOW_LED);
//			show_counter(); // not done
			if (timer4_flag == 1){
				timer4_set(green_duration * 1000);
				status = GREEN_RED;
			}
			if (button_count[0] == 1){
				timer2_set(2000);
				status = DUMB;
				mode = MANUAL_RED;
			}
			break;
		case YELLOW_RED:
//			test_led_traffic(3);
			led_control(LEFT, YELLOW_LED);
			led_control(RIGHT, RED_LED);
			show_counter(); // not done
			if (timer4_flag == 1){
				timer4_set(green_duration * 1000);
				status = RED_GREEN;
			}
			if (button_count[0] == 1){
				timer2_set(2000);
				status = DUMB;
				mode = MANUAL_RED;
			}
			break;
		case GREEN_RED:
			led_control(LEFT, GREEN_LED);
			led_control(RIGHT, RED_LED);
//			show_counter(); // not done
			if (timer4_flag == 1){
				timer4_set(yellow_duration * 1000);
				status = YELLOW_RED;
			}
			if (button_count[0] == 1){
				timer2_set(2000);
				status = DUMB;
				mode = MANUAL_RED;
			}
			break;
		case DUMB:
			break;
	}
}

void fsm_run(){
	switch(mode){
		case INIT_FSM:
			status = INIT_AUTO;
			mode = AUTO;
			show_freq(0);
			break;
		case MANUAL_RED:
			led_blinking(RED_LED);
			show_freq(red_counter);
			if (timer2_flag == 1) {
				status = INIT_AUTO;
				mode = AUTO;
			}
			if (button_count[0] == 1){
				mode = MANUAL_YELLOW;
				timer2_set(10000);
			}
			if (button_count[1] == 1){
				red_counter++;
				if (red_counter > 99) {
					lcd_show_string(10, 220, "Exceed limit.", BLUE, WHITE, 16, 1);
					red_counter = 5;
					timer4_set(2000);
				}
				timer2_set(10000);
			}
			if (timer4_flag == 1){
				lcd_show_string(10, 220, "Exceed limit.", WHITE, WHITE, 16, 0);
				timer4_flag = 0;
			}
			if (button_count[2] == 1){
				red_duration_process();
				update_led_counter();
				status = INIT_AUTO;
				mode = AUTO;
			}
			break;
		case MANUAL_GREEN:
			led_blinking(GREEN_LED);
			show_freq(green_counter);
			if (timer2_flag == 1) {
				status = INIT_AUTO;
				mode = AUTO;
			}
			if (button_count[0] == 1){
				mode = MANUAL_RED;
				timer2_set(10000);
			}
			if (button_count[1] == 1){
				green_counter++;
				if (green_counter > 99) {
					lcd_show_string(10, 220, "Exceed limit.", BLUE, WHITE, 16, 1);
					green_counter = 2;
					timer4_set(2000);
				}
				timer2_set(10000);
			}
			if (timer4_flag == 1){
				lcd_show_string(10, 220, "Exceed limit.", WHITE, WHITE, 16, 0);
				timer4_flag = 0;
			}
			if (button_count[2] == 1){
				green_duration_process();
				update_led_counter();
				status = INIT_AUTO;
				mode = AUTO;
			}
			break;
		case MANUAL_YELLOW:
			led_blinking(YELLOW_LED);
			show_freq(yellow_counter);
			if (timer2_flag == 1) {
				status = INIT_AUTO;
				mode = AUTO;
			}
			if (button_count[0] == 1){
				mode = MANUAL_GREEN;
				timer2_set(10000);
			}
			if (button_count[1] == 1){
				yellow_counter++;
				if (yellow_counter > 20) {
					lcd_show_string(10, 220, "Exceed limit.", BLUE, WHITE, 16, 1);
					yellow_counter = 3;
					timer4_set(2000);
				}
				timer2_set(10000);
			}
			if (timer4_flag == 1){
				lcd_show_string(10, 220, "Exceed limit.", WHITE, WHITE, 16, 0);
				timer4_flag = 0;
			}
			if (button_count[2] == 1){
				yellow_duration_process();
				update_led_counter();
				status = INIT_AUTO;
				mode = AUTO;
			}
			break;
		case AUTO:
			show_freq(0);
			break;
		default:
			break;
	}
}
