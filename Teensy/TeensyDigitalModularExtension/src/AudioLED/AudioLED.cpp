#include <Arduino.h>
#include "utility/dspinst.h"
#include "AudioLED.h"

void AudioLED::update(void)
{
	audio_block_t* block = receiveReadOnly(0);
	if (!block) {
		analogWrite(LEDpin, 0);
		return;
	}

	int64_t sum = 0;
#if defined(__ARM_ARCH_7EM__)
	uint32_t* p = (uint32_t*)(block->data);
	uint32_t* end = p + AUDIO_BLOCK_SAMPLES / 2;
	
	do {
		uint32_t n1 = *p++;
		uint32_t n2 = *p++;
		uint32_t n3 = *p++;
		uint32_t n4 = *p++;
		sum = multiply_accumulate_16tx16t_add_16bx16b(sum, n1, n1);
		sum = multiply_accumulate_16tx16t_add_16bx16b(sum, n2, n2);
		sum = multiply_accumulate_16tx16t_add_16bx16b(sum, n3, n3);
		sum = multiply_accumulate_16tx16t_add_16bx16b(sum, n4, n4);
	} while (p < end);
#else
	int16_t* p = block->data;
	int16_t* end = p + AUDIO_BLOCK_SAMPLES;

	do {
		int32_t n = *p++;
		sum += n * n;
	} while (p < end);
#endif
	analogWrite(LEDpin, (uint8_t)(sqrtf(sum / (float)AUDIO_BLOCK_SAMPLES) / 128.0));
	release(block);
}