/*
 * word.h
 *
 * This file implements a word of memory that is comprised
 * of a sequence of bytes. The word has a defined set of
 * characteristics including number of bytes and endian order.
 *
 * @since 2019-01-15
 * @uathor: philip gust
 */

#include <stdbool.h>
#include <stdint.h>

/** number of bits in a word */
static const int wordsize = 32;

/** top bit of a word */
static const int wordtopbit = 31; // wordsize-1;]

/** number of bytes in a word */
static const int wordbytes = 4; // wordsize>>3;

/** Definition of a bit */
typedef bool bit;

/** Get lowest bit value */
#define toBit(v) ((v)&1)

/** Negate bit value */
#define notBit(v) (1-toBit(v))

/** definition of a byte of memory */
typedef uint8_t byte;

/** definition of a word as a sequence of wordbytes bytes */
typedef byte word[4];  // wordbytes

/** definition of endian designators */
typedef enum endian { bigendian, littleendian} endian;

/** endian configuration of word  */
static const endian wordendian = bigendian;

/** word representing 0 */
static const word zeroWord = {0x0, 0x0, 0x0, 0x0};

/** word representing largest positive word -- modify if endian changes */
static const word maxWord = {0x7F, 0xFF, 0xFF, 0xFF};	// big-endian

/** word representing largest negative word - modify if endian changes */
static const word minWord = {0x80, 0x0, 0x0, 0x0};	// big-endian

/**
 * Set bit of word to specified bit.
 *
 * @param word the word
 * @param bitofword the bit number to set
 * @param val the bit value
 */
void setBitOfWord(word op, unsigned bitofword, bit bit);

/**
 * Get bit of word.
 *
 * @param word the word containing the bit in little-endian order.
 *
 * @param the bit number to get
 * @return the bit value
 */
bit getBitOfWord(const word op, unsigned bitofword);

/**
 * Set word to word operand.
 *
 * @param result the result
 * @param op the operand
 */
void setWord(word result, const word op);

