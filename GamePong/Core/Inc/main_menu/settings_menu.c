#include <main_menu/main_menu.h>
#include <main_menu/settings_menu.h>
#include <main_menu/settings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game/game.h"
#include "gw_hardware/gw_buttons.h"
#include "gw_hardware/gw_flash.h"
#include "gw_hardware/gw_lcd.h"
#include "gw_hardware/gw_bq24072.h"
#include "gw_hardware/gw_rtc.h"
#include "gw_hardware/gw_audio.h"
#include "graphics/graphics.h"
#include "graphics/sprites.h"

static void settings_menu_loop();
static void process_input();
static void update();
static void draw_background(uint16_t *buf);
static void change_settings(int dir);

Difficulty difficulty = EASY;
uint16_t paddle_color = LCD_COLOR_WHITE;
uint16_t ball_color = LCD_COLOR_WHITE;

static int ticks = 0;
int in_settings_menu = 1;

static int action_delay = 0;
static uint8_t selected_item = 0;
static uint16_t selected_item_color = LCD_COLOR_DARKYELLOW;

static uint8_t paddle_color_idx = 0;
static uint8_t ball_color_idx = 0;
static uint16_t settings_color_pallete[] = {
LCD_COLOR_WHITE, LCD_COLOR_RED, LCD_COLOR_GREEN, LCD_COLOR_BLUE, LCD_COLOR_CYAN,
LCD_COLOR_MAGENTA, LCD_COLOR_BROWN, LCD_COLOR_ORANGE,
LCD_COLOR_LIGHTRED, LCD_COLOR_LIGHTGREEN, LCD_COLOR_LIGHTBLUE,
LCD_COLOR_LIGHTMAGENTA, LCD_COLOR_DARKRED, LCD_COLOR_DARKGREEN,
LCD_COLOR_DARKBLUE, LCD_COLOR_DARKMAGENTA };

void settings_menu_init() {
	action_delay = 10;
	selected_item = 0;
	in_settings_menu = 1;
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font24);
	LCD_SetFontScale(1);
	while (in_settings_menu) {
		settings_menu_loop();
	}
	//main_menu_init();
}
static void settings_menu_loop() {
	uint16_t *fb = lcd_get_active_buffer();
	draw_background(fb);

	process_input();
	update();

	lcd_swap();
	lcd_wait_for_vblank();
}

static void process_input() {
	uint32_t buttons = buttons_get();
	if (buttons & B_POWER) {
		HAL_NVIC_SystemReset();
	}
	if (action_delay <= 0) {
		if (buttons & B_Up) {
			selected_item--;
			action_delay = 10;
		}
		if (buttons & B_Down) {
			selected_item++;
			action_delay = 10;
		}
		if (buttons & B_Left) {
			action_delay = 10;
			change_settings(-1);
		}
		if (buttons & B_Right) {
			action_delay = 10;
			change_settings(1);
		}
		if (buttons & B_A) {
			action_delay = 10;
			change_settings(1);
		}
	} else {
		action_delay--;
	}
	if (buttons & B_B) {
		in_settings_menu = 0;
	}
}

static void update() {
	ticks++;
}

static void draw_background(uint16_t *buf) {
	LCD_Clear(buf, LCD_COLOR_BLACK);

	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFontScale(2);
	LCD_DisplayChar(buf, 40, 20, 'S');
	LCD_DisplayChar(buf, 70, 20, 'e');
	LCD_DisplayChar(buf, 100, 20, 't');
	LCD_DisplayChar(buf, 130, 20, 't');
	LCD_DisplayChar(buf, 160, 20, 'i');
	LCD_DisplayChar(buf, 190, 20, 'n');
	LCD_DisplayChar(buf, 220, 20, 'g');
	LCD_DisplayChar(buf, 250, 20, 's');
	LCD_SetFontScale(1);

	if (selected_item % 4 == 0) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	switch (difficulty) {
	case EASY:
		LCD_DisplayStringAt(buf, 0, 110, (uint8_t*) "Difficulty: EASY\0",
				CENTER_MODE);
		break;
	case MEDIUM:
		LCD_DisplayStringAt(buf, 0, 110, (uint8_t*) "Difficulty: MEDIUM\0",
				CENTER_MODE);
		break;
	case HARD:
		LCD_DisplayStringAt(buf, 0, 110, (uint8_t*) "Difficulty: HARD\0",
				CENTER_MODE);
		break;
	default:
		break;
	}
	if (selected_item % 4 == 1) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 140, (uint8_t*) "Paddle color:   \0",
			CENTER_MODE);
	LCD_FillRect(buf, 250, 150, 50, 8, paddle_color);
	if (selected_item % 4 == 2) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 170, (uint8_t*) "Ball color: \0", CENTER_MODE);
	LCD_FillCircle(buf, 275, 180, 10, ball_color);
	if (selected_item % 4 == 3) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 200, (uint8_t*) "Back\0", CENTER_MODE);
}

static void change_settings(int dir) {
	switch (selected_item % 4) {
	case 0:
		switch (difficulty) {
		case EASY:
			if (dir > 0) {
				difficulty = MEDIUM;
			} else {
				difficulty = HARD;
			}
			break;
		case MEDIUM:
			if (dir > 0) {
				difficulty = HARD;
			} else {
				difficulty = EASY;
			}
			break;
		case HARD:
			if (dir > 0) {
				difficulty = EASY;
			} else {
				difficulty = MEDIUM;
			}
			break;
		default:
			break;
		}
		break;
	case 1:
		if (dir > 0) {
			paddle_color_idx++;
		} else {
			paddle_color_idx--;
		}
		paddle_color_idx = paddle_color_idx % 16;
		paddle_color = settings_color_pallete[paddle_color_idx];
		break;
	case 2:
		if (dir > 0) {
			ball_color_idx++;
		} else {
			ball_color_idx--;
		}
		ball_color_idx = ball_color_idx % 16;
		ball_color = settings_color_pallete[ball_color_idx];
		break;
	case 3:
		in_settings_menu = 0;
		break;
	default:
		break;
	}
}
