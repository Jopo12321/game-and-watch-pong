#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_over.h"
#include "game.h"
#include "main_menu/main_menu.h"
#include "gw_hardware/gw_buttons.h"
#include "gw_hardware/gw_flash.h"
#include "gw_hardware/gw_lcd.h"
#include "gw_hardware/gw_bq24072.h"
#include "gw_hardware/gw_rtc.h"
#include "gw_hardware/gw_audio.h"
#include "graphics/graphics.h"
#include "graphics/sprites.h"

static void game_over_loop();
static void process_input();
static void update();
static void draw_background(uint16_t *buf);
static void menu_select();

int game_over_menu = 1;

extern int restart_game;
extern int in_game;

static uint16_t selected_item_color = LCD_COLOR_DARKBLUE;
static int ticks = 0;
static int action_delay = 0;
static uint8_t selected_item = 0;
int winner;

void game_over_init(int player_won) {
	winner = player_won;
	game_over_menu = 1;
	selected_item = 0;
	action_delay = 15;
	LCD_SetTextColor(LCD_COLOR_WHITE);
	LCD_SetFont(&Font24);
	LCD_SetFontScale(1);
	while (game_over_menu) {
		game_over_loop();
	}
	menu_select();
}

static void game_over_loop() {
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
			action_delay = 15;
		}
		if (buttons & B_Down) {
			selected_item++;
			action_delay = 15;
		}
		if (buttons & B_A) {
			game_over_menu = 0;
		}
	} else {
		action_delay--;
	}
}
static void update() {
	ticks++;
}

static void draw_background(uint16_t *buf) {
	LCD_SetBackColor(LCD_COLOR_GRAY);
	LCD_FillCircle(buf, 80, 70, 3, LCD_COLOR_WHITE);
	LCD_FillCircle(buf, 239, 70, 3, LCD_COLOR_WHITE);
	LCD_FillCircle(buf, 80, 169, 3, LCD_COLOR_WHITE);
	LCD_FillCircle(buf, 239, 169, 3, LCD_COLOR_WHITE);

	LCD_FillRect(buf, 77, 70, 3, 100, LCD_COLOR_WHITE);
	LCD_FillRect(buf, 240, 70, 3, 100, LCD_COLOR_WHITE);
	LCD_FillRect(buf, 80, 67, 160, 3, LCD_COLOR_WHITE);
	LCD_FillRect(buf, 80, 170, 160, 3, LCD_COLOR_WHITE);

	LCD_FillRect(buf, 80, 70, 160, 100, LCD_COLOR_GRAY);

	LCD_SetTextColor(LCD_COLOR_DARKRED);
	if (winner == 1) {
		LCD_DisplayStringAt(buf, 0, 80, (uint8_t*) "You win!\0", CENTER_MODE);
	} else {
		LCD_DisplayStringAt(buf, 0, 80, (uint8_t*) "You lose!\0", CENTER_MODE);
	}
	if (selected_item % 2 == 0) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 110, (uint8_t*) "Restart\0", CENTER_MODE);
	if (selected_item % 2 == 1) {
		LCD_SetTextColor(selected_item_color);
	} else {
		LCD_SetTextColor(LCD_COLOR_WHITE);
	}
	LCD_DisplayStringAt(buf, 0, 140, (uint8_t*) "Exit\0", CENTER_MODE);
	LCD_SetBackColor(LCD_COLOR_BLACK);
}

static void menu_select() {
	if (selected_item % 2 == 0) {
		in_game = 0;
		restart_game = 1;
	}
	if (selected_item % 2 == 1) {
		in_game = 0;
	}
}
