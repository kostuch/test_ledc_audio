#include <Arduino.h>
#include "pwm_audio.h"

#include "math.h"
#include "stdio.h"
#define PI_2 (6.283185307179f)
#define CONFIG_LEFT_CHANNEL_GPIO 19

uint8_t *buf;
uint32_t src_buf_size = 240;
uint32_t buf_index;
size_t cnt;
uint16_t block_w = 256;

void sin_test()
{
	if (buf_index < src_buf_size)
	{
		if ((src_buf_size - buf_index) < block_w) block_w = src_buf_size - buf_index;

		pwm_audio_write(buf + buf_index, block_w, &cnt, 0);
		buf_index += cnt;
	}
	else
	{
		buf_index = 0;
		block_w = 256;
	}
}

void sin_test2()
{
	pwm_audio_write(buf + buf_index, block_w, &cnt, 0);
	buf_index = (buf_index + cnt) % src_buf_size;
}

void setup()
{
	buf = (uint8_t *)malloc(src_buf_size);

	if (buf == NULL)
	{
		ESP_LOGE(TAG, "malloc error");
		vTaskDelay(portMAX_DELAY);
	}

	for (size_t i = 0; i < src_buf_size; i++)
	{
		buf[i] = 128 + (127.8f * sinf(PI_2 * (float)i / (float)src_buf_size));
		printf("sin%d = %d\n", i, buf[i]);
	}

	pwm_audio_config_t pac;
	pac.duty_resolution = LEDC_TIMER_8_BIT;
	pac.gpio_num_left = CONFIG_LEFT_CHANNEL_GPIO;
	pac.ledc_channel_left = LEDC_CHANNEL_0;
	pac.gpio_num_right = -1;
	pac.ledc_channel_right = LEDC_CHANNEL_1;
	pac.ledc_timer_sel = LEDC_TIMER_0;
	pac.tg_num = TIMER_GROUP_0;
	pac.timer_num = TIMER_0;
	pac.ringbuf_len = 1024;
	pwm_audio_init(&pac);
	pwm_audio_set_param(48000, LEDC_TIMER_8_BIT, 1);
	pwm_audio_start();
}

void loop()
{
	sin_test();
}