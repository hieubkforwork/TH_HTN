/*
 * global.h
 */

#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "main.h"

// color
#define CLEAR           0
#define RED_LED			1
#define YELLOW_LED		2
#define GREEN_LED		3

// led-direct
#define LEFT            5
#define RIGHT           6

// status of auto
#define INIT_AUTO       10
#define RED_GREEN       11
#define RED_YELLOW      12
#define GREEN_RED       13
#define YELLOW_RED      14
#define DUMB            15

// mode
#define INIT_FSM		20
#define AUTO            21
#define MANUAL_RED      22
#define MANUAL_YELLOW   23
#define MANUAL_GREEN    24



extern int status;
extern int mode;
extern int blink_state;

extern int red_duration;
extern int yellow_duration;
extern int green_duration;
extern int red_counter;
extern int yellow_counter;
extern int green_counter;


void init_traffic();

#endif /* INC_GLOBAL_H_ */
