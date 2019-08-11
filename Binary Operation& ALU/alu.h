/*
 * alu.h
 *
 * This file implements functions performed by an arithmetic logic
 * unit of a computer on a sequence of bytes that comprise a word.
 *
 * @since 2019-01-15
 * @author philip gust
 */
#include <stdbool.h>
#include "word.h"

/**
 * Returns true if word is less than zero.
 *
 * Examples (little-endian):
 *   testLtWord(0001 0000 0000 0000) -> false
 *   testLtWord(0000 1111 1111 1111) -> true
 */
bool testLtWord(const word op);

/**
 * Returns true if word is less than zero.
 *
 * Examples (little-endian):
 *   testGeWord(0000 0000 0000 0000) -> true
 *   testGeWord(0001 0000 0000 0000) -> true
 *   testGeWord(0000 1111 1111 1111) -> false
 */
bool testGeWord(const word op);

/**
 * Returns true if word is zero.
 *
 * Examples:
 *   testLtWord(0000 0000 0000 0000) -> true
 *   testLtWord(0000 1111 1111 1111) -> false
 */
bool testEqWord(const word op);

/**
 * Arithmetic shift word by count. Same as multiplying or
 * dividing by power of 2.  Shifts bits in word left (+) or
 * right (-) by the specified count. Fills in with 0 from
 * the right, and the sign bit from the left.
 *
 * Examples:
 * 	ash(1010 1011 1111 1111, 5)  -> 1111 1111 1110 0000
 * 	ash(1111 1111 0000 0000, -5) -> 1111 1111 1111 1000
 * 	ash(1111 1111 0000 0000, -5) -> 1111 1111 1111 1000
 *
 * @param op the operand
 * @param count the shift count
 */
void ashWord(word result, const word op, int count);

/**
 * Circular shift word by count. Shifts bits in word
 * left (+) or right (-) by the specified count. Bits
 * shifted off either end of word are rotated in to
 * the other end of word.
 *
 * Examples:
 * 	csh(1010 1011 1111 1111, 4)  -> 1011 1111 1111 1010
 * 	csh(1010 1011 1111 1111, -4) -> 1111 1010 1011 1111
 *
 * @param op the operand
 * @param count the shift count
 */
void cshWord(word result, const word op, int count);

/**
 * Logical shift word by count. Shifts bits in word
 * left (+) or right (-) by the specified count. Shifts
 * bits off end and fills in with 0 in either direction.
 *
 * Examples:
 * 	lsh(1111 1111 1111 1111, 5)  -> 1111 1111 1110 0000
 * 	lsh(1111 1111 1111 1111, -5) -> 0000 0111 1111 1111
 *
 * @param op the operand
 * @param count the shift count
 */
void lshWord(word result, const word op, int count);

/**
 * Mask all but lower (+) or upper (-) count bits
 * of word operand.
 *
 * Examples:
 * 	mask(1010 1011 1111 1111, 5)  -> 0000 0000 0001 1111
 * 	mask(1010 1011 1111 1111, -5) -> 1010 1000 0000 0000
 *
 * @param result the result
 * @param op the operand
 * @param count the mask count
 */
void maskWord(word result, const word op, int count);

/**
 * Logical AND of two word operands.
 *
 * Examples:
 *   and(1111 0000 0110 1001, 1111 1111 0000 0000) -> 1111 0000 0000 0000
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void andWord(word result, const word op1, const word op2);

/**
 * Logical OR of two word operands.
 *
 * Examples:
 *   and(1111 0000 0110 1001, 0000 1111 0000 0000) -> 1111 1111 0110 1001
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void orWord(word result, const word op1, const word op2);

/**
 * Logical XOR of two word operands.
 *
 * Examples:
 *   and(1111 0000 0110 1001, 1111 1111 1111 1111) -> 0000 1111 1001 0110
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void xorWord(word result, const word op1, const word op2);

/**
 * Logical NOT of word operand.
 *
 * Examples:
 * 	not(0011 0000 0000 0000) -> 1100 1111 1111 1111
 *
 * @param result the result
 * @param op the operand
 */
void notWord(word result, const word op);

/**
 * Negative of word operand.
 *
 * Examples:
 * 	negative(0011 0000 0000 0000) -> 1101 1111 1111 1111
 * 	negative(1111 1111 1111 1111) -> 0001 0000 0000 0000
 *
 * @param result the result
 * @param op the operand
 */
void negativeWord(word result, const word op);

/**
 * Sum of two word operands.
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void addWord(word result, const word op1, const word op2);

/**
 * Difference of two word operands.
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void subWord(word result, const word op1, const word op2);

/**
 * Product of two word operands.
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void mulWord(word result, const word op1, const word op2);

/**
 * Quotient of two word operands also returning remainder.
 * The sign of the quotient is positive if the signs of
 * the operands match, and negative if they do not.
 * The sign of the remainder matches the sign of the dividend.
 *
 * @param result the result
 * @param remainder the remainder
 * @param op1 the first operand
 * @param op2 the second operand
 */
void div2Word(word result, word remainder, const word op1, const word op2);

/**
 * Quotient of two word operands.
 * The sign of the quotient is positive if the signs of
 * the operands match, and negative if they do not.
 *
 * @param result the result
 * @param remainder the remainder
 * @param op1 the first operand
 * @param op2 the second operand
 */
void divWord(word result, const word op1, const word op2);

/**
 * Remainder of two word operands.
 * The sign of the remainder matches the sign of the dividend.
 *
 * @param result the result
 * @param remainder the remainder
 * @param op1 the first operand
 * @param op2 the second operand
 */
void remainderWord(word result, const word op1, const word op2);
