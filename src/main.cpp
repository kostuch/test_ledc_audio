#include <Arduino.h>
#include "pwm_audio.h"

#include "math.h"
#include "stdio.h"
#include "chirp22.h"
#define PI_2 6.283185307179
#define CONFIG_LEFT_CHANNEL_GPIO 19

#define AUDIO_SRC_SIZE 256
#define RINGBUFFER_SIZE 1024
int8_t buf[AUDIO_SRC_SIZE];
bool replay = true;
bool done;

void sin_test()
{
	static uint16_t buf_index;
	size_t cnt;
	pwm_audio_write((uint8_t*)(buf + buf_index), AUDIO_SRC_SIZE, &cnt, portMAX_DELAY);
	buf_index = (buf_index + cnt) % AUDIO_SRC_SIZE;
}

void wave_test()
{
	static uint32_t index;
	size_t cnt;
	pwm_audio_write((uint8_t*)(samples + index), AUDIO_SRC_SIZE, &cnt, 0);
	index = index + cnt;
	
	if (index > sizeof(samples))
	{
		if (!replay) done = true;

		index = 0;
	}
}

void setup()
{
	for (size_t i = 0; i < AUDIO_SRC_SIZE; i++)	buf[i] = 127 * sin(PI_2 * i / AUDIO_SRC_SIZE);

	pwm_audio_config_t pac;
	pac.duty_resolution = LEDC_TIMER_8_BIT;
	pac.gpio_num_left = CONFIG_LEFT_CHANNEL_GPIO;
	pac.ledc_channel_left = LEDC_CHANNEL_0;
	pac.gpio_num_right = -1;
	pac.ledc_channel_right = LEDC_CHANNEL_1;
	pac.ledc_timer_sel = LEDC_TIMER_0;
	pac.tg_num = TIMER_GROUP_0;
	pac.timer_num = TIMER_0;
	pac.ringbuf_len = RINGBUFFER_SIZE;
	pwm_audio_init(&pac);
	pwm_audio_set_param(sampleRate, LEDC_TIMER_8_BIT, 1);
    pwm_audio_start();
}

void loop()
{
	//sin_test();
	if (!done) wave_test();
}