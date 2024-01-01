#ifndef INC_GW_AUDIO_H_
#define INC_GW_AUDIO_H_

#include <stdint.h>

#define AUDIO_SAMPLE_RATE   		(48000)
#define AUDIO_BUFFER_LENGTH 		(AUDIO_SAMPLE_RATE/20)
#define AUDIO_PROGRAMMABLE_LENGTH	(40) //2s

// Notes, the sound at higher notes may be not accurate
#define NOTE_G2                 245
#define NOTE_GS2        		231
#define NOTE_A2         		218
#define NOTE_AS2                206
#define NOTE_B2                 194
#define NOTE_C3                 183
#define NOTE_CS3                173
#define NOTE_D3                 163
#define NOTE_DS3                154
#define NOTE_E3                 146
#define NOTE_F3                 137
#define NOTE_FS3                130
#define NOTE_G3                 122
#define NOTE_GS3                116
#define NOTE_A3                 109
#define NOTE_AS3                103
#define NOTE_B3                 97
#define NOTE_C4                 92
#define NOTE_CS4                87
#define NOTE_D4                 82
#define NOTE_DS4                77
#define NOTE_E4                 73
#define NOTE_F4                 69
#define NOTE_FS4                65
#define NOTE_G4                 61
#define NOTE_GS4                58
#define NOTE_A4                 55
#define NOTE_AS4                51
#define NOTE_B4                 49
#define NOTE_C5                 46
#define NOTE_CS5                43
#define NOTE_D5                 41
#define NOTE_DS5                39
#define NOTE_E5                 36
#define NOTE_F5                 34
#define NOTE_FS5                32
#define NOTE_G5                 31
#define NOTE_GS5                29
#define NOTE_A5                 27
#define NOTE_AS5                26
#define NOTE_B5                 24
#define NOTE_C6                 23
#define NOTE_CS6                22
#define NOTE_D6                 20
#define NOTE_DS6                19
#define NOTE_E6                 18
#define NOTE_F6                 17

enum duty {
	PW_SHORT, PW_HALF, PW_LONG
};

typedef struct Generator_vals {
	uint8_t gen_freq[AUDIO_PROGRAMMABLE_LENGTH];
	uint8_t gen_vol[AUDIO_PROGRAMMABLE_LENGTH];
	uint8_t gen_duty[AUDIO_PROGRAMMABLE_LENGTH];
	int16_t audiobuffer[AUDIO_BUFFER_LENGTH];
	uint16_t samples_high;
	uint16_t samples_low;
} Generator;

typedef struct Noise_vals {
	uint8_t noise_freq[AUDIO_PROGRAMMABLE_LENGTH];
	uint8_t noise_vol[AUDIO_PROGRAMMABLE_LENGTH];
	int16_t audiobuffer[AUDIO_BUFFER_LENGTH];
	uint16_t LFSR;
	uint16_t last;
} Noise;

extern int16_t audiobuffer[AUDIO_BUFFER_LENGTH] __attribute__((section (".audio")))__attribute__ ((aligned (16)));
extern Generator gen1 __attribute__((section (".audio")))__attribute__ ((aligned (16)));
extern Generator gen2 __attribute__((section (".audio")))__attribute__ ((aligned (16)));
extern Generator gen3 __attribute__((section (".audio")))__attribute__ ((aligned (16)));
extern Noise noise1 __attribute__((section (".audio")))__attribute__ ((aligned (16)));

void gw_audio_init(void);
void gw_audio_stop(void);

void gw_audio_pause(void);
void gw_audio_resume(void);

uint32_t get_audio_frame(void);

void play_generator(Generator *gen, int start, int len);
void move_generator(Generator *gen);

void play_noise(Noise *noise, int start, int len);
void move_noise(Noise *noise);

#endif /* INC_GW_AUDIO_H_ */
