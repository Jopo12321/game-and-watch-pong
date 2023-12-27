#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "game.h"
#include "gw_hardware/gw_buttons.h"
#include "gw_hardware/gw_flash.h"
#include "gw_hardware/gw_lcd.h"
#include "gw_hardware/gw_bq24072.h"
#include "gw_hardware/gw_rtc.h"
#include "gw_hardware/gw_audio.h"
#include "graphics.h"

#define randint(max, min) 	(rand() % (max + 1 - min) + min)
#define randsing()			(rand() > RAND_MAX/2 ? -1: 1)

int score_p1 = 0;
int score_p2 = 0;
char score_p1_buf[6];
char score_p2_buf[6];
char text_buf[256];

const int p1_speed = 2;
const int p1_height = 40;
const int p1_width = 5;
int p1_x = 3;
int p1_y = 110;

const int p2_speed = 2;
const int p2_height = 40;
const int p2_width = 5;
int p2_x = 320 - 3 - p2_width;
int p2_y = 110;

const int ball_speed = 3;
const int ball_r = 8;
int ball_xspeed;
int ball_yspeed;
int ball_x;
int ball_y;

void game_init(void) {
	uint16_t *buf = lcd_get_active_buffer();
	draw_background(buf);
	ball_xspeed = randsing() * ball_speed;
	ball_yspeed = randsing() * ball_speed;
	ball_x = 160;
	ball_y = randint(190, 70);
	lcd_sync();
	lcd_swap();
	lcd_wait_for_vblank();
}

void game_loop(void) {
	uint16_t *buf = lcd_get_active_buffer();
	draw_background(buf);
	update_player(buf);
	update_ai(buf);
	update_ball(buf);
	lcd_swap();
	lcd_wait_for_vblank();
}

void draw_background(uint16_t *buf) {
	LCD_Clear(buf, LCD_COLOR_BLACK);
	LCD_DrawRect(buf, 0, 20, LCD_GetXSize() - 1, LCD_GetYSize() - 21,
	LCD_COLOR_WHITE);
	draw_score(buf);
}

void draw_score(uint16_t *buf) {
	LCD_SetFont(&Font16);
	sprintf(score_p1_buf, "%d", score_p1);
	LCD_DisplayStringAt(buf, 0, 4, (uint8_t*) score_p1_buf, LEFT_MODE);
	sprintf(score_p2_buf, "%d", score_p2);
	LCD_DisplayStringAt(buf, 2, 4, (uint8_t*) score_p2_buf, RIGHT_MODE);
	sprintf(text_buf, "SCORE");
	LCD_DisplayStringAt(buf, 1, 4, (uint8_t*) text_buf, CENTER_MODE);
}

void update_player(uint16_t *buf) {
	uint32_t buttons = buttons_get();
	if (buttons & B_Up) {
		if (p1_y >= 20 + p1_speed + 2) {
			p1_y = p1_y - p1_speed;
		}
	}
	if (buttons & B_Down) {
		if (p1_y <= LCD_GetYSize() - p1_height - p1_speed - 2) {
			p1_y = p1_y + p1_speed;
		}
	}
	if (buttons & B_GAME) {
		score_p1 = 0;
		score_p2 = 0;
		p1_x = 3;
		p1_y = 110;
		p2_x = 320 - 3 - p2_width;
		p2_y = 110;
		HAL_Delay(500);
		game_init();
	}
	if (buttons & B_POWER) {
		HAL_NVIC_SystemReset();
	}
	LCD_FillRect(buf, p1_x, p1_y, p1_width, p1_height, LCD_COLOR_WHITE);
}

void update_ball(uint16_t *buf) {

	if (ball_x <= ball_r + ball_speed + p1_x + p1_width / 2
			&& ball_xspeed == -ball_speed) {
		ball_xspeed = ball_speed;
		if (ball_y >= p1_y - 2 && ball_y <= p1_y + p1_height + 2) {

		} else {
			score_p2++;
			draw_score(buf);
			LCD_FillCircle(buf, ball_x, ball_y, ball_r, LCD_COLOR_RED);
			lcd_swap();
			lcd_wait_for_vblank();
			HAL_Delay(1000);
			game_init();
		}
	}
	if (ball_x >= p2_x + p2_width / 2 - ball_r - ball_speed
			&& ball_xspeed == ball_speed) {
		ball_xspeed = -ball_speed;
		if (ball_y >= p2_y - 2 && ball_y <= p2_y + p2_height + 2) {

		} else {
			score_p1++;
			draw_score(buf);
			LCD_FillCircle(buf, ball_x, ball_y, ball_r, LCD_COLOR_RED);
			lcd_swap();
			lcd_wait_for_vblank();
			HAL_Delay(1000);
			game_init();
		}
	}
	if (ball_y <= 20 + ball_r + ball_speed && ball_yspeed == -ball_speed) {
		ball_yspeed = ball_speed;
	}
	if (ball_y >= LCD_GetYSize() - ball_r - ball_speed
			&& ball_yspeed == ball_speed) {
		ball_yspeed = -ball_speed;
	}
	ball_x = ball_x + ball_xspeed;
	ball_y = ball_y + ball_yspeed;

	LCD_FillCircle(buf, ball_x, ball_y, ball_r, LCD_COLOR_WHITE);
}

void update_ai(uint16_t *buf) {
	if (ball_y < (p2_y + (p2_height / 2))) {
		if (p2_y >= 20 + p2_speed + 2) {
			p2_y = p2_y - p2_speed;
		}
	}
	if (ball_y > (p2_y + (p2_height / 2))) {
		if (p2_y <= LCD_GetYSize() - p2_height - p2_speed - 2) {
			p2_y = p2_y + p2_speed;
		}
	}
	LCD_FillRect(buf, p2_x, p2_y, p2_width, p2_height, LCD_COLOR_WHITE);
}
