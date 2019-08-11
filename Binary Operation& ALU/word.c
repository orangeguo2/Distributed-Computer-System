/*
 * arithmetic_unit.c
 *
 * This file implements a word of memory that is comprised
 * of a sequence of bytes. The word has a defined set of
 * characteristics including number of bytes and endian order.
 *
 * @since 2019-01-15
 * @uathor: philip gust
 */

#include <stdbool.h>
#include <stdio.h>

#include "word.h"

/**
 * Set bit of word to specified bit.
 *
 * @param word the word
 * @param bitofword the bit number to set
 * @param val the bit value
 */
void setBitOfWord(word op, unsigned bitofword, bit val) {
	unsigned byteno = bitofword >> 3;  // divide by 8
	if (wordendian == bigendian) {
		byteno = (wordbytes-1) - byteno;
	}

	unsigned bitofbyte = bitofword & 0x7;  // mod 8
	byte mask = 1 << bitofbyte; // mask for bit
	if (val == 0) {
		op[byteno] &=  ~mask;
	} else {
		op[byteno] |= mask;
	}
}

/**
 * Get bit of word.
 *
 * @param word the word containing the bit in little-endian order.
 *
 * @param the bit number to get
 * @return the bit value
 */
bit getBitOfWord(const word op, unsigned bitofword) {
	unsigned byteno = bitofword >> 3;  // divide by 8
	if (wordendian == bigendian) {
		byteno = (wordbytes-1) - byteno;
	}
	unsigned bitofbyte = bitofword & 0x7;  // mod 8
	return (op[byteno] >> bitofbyte) & 1; // extract bit
}

/**
 * Set word to word operand.
 *
 * @param result the result
 * @param op the operand
 */
void setWord(word result, const word op) {
	for (int b = 0; b < wordbytes; b++) {
		result[b] = op[b];
	}
}
