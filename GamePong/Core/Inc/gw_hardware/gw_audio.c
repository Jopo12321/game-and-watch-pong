#include "gw_audio.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern SAI_HandleTypeDef hsai_BlockA1;
extern DMA_HandleTypeDef hdma_sai1_a;

int16_t audiobuffer[AUDIO_BUFFER_LENGTH];
Generator gen1;
Generator gen2;
Generator gen3;
Noise noise1;

static uint32_t audio_frame = 0;

void gw_audio_init(void) {
	memset(audiobuffer, 0, sizeof(audiobuffer));
	HAL_SAI_Transmit_DMA(&hsai_BlockA1, (uint8_t*) audiobuffer,
	AUDIO_BUFFER_LENGTH);
	int i;
	for (i = 0; i < AUDIO_PROGRAMMABLE_LENGTH; i++) {
		(&gen1)->gen_freq[i] = 0;
		(&gen1)->gen_duty[i] = PW_HALF;
		(&gen1)->gen_vol[i] = (uint8_t) 0;

		(&gen2)->gen_freq[i] = 0;
		(&gen2)->gen_duty[i] = PW_HALF;
		(&gen2)->gen_vol[i] = (uint8_t) 0;

		(&gen3)->gen_freq[i] = 0;
		(&gen3)->gen_duty[i] = PW_HALF;
		(&gen3)->gen_vol[i] = (uint8_t) 0;

		(&noise1)->noise_freq[i] = 0;
		(&noise1)->noise_vol[i] = (uint8_t) 0;
	}

	(&gen1)->samples_high = 0;
	(&gen1)->samples_low = 0;
	(&gen1)->samples_high = 0;
	(&gen1)->samples_low = 0;
	(&gen1)->samples_high = 0;
	(&gen1)->samples_low = 0;
	(&noise1)->LFSR = 1;
	(&noise1)->last = 0;

	for (i = 0; i < AUDIO_BUFFER_LENGTH; i++) {
		(&gen1)->audiobuffer[i] = 0;
		(&gen2)->audiobuffer[i] = 0;
		(&gen3)->audiobuffer[i] = 0;
		(&noise1)->audiobuffer[i] = 0;
	}
}

void gw_audio_stop(void) {
	HAL_SAI_DMAStop(&hsai_BlockA1);
}

void gw_audio_pause(void) {
	HAL_SAI_DMAPause(&hsai_BlockA1);
}

void gw_audio_resume(void) {
	HAL_SAI_DMAResume(&hsai_BlockA1);
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai) {
	move_generator(&gen1);
	play_generator(&gen1, 0, AUDIO_BUFFER_LENGTH / 2);
	move_generator(&gen2);
	play_generator(&gen2, 0, AUDIO_BUFFER_LENGTH / 2);
	move_generator(&gen3);
	play_generator(&gen3, 0, AUDIO_BUFFER_LENGTH / 2);
	move_noise(&noise1);
	play_noise(&noise1, 0, AUDIO_BUFFER_LENGTH / 2);
	int i;
	for (i = 0; i < AUDIO_BUFFER_LENGTH / 2; i++) {
		audiobuffer[i] = 4
				* (gen1.audiobuffer[i] + gen2.audiobuffer[i]
						+ gen3.audiobuffer[i] + noise1.audiobuffer[i]);
	}
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai) {
	audio_frame++;
	play_generator(&gen1, AUDIO_BUFFER_LENGTH / 2, AUDIO_BUFFER_LENGTH / 2);
	play_generator(&gen2, AUDIO_BUFFER_LENGTH / 2, AUDIO_BUFFER_LENGTH / 2);
	play_generator(&gen3, AUDIO_BUFFER_LENGTH / 2, AUDIO_BUFFER_LENGTH / 2);
	play_noise(&noise1, AUDIO_BUFFER_LENGTH / 2, AUDIO_BUFFER_LENGTH / 2);
	int i;
	for (i = AUDIO_BUFFER_LENGTH / 2; i < AUDIO_BUFFER_LENGTH; i++) {
		audiobuffer[i] = 2
				* (gen1.audiobuffer[i] + gen2.audiobuffer[i]
						+ gen3.audiobuffer[i] + noise1.audiobuffer[i]);
	}
}

void play_generator(Generator *gen, int start, int len) {
	int i;
	uint8_t freq = (*gen).gen_freq[0];
	if (freq == 0) {
		for (i = start; i < start + len; i++) {
			gen->audiobuffer[i] = 0;
		}
		return;
	}
	uint8_t duty = (*gen).gen_duty[0];
	uint16_t samples_high;
	uint16_t samples_low;
	switch (duty) {
	case PW_SHORT:
		samples_high = 1 * (uint16_t) freq;
		samples_low = 3 * (uint16_t) freq;
		break;
	case PW_HALF:
		samples_high = 2 * (uint16_t) freq;
		samples_low = 2 * (uint16_t) freq;
		break;
	case PW_LONG:
		samples_high = 3 * (uint16_t) freq;
		samples_low = 1 * (uint16_t) freq;
		break;
	default:
		samples_high = 2 * (uint16_t) freq;
		samples_low = 2 * (uint16_t) freq;
		break;
	}
	uint8_t vol = (*gen).gen_vol[0];
	uint16_t counter_high = (*gen).samples_high;
	uint16_t counter_low = (*gen).samples_low;

	if (counter_high > samples_high) {
		counter_high = samples_high;
	}
	if (counter_low > samples_low) {
		counter_low = samples_low;
	}

	for (i = start; i < start + len; i++) {
		if (counter_high > 0) {
			gen->audiobuffer[i] = (int16_t) vol;
			counter_high--;
		} else {
			if (counter_low > 0) {
				gen->audiobuffer[i] = -(int16_t) vol;
				counter_low--;
			} else {
				counter_high = samples_high;
				counter_low = samples_low;
				i--;
			}
		}
	}
	gen->samples_high = counter_high;
	gen->samples_low = counter_low;
}

void move_generator(Generator *gen) {
	int i, n = AUDIO_PROGRAMMABLE_LENGTH;
	uint8_t *a = gen->gen_freq;
	uint8_t temp = a[0];
	for (i = 0; i < n - 1; i++) {
		a[i] = a[i + 1];
	}
	a[n - 1] = temp;
	a = gen->gen_vol;
	temp = a[0];
	for (i = 0; i < n - 1; i++) {
		a[i] = a[i + 1];
	}
	a[n - 1] = temp;
	a = gen->gen_duty;
	temp = a[0];
	for (i = 0; i < n - 1; i++) {
		a[i] = a[i + 1];
	}
	a[n - 1] = temp;
}

void play_noise(Noise *noise, int start, int len) {
	int i;
	uint16_t freq = 4 * (uint16_t) (*noise).noise_freq[0];
	if (freq == 0) {
		for (i = start; i < start + len; i++) {
			noise->audiobuffer[i] = 0;
		}
		return;
	}
	uint8_t vol = (*noise).noise_vol[0];
	uint16_t LFSR = (*noise).LFSR;
	uint16_t last = (*noise).last;
	int out = 0;
	if (last > freq) {
		last = freq;
	}
	for (i = start; i < start + len; i++) {
		if (last > 0) {
			noise->audiobuffer[i] =
					(out == 1) ? -1 * (int16_t) vol : (int16_t) vol;
			last--;
		} else {
			last = freq;
			i--;
			uint16_t xor_result = (LFSR & 0b01) ^ ((LFSR & 0b10) >> 1);
			LFSR = (LFSR >> 1) | (xor_result << 14);
			out = ~LFSR & 0x01;
		}
	}
	noise->LFSR = LFSR;
	noise->last = last;
}

void move_noise(Noise *noise) {
	int i, n = AUDIO_PROGRAMMABLE_LENGTH;
	uint8_t *a = noise->noise_freq;
	uint8_t temp = a[0];
	for (i = 0; i < n - 1; i++) {
		a[i] = a[i + 1];
	}
	a[n - 1] = temp;
	a = noise->noise_vol;
	temp = a[0];
	for (i = 0; i < n - 1; i++) {
		a[i] = a[i + 1];
	}
	a[n - 1] = temp;
}

uint32_t get_audio_frame() {
	return audio_frame;
}
