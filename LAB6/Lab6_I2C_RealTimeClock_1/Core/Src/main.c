/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "lcd.h"
#include "ds3231.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INIT_SYSTEM     0
#define SET_HOUR        1
#define SET_MINUTE      2
#define SET_DAY         3
#define SET_DATE        4
#define SET_MONTH       5
#define SET_YEAR        6
#define SET_SECOND      7
#define WATCHING        8

#define SCHEDULE_HOUR       10
#define SCHEDULE_MINUTE     11
#define SCHEDULE_DAY        12
#define SCHEDULE_DATE       13
#define SCHEDULE_MONTH      14
#define SCHEDULE_YEAR       15
#define SCHEDULE_SECOND     16

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int statusSetupTime = INIT_SYSTEM;
int timeBlink = 0;
// SETTING MODE
int sec_tmp = 0;
int hour_tmp = 0;
int minute_tmp = 0;
int date_tmp = 0;
int day_tmp = 0;
int month_tmp = 0;
int year_tmp = 0;
// SCHEDULING MODE
int sec_timer = 0;
int hour_timer = 0;
int min_timer = 0;
int date_timer = 0;
int day_timer = 0;
int month_timer = 0;
int year_timer = 0;

int isSchedule = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void DisplayTime();
void UpdateTime();
void SetUpTime();
unsigned char IsButtonSettingMode();
unsigned char IsButtonSet();
unsigned char IsButtonScheduleMode();
unsigned char IsButtonUp();
unsigned char IsButtonDown();
// SETTING
void SetHour();
void SetMinute();
void SetSecond();
void SetDay();
void SetDate();
void SetMonth();
void SetYear();
// SCHEDULING
void scheduleHour();
void scheduleMinute();
void scheduleSecond();
void scheduleDay();
void scheduleDate();
void scheduleMonth();
void scheduleYear();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_SPI1_Init();
	MX_FSMC_Init();
	MX_I2C1_Init();
	MX_TIM4_Init();
	/* USER CODE BEGIN 2 */
	system_init();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	lcd_clear(BLACK);
	UpdateTime();

	while (1) {
		while (!timer2_flag);
		timer2_flag = 0;
        button_scan();
		DisplayTime();
		SetUpTime();
    /* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void system_init() {
	HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
	HAL_GPIO_WritePin(OUTPUT_Y1_GPIO_Port, OUTPUT_Y1_Pin, 0);
	HAL_GPIO_WritePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin, 0);

	lcd_init();
	ds3231_init();

	timer2_init();
	timer2_set(50);
}

void UpdateTime() {
	ds3231_write(ADDRESS_YEAR, 23);
	ds3231_write(ADDRESS_MONTH, 10);
	ds3231_write(ADDRESS_DATE, 20);
	ds3231_write(ADDRESS_DAY, 6);
	ds3231_write(ADDRESS_HOUR, 20);
	ds3231_write(ADDRESS_MIN, 11);
	ds3231_write(ADDRESS_SEC, 23);
}

void DisplayTime() {
	if (statusSetupTime == INIT_SYSTEM || statusSetupTime == WATCHING) ds3231_read_time();

	lcd_show_int_num(70, 100, ds3231_hours/10, 1, GREEN, BLACK, 24);
	lcd_show_int_num(83, 100, ds3231_hours%10, 1, GREEN, BLACK, 24);
    lcd_show_char(96, 100, ':', GREEN, BLACK, 24, 0);
    lcd_show_int_num(110, 100, ds3231_min/10, 1, GREEN, BLACK, 24);
    lcd_show_int_num(123, 100, ds3231_min%10, 1, GREEN, BLACK, 24);
    lcd_show_char(136, 100, ':', GREEN, BLACK, 24, 0);
    lcd_show_int_num(150, 100, ds3231_sec/10, 1, GREEN, BLACK, 24);
    lcd_show_int_num(163, 100, ds3231_sec%10, 1, GREEN, BLACK, 24);

    switch(ds3231_day) {
        case 1:
        	lcd_show_string(20, 130, "SUN", YELLOW, BLACK, 24, 0);
            break;
        case 2:
        	lcd_show_string(20, 130, "MON", YELLOW, BLACK, 24, 0);
            break;
        case 3:
        	lcd_show_string(20, 130, "TUE", YELLOW, BLACK, 24, 0);
            break;
        case 4:
        	lcd_show_string(20, 130, "WED", YELLOW, BLACK, 24, 0);
            break;
        case 5:
        	lcd_show_string(20, 130, "THU", YELLOW, BLACK, 24, 0);
            break;
        case 6:
        	lcd_show_string(20, 130, "FRI", YELLOW, BLACK, 24, 0);
            break;
        case 7:
        	lcd_show_string(20, 130, "SAT", YELLOW, BLACK, 24, 0);
            break;
    }

    switch(ds3231_month) {
        case 1:
        	lcd_show_string(105, 130, "JAN", YELLOW, BLACK, 24, 0);
            break;
        case 2:
        	lcd_show_string(105, 130, "FEB", YELLOW, BLACK, 24, 0);
            break;
        case 3:
        	lcd_show_string(105, 130, "MAR", YELLOW, BLACK, 24, 0);
            break;
        case 4:
        	lcd_show_string(105, 130, "APR", YELLOW, BLACK, 24, 0);
            break;
        case 5:
        	lcd_show_string(105, 130, "MAY", YELLOW, BLACK, 24, 0);
            break;
        case 6:
        	lcd_show_string(105, 130, "JUN", YELLOW, BLACK, 24, 0);
            break;
        case 7:
        	lcd_show_string(105, 130, "JUL", YELLOW, BLACK, 24, 0);
            break;
        case 8:
        	lcd_show_string(105, 130, "AUG", YELLOW, BLACK, 24, 0);
            break;
        case 9:
        	lcd_show_string(105, 130, "SEP", YELLOW, BLACK, 24, 0);
            break;
        case 10:
        	lcd_show_string(105, 130, "OCT", YELLOW, BLACK, 24, 0);
            break;
        case 11:
        	lcd_show_string(105, 130, "NOV", YELLOW, BLACK, 24, 0);
            break;
        case 12:
        	lcd_show_string(105, 130, "DEC", YELLOW, BLACK, 24, 0);
            break;
    }
    lcd_show_int_num(70, 130, ds3231_date, 2, YELLOW, BLACK, 24);
	lcd_show_int_num(150, 130, 20, 2, YELLOW, BLACK, 24);
	lcd_show_int_num(176, 130, ds3231_year, 2, YELLOW, BLACK, 24);
}

void SetUpTime() {
    switch(statusSetupTime) {
        case INIT_SYSTEM:
            statusSetupTime = WATCHING;
            break;
        case WATCHING:
        	lcd_show_string_center(0, 0, "   MODE1: WATCHING CLOCK   ", WHITE, BLUE, 16, 0);
            if(IsButtonSettingMode()) {
                statusSetupTime = SET_HOUR;
                hour_tmp  = ds3231_hours;
                minute_tmp = ds3231_min;
                sec_tmp = ds3231_sec;
                date_tmp = ds3231_date;
                month_tmp = ds3231_month;
                year_tmp = ds3231_year;
                day_tmp = ds3231_day;
            }
            if (IsButtonScheduleMode()) {
                statusSetupTime = SCHEDULE_HOUR;

                hour_tmp  = ds3231_hours;
                minute_tmp = ds3231_min;
                sec_tmp = ds3231_sec;
                date_tmp = ds3231_date;
                month_tmp = ds3231_month;
                year_tmp = ds3231_year;
                day_tmp = ds3231_day;

                sec_timer = ds3231_sec;
                hour_timer = ds3231_hours;
                min_timer = ds3231_min;
                date_timer = ds3231_date;
                day_timer = ds3231_day;
                month_timer = ds3231_month;
                year_timer = ds3231_year;
            }
            if (isSchedule && ds3231_sec == sec_timer && ds3231_hours == hour_timer && ds3231_min == min_timer &&
                ds3231_date == date_timer && ds3231_day == day_timer && ds3231_month == month_timer &&
                ds3231_year == year_timer) lcd_show_string_center(0, 200, "TIME UP!", RED, WHITE, 24, 0);
            if (isSchedule && ds3231_sec == (sec_timer + 5) && ds3231_hours == hour_timer && ds3231_min == min_timer &&
                ds3231_date == date_timer && ds3231_day == day_timer && ds3231_month == month_timer &&
                ds3231_year == year_timer) {
                    lcd_clear(BLACK);
                    isSchedule = 0;
                }

            break;
        case SET_HOUR:
        	lcd_show_string_center(0, 0, "   MODE2: SETTING CLOCK   ", WHITE, BLUE, 16, 0);
            SetHour();
            if (IsButtonSettingMode()) {
                statusSetupTime = SET_MINUTE;
                ds3231_hours = hour_tmp;
                ds3231_write(ADDRESS_HOUR, ds3231_hours);
            }
            if (IsButtonSet()) hour_tmp = ds3231_hours;
            break;
        case SET_MINUTE:
            lcd_show_string_center(0, 0, "   MODE2: SETTING CLOCK   ", WHITE, BLUE, 16, 0);
            SetMinute();
            if(IsButtonSettingMode()) {
                statusSetupTime = SET_SECOND;
                ds3231_min = minute_tmp;
                ds3231_write(ADDRESS_MIN, ds3231_min);
            }
            if (IsButtonSet()) minute_tmp = ds3231_min;
            break;
        case SET_SECOND:
            lcd_show_string_center(0, 0, "   MODE2: SETTING CLOCK   ", WHITE, BLUE, 16, 0);
            SetSecond();
            if(IsButtonSettingMode()) {
                statusSetupTime = SET_DAY;
                ds3231_sec = sec_tmp;
                ds3231_write(ADDRESS_SEC, ds3231_sec);
            }
            if (IsButtonSet()) sec_tmp = ds3231_sec;
            break;
        case SET_DAY:
            lcd_show_string_center(0, 0, "   MODE2: SETTING CLOCK   ", WHITE, BLUE, 16, 0);
            SetDay();
            if(IsButtonSettingMode()) {
                statusSetupTime = SET_DATE;
                ds3231_day = day_tmp;
                ds3231_write(ADDRESS_DAY, ds3231_day);
            }
            if (IsButtonSet()) day_tmp = ds3231_day;
            break;
        case SET_DATE:
            lcd_show_string_center(0, 0, "   MODE2: SETTING CLOCK   ", WHITE, BLUE, 16, 0);
            SetDate();
            if(IsButtonSettingMode()) {
                statusSetupTime = SET_MONTH;
                ds3231_date = date_tmp;
                ds3231_write(ADDRESS_DATE, ds3231_date);
            }
            if (IsButtonSet()) date_tmp = ds3231_date;
            break;
        case SET_MONTH:
            lcd_show_string_center(0, 0, "   MODE2: SETTING CLOCK   ", WHITE, BLUE, 16, 0);
            SetMonth();
            if(IsButtonSettingMode()) {
                statusSetupTime = SET_YEAR;
                ds3231_month = month_tmp;
                ds3231_write(ADDRESS_MONTH, ds3231_month);
                ds3231_date = date_tmp;
                ds3231_write(ADDRESS_DATE, ds3231_date);
            }
            if (IsButtonSet()) {
                month_tmp = ds3231_month;
                date_tmp = ds3231_date;
            }
            break;
        case SET_YEAR:
            lcd_show_string_center(0, 0, "   MODE2: SETTING CLOCK   ", WHITE, BLUE, 16, 0);
            SetYear();
            if(IsButtonSettingMode()) {
                statusSetupTime = WATCHING;
                ds3231_year = year_tmp;
                ds3231_write(ADDRESS_YEAR, ds3231_year);
                ds3231_month = month_tmp;
                ds3231_write(ADDRESS_MONTH, ds3231_month);
                ds3231_date = date_tmp;
                ds3231_write(ADDRESS_DATE, ds3231_date);
                ds3231_hours = hour_tmp;
                ds3231_write(ADDRESS_HOUR, ds3231_hours);
                ds3231_min = minute_tmp;
                ds3231_write(ADDRESS_MIN, ds3231_min);
                ds3231_sec = sec_tmp;
                ds3231_write(ADDRESS_SEC, ds3231_sec);
            }
            if (IsButtonSet()) {
                year_tmp = ds3231_year;
                date_tmp = ds3231_date;
            }
            break;
        case SCHEDULE_HOUR:
            lcd_show_string_center(0, 0, "    MODE3: SCHEDULER   ", WHITE, BLUE, 16, 0);
            scheduleHour();
            if(IsButtonScheduleMode()) {
                statusSetupTime = SCHEDULE_MINUTE;
            }
            if (IsButtonSet()) hour_timer = ds3231_hours;
            break;

        case SCHEDULE_MINUTE:
            lcd_show_string_center(0, 0, "    MODE3: SCHEDULER   ", WHITE, BLUE, 16, 0);
            scheduleMinute();
            if(IsButtonScheduleMode()) {
                statusSetupTime = SCHEDULE_SECOND;
            }
            if (IsButtonSet()) min_timer = ds3231_min;
            break;

        case SCHEDULE_SECOND:
            lcd_show_string_center(0, 0, "    MODE3: SCHEDULER   ", WHITE, BLUE, 16, 0);
            scheduleSecond();
            if(IsButtonScheduleMode()) {
                statusSetupTime = SCHEDULE_DAY;
            }
            if (IsButtonSet()) sec_timer = ds3231_sec;
            break;

        case SCHEDULE_DAY:
            lcd_show_string_center(0, 0, "    MODE3: SCHEDULER   ", WHITE, BLUE, 16, 0);
            scheduleDay();
            if(IsButtonScheduleMode()) {
                statusSetupTime = SCHEDULE_DATE;
            }
            if (IsButtonSet()) day_timer = ds3231_day;
            break;

        case SCHEDULE_DATE:
            lcd_show_string_center(0, 0, "    MODE3: SCHEDULER   ", WHITE, BLUE, 16, 0);
            scheduleDate();
            if(IsButtonScheduleMode()) {
                statusSetupTime = SCHEDULE_MONTH;
            }
            if (IsButtonSet()) date_timer = ds3231_date;
            break;

        case SCHEDULE_MONTH:
            lcd_show_string_center(0, 0, "    MODE3: SCHEDULER   ", WHITE, BLUE, 16, 0);
            scheduleMonth();
            if(IsButtonScheduleMode()) {
                statusSetupTime = SCHEDULE_YEAR;
            }

            break;

        case SCHEDULE_YEAR:
            lcd_show_string_center(0, 0, "    MODE3: SCHEDULER   ", WHITE, BLUE, 16, 0);
            scheduleYear();
            if(IsButtonScheduleMode()) {
                statusSetupTime = WATCHING;
                ds3231_year = year_tmp;
                ds3231_write(ADDRESS_YEAR, ds3231_year);
                ds3231_month = month_tmp;
                ds3231_write(ADDRESS_MONTH, ds3231_month);
                ds3231_date = date_tmp;
                ds3231_write(ADDRESS_DATE, ds3231_date);
                ds3231_hours = hour_tmp;
                ds3231_write(ADDRESS_HOUR, ds3231_hours);
                ds3231_min = minute_tmp;
                ds3231_write(ADDRESS_MIN, ds3231_min);
                ds3231_sec = sec_tmp;
                ds3231_write(ADDRESS_SEC, ds3231_sec);
                isSchedule = 1;
            }

            break;

        default:
            statusSetupTime = INIT_SYSTEM;
            break;
    }
}

unsigned char IsButtonSettingMode() {
    if (button_count[12] == 1) return 1; // BTN_E
    else return 0;
}
unsigned char IsButtonSet() {
    if (button_count[14] == 1) return 1; // BTN_B
    else return 0;
}
unsigned char IsButtonScheduleMode() {
    if (button_count[13] == 1) return 1; // BTN_0
    else return 0;
}
unsigned char IsButtonUp() {
    if ((button_count[3] == 1) || (button_count[3] >= 10 && button_count[3]%4 == 0)) return 1;
    else return 0;
}
unsigned char IsButtonDown() {
    if ((button_count[7] == 1) || (button_count[7] >= 10 && button_count[7]%4 == 0)) return 1;
    else return 0;
}

void SetHour() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(70, 100, "  ", GREEN, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_hours++;
        if(ds3231_hours > 23) ds3231_hours = 0;
    }
    if(IsButtonDown()) {
        ds3231_hours--;
        if(ds3231_hours == 255) ds3231_hours = 23;
    }
    ds3231_write(ADDRESS_HOUR, ds3231_hours);
}

void SetMinute() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(110, 100, "  ", GREEN, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_min++;
        if(ds3231_min > 59) ds3231_min = 0;
    }
    if(IsButtonDown()) {
        ds3231_min--;
        if(ds3231_min == 255) ds3231_min = 59;
    }
    ds3231_write(ADDRESS_MIN, ds3231_min);
}

void SetSecond() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(150, 100, "  ", GREEN, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_sec++;
        if(ds3231_sec > 59) ds3231_sec = 0;
    }
    if(IsButtonDown()) {
        ds3231_sec--;
        if(ds3231_sec == 255) ds3231_sec = 59;
    }
    ds3231_write(ADDRESS_SEC, ds3231_sec);
}

void SetDay(){
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(20, 130, "   ", YELLOW, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_day++;
        if(ds3231_day > 7) ds3231_day = 1;
    }
    if(IsButtonDown()) {
        ds3231_day--;
        if(ds3231_day < 1) ds3231_day = 7;
    }
    ds3231_write(ADDRESS_DAY, ds3231_day);
}

// Help functions
unsigned char IsLeapYear(int y) {
    if((y % 400 == 0) || (y % 4 == 0 && y % 100 != 0)) return 1;
    return 0;
}
unsigned char GetMaxDay(unsigned char month, unsigned char year) {
    switch(month) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            if(IsLeapYear(year)) return 29;
            else return 28;
    }
    return 31;
}
void SetDate() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(70, 130, "  ", YELLOW, BLACK, 24, 0);

    if(IsButtonUp()) {
        ds3231_date++;
        // Decide correct date-limit for each month/year (leap-year or not)
        unsigned char maxDay = GetMaxDay(month_tmp, year_tmp);
        if(ds3231_date > maxDay) ds3231_date = 1;
    }
    if(IsButtonDown()) {
        ds3231_date--;
        // Decide correct date-limit for each month/year (leap-year or not)
        unsigned char maxDay = GetMaxDay(month_tmp, year_tmp);
        if(ds3231_date < 1) ds3231_date = maxDay;
    }
    ds3231_write(ADDRESS_DATE, ds3231_date);
}

void SetMonth() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(105, 130, "   ", YELLOW, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_month++;
        if(ds3231_month > 12) ds3231_month = 1;
        // Limit date correspond with month
        unsigned char maxDay = GetMaxDay(ds3231_month, year_tmp);
        if(ds3231_date > maxDay) {
            ds3231_date = maxDay;
            ds3231_write(ADDRESS_DATE, ds3231_date);
        }
    }
    if(IsButtonDown()) {
        ds3231_month--;
        if(ds3231_month < 1) ds3231_month = 12;
        // Limit date correspond with month
        unsigned char maxDay = GetMaxDay(ds3231_month, year_tmp);
        if(ds3231_date > maxDay) {
            ds3231_date = maxDay;
            ds3231_write(ADDRESS_DATE, ds3231_date);
        }
    }
    ds3231_write(ADDRESS_MONTH, ds3231_month);
}

void SetYear() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(176, 130, "  ", YELLOW, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_year++;
        // Limit date correspond with year
        unsigned char maxDay = GetMaxDay(month_tmp, ds3231_year);
        if(ds3231_date > maxDay) {
            ds3231_date = maxDay;
            ds3231_write(ADDRESS_DATE, ds3231_date);
        }
    }
    if(IsButtonDown()) {
        ds3231_year--;
        // Limit date correspond with year
        unsigned char maxDay = GetMaxDay(month_tmp, ds3231_year);
        if(ds3231_date > maxDay) {
            ds3231_date = maxDay;
            ds3231_write(ADDRESS_DATE, ds3231_date);
        }
    }
    ds3231_write(ADDRESS_YEAR, ds3231_year);
}

void scheduleHour() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(70, 100, "  ", GREEN, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_hours++;
        if(ds3231_hours > 23) ds3231_hours = hour_tmp;
    }
    hour_timer = ds3231_hours;
}
void scheduleMinute() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(110, 100, "  ", GREEN, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_min++;
        if(ds3231_min > 59) ds3231_min = minute_tmp;
    }
    min_timer = ds3231_min;
}
void scheduleSecond() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(150, 100, "  ", GREEN, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_sec++;
        if(ds3231_sec > 59) ds3231_sec = sec_tmp;
    }
    sec_timer = ds3231_sec;
}
void scheduleDay() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(20, 130, "   ", YELLOW, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_day++;
        if(ds3231_day > 7) ds3231_day = day_tmp;
    }
    day_timer = ds3231_day;
}
void scheduleDate() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(70, 130, "  ", YELLOW, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_date++;
        unsigned char maxDay = GetMaxDay(month_tmp, year_tmp);
        if(ds3231_date > maxDay) ds3231_date = date_tmp;
    }
    date_timer = ds3231_date;
}

void scheduleMonth() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(105, 130, "   ", YELLOW, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_month++;
        if(ds3231_month > 12) ds3231_month = month_tmp;
        unsigned char maxDay = GetMaxDay(ds3231_month, year_tmp);
        if(ds3231_date > maxDay) ds3231_date = maxDay;
    }
    month_timer = ds3231_month;
}

void scheduleYear() {
    timeBlink = (timeBlink + 1) % 20;
    if(timeBlink < 5) lcd_show_string(176, 130, "  ", YELLOW, BLACK, 24, 0);
    if(IsButtonUp()) {
        ds3231_year++;
        unsigned char maxDay = GetMaxDay(month_timer, ds3231_year);
        if(ds3231_date > maxDay) ds3231_date = maxDay;
    }
    year_timer = ds3231_year;
}



/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
