#include "textmodememory.h"

byte* BASE_TEXTBUFFER_ADDRESS = (byte*)BASE_TEXT_MODE_ADDRESS;

void clear_base_text_mem(uint32_t color) {
    for(size_t byteIndex = 0; byteIndex < 80 * 25 * 2; byteIndex += 2)
    {
        BASE_TEXTBUFFER_ADDRESS[byteIndex] = ' ';
        BASE_TEXTBUFFER_ADDRESS[byteIndex + 1] = color;
    }
}

void write_word_base_text_mem(short line, short column, const char* word, uint32_t color) {
    column *= 2;
	size_t word_len = strlen(word);
	size_t first_byte_pos = (line * 80 * 2) + column;
	for (size_t byteIndex = 0, wordIndex = 0; wordIndex < word_len; byteIndex += 2, ++wordIndex)
	{
		BASE_TEXTBUFFER_ADDRESS[first_byte_pos + byteIndex] = word[wordIndex];
		BASE_TEXTBUFFER_ADDRESS[first_byte_pos + (byteIndex + 1)] = color;
	}
}
