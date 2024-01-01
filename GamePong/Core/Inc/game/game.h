#ifndef INC_GAME_H_
#define INC_GAME_H_

#include <stdint.h>

void game_init();
void game_loop();

void draw_background(uint16_t *buf);
void draw_score(uint16_t *buf);
void update_player(uint16_t *buf);
void update_ball(uint16_t *buf);
void update_ai(uint16_t *buf);

#endif /* INC_GAME_H_ */
