#ifndef INC_MAIN_MENU_SETTINGS_H_
#define INC_MAIN_MENU_SETTINGS_H_

typedef enum difficulty {
	EASY = 0x00, MEDIUM = 0x01, HARD = 0x02
} Difficulty;

extern Difficulty difficulty;
extern uint16_t paddle_color;
extern uint16_t ball_color;

#endif
