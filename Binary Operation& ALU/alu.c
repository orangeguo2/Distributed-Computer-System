/*
 * alu.c
 *
 * This file implements functions performed by an arithmetic logic
 * unit of a computer on a sequence of bytes that comprise a word.
 *
 * @since 2019-01-15
 * @author philip gust
 */
#include <stdlib.h>

#include "alu.h"

/**
 * Returns true if word is less than zero.
 *
 * Examples (little-endian):
 *   testLtWord(0001 0000 0000 0000) -> false
 *   testLtWord(0000 1111 1111 1111) -> true
 */
bool testLtWord(const word op) {
	// your code here
	//
	// NOTES:
	// A word is negative only if its sign bit
	// (wordtopbit) is set.
	if (getBitOfWord(op, wordtopbit)) {
		return true;
	}

	return false;
}

/**
 * Returns true if word is greater than zero.
 *
 * Examples (little-endian):
 *   testGeWord(0000 0000 0000 0000) -> true
 *   testGeWord(0001 0000 0000 0000) -> true
 *   testGeWord(0000 1111 1111 1111) -> false
 */
bool testGeWord(const word op) {
	// your code here
	//
	// NOTES:
	// This is the inverse of the LT condition.
	if (!testLtWord(op))
		return true;
	return false;
}

/**
 * Returns true if word is zero.
 *
 * Examples:
 *   testLtWord(0000 0000 0000 0000) -> true
 *   testLtWord(0000 1111 1111 1111) -> false
 */
bool testEqWord(const word op) {
	// your code here
	//
	// NOTES:
	// Return true if all bits of word are 0
	for (int b = 0; b <= wordtopbit; b++) {
		if (getBitOfWord(op, b)) {
			return false;
		}
	}
	return true;
}

/**
 * Arithmetic shift word by count. Same as multiplying or
 * dividing by power of 2.  Shifts bits in word left (+) or
 * right (-) by the specified count. Fills in with 0 from
 * the right, and the sign bit from the left.

 *
 * Examples:
 * 	ash(1010 1011 1111 1111, 5)  -> 1111 1111 1110 0000
 * 	ash(1111 1111 0000 0000, -5) -> 1111 1111 1111 1000
 *
 * @param op the operand
 * @param count the shift count
 */
void ashWord(word result, const word op, int count) {
	int c = abs(count);
	if (c >= wordsize) {
		c = wordsize - 1;
	}

	// local copy in case op and result overlap
	word localop;
	setWord(localop, op);

	uint8_t sign = getBitOfWord(localop, wordtopbit);
	if (count < 0) {
		// move upper bits of word right
		for (int b = wordtopbit - 1; b >= c; b--) {
			bit t = getBitOfWord(localop, b);
			setBitOfWord(result, b - c, t);
		}
		// clear upper bits of word to sign bit
		for (int b = c; b >= 1; b--) {
			setBitOfWord(result, wordtopbit - b, sign);
		}
	} else {
		// move lower bits of word left
		for (int b = wordtopbit - 1; b >= c; b--) {
			bit t = getBitOfWord(localop, b - c);
			setBitOfWord(result, b, t);
		}
		// clear lower bits of word
		for (int b = c - 1; b >= 0; b--) {
			setBitOfWord(result, b, 0);
		}
	}
	// set top bit of word
	setBitOfWord(result, wordtopbit, sign);
}

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
void cshWord(word result, const word op, int count) {
	word localop;
	setWord(localop, op);

	int c = abs(count);


	if (count == 0) {
		setWord(result, localop);
		return;
	}
	if (count > 0) {
		for (int b = wordtopbit; b >= c; b--) {
			bit t = getBitOfWord(localop, b - c);
			setBitOfWord(result, b, t);
		}
		for (int b = c - 1, i=0; b >= 0; b--,i++) {
			bit t = getBitOfWord(localop, wordtopbit-i);
			setBitOfWord(result, b, t);
		}
	} else {
		for (int b = wordtopbit; b >= c; b--) {
			bit t = getBitOfWord(localop, b);
			setBitOfWord(result, b - c, t);
		}
		for (int b = c - 1,i=0; b >= 0; b--,i++) {
			bit t = getBitOfWord(localop, b);
			setBitOfWord(result, wordtopbit - i, t);
		}
	}

}

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
void lshWord(word result, const word op, int count) {
	word localop;
	setWord(localop, op);

	int c = abs(count);
	if (c >= wordsize) {
		c = wordsize - 1;
	}

	if (count == 0) {
		setWord(result, localop);
		return;
	}
	if (count > 0) {
		for (int b = wordtopbit; b >= c; b--) {
			bit t = getBitOfWord(localop, b - c);
			setBitOfWord(result, b, t);
		}
		for (int b = c - 1; b >= 0; b--) {
			setBitOfWord(result, b, 0);
		}
	} else {
		for (int b = wordtopbit; b >= c; b--) {
			bit t = getBitOfWord(localop, b);
			setBitOfWord(result, b - c, t);
		}
		for (int b = c - 1; b >= 0; b--) {
			setBitOfWord(result, wordtopbit - b, 0);
		}
	}

}

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
void maskWord(word result, const word op, int count) {
	int c = abs(count);
	if (c > wordsize) {
		c = wordsize;
	}

	if (count < 0) {
		// copy lower bits of word
		for (int b = 0; b < c; b++) {
			bit t = getBitOfWord(op, b);
			setBitOfWord(result, b, t);
		}

		// clear upper bits of word
		for (int b = c; b <= wordtopbit; b++) {
			setBitOfWord(result, b, 0);
		}
	} else {
		// copy upper bits of word
		for (int b = c; b <= wordtopbit; b++) {
			bit t = getBitOfWord(op, b);
			setBitOfWord(result, b, t);
		}

		// clear lower bits of word
		for (int b = 0; b < count; b++) {
			setBitOfWord(result, b, 0);
		}
	}
}

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
void andWord(word result, const word op1, const word op2) {
	// your code here
	//
	// NOTES:
	// Use bit-wise AND operator (&) on each bit of op1
	// and op2 to set each bit of the result.

	//setWord(result, zeroWord);  // delete this line
	for (uint8_t b = 0; b <= wordtopbit; b++) {
		bit setBit = getBitOfWord(op1, b) & getBitOfWord(op2, b);
		setBitOfWord(result, b, setBit);
	}
}

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
void orWord(word result, const word op1, const word op2) {
	// your code here
	//
	// NOTES:
	// Use bit-wise OR operator (|) on each bit of op1
	// and op2 to set each bit of the result.

	//setWord(result, zeroWord);  // delete this line
	for (uint8_t b = 0; b <= wordtopbit; b++) {
		bit setBit = getBitOfWord(op1, b) | getBitOfWord(op2, b);
		setBitOfWord(result, b, setBit);
	}
}

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
void xorWord(word result, const word op1, const word op2) {
	// your code here
	//
	// NOTES:
	// Use bit-wise XOR operator (^) on each bit of op1
	// and op2 to set each bit of the result.

	for (uint8_t b = 0; b <= wordtopbit; b++) {
		bit setBit = getBitOfWord(op1, b) ^ getBitOfWord(op2, b);
		setBitOfWord(result, b, setBit);
	}
}

/**
 * Logical NOT of word operand.
 *
 * Examples:
 * 	not(0011 0000 0000 0000) -> 1100 1111 1111 1111
 *
 * @param result the result
 * @param op the operand
 */
void notWord(word result, const word op) {
	for (int b = 0; b <= wordtopbit; b++) {
		setBitOfWord(result, b, notBit(getBitOfWord(op, b)));
	}
}

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
void negativeWord(word result, const word op) {
	bit carry = 1;
	for (int b = 0; b <= wordtopbit; b++) {
		uint8_t r = notBit(getBitOfWord(op,b)) + carry;
		carry = r >> 1;
		setBitOfWord(result, b, toBit(r));
	}
}

/**
 * Sum of two word operands.
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void addWord(word result, const word op1, const word op2) {

	bit carry = 0;
	for (uint8_t b = 0; b <= wordtopbit; b++) {
		uint8_t singleSum = getBitOfWord(op1, b) + getBitOfWord(op2, b) + carry;
		carry = singleSum >> 1;
		setBitOfWord(result, b, toBit(singleSum));
	}

}

/**
 * Difference of two word operands.
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void subWord(word result, const word op1, const word op2) {
	bit carry = 1;
	for (uint8_t b = 0; b <= wordtopbit; b++) {
		uint8_t t = getBitOfWord(op1, b) + notBit(getBitOfWord(op2, b)) + carry;
		carry = t >> 1;
		setBitOfWord(result, b, toBit(t));
	}
}

/**
 * Product of two word operands.
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void mulWord(word result, const word op1, const word op2) {

	word local1, local2;
	setWord(local1, op1);
	setWord(local2, op2);

	setWord(result, zeroWord);

	bit sign1 = getBitOfWord(op1, wordtopbit);
	bit sign2 = getBitOfWord(op2, wordtopbit);
	bit sign = sign1 ^ sign2;

	if (testLtWord(local1)) {
		negativeWord(local1, op1);
	}
	if (testLtWord(local2)) {
		negativeWord(local2, op2);
	}

	for (uint8_t b = 0; b <= wordtopbit - 1; b++) {
		if (getBitOfWord(local2, b)) {
			word resultTemp;
			addWord(resultTemp, result, local1);
			setWord(result, resultTemp);
		}
		//lshWord(word result, const word op, int count);
		lshWord(local1, local1, 1);
	}
	setBitOfWord(result, wordtopbit, sign);
}

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
void div2Word(word result, word remainder, const word op1, const word op2) {
	setWord(result, zeroWord);
	setWord(remainder, zeroWord);

	if (testEqWord(op2)) {
		// handle divide by 0 by returning largest
		// positive or negative number
		setWord(result, (testGeWord(op1) ? maxWord : minWord));
	} else {
		word w1, w2;
		bool resultNegative = false;
		// operands must be positive
		if (testLtWord(op1)) {
			negativeWord(w1, op1);
			resultNegative = !resultNegative;
		} else {
			setWord(w1, op1);
		}
		if (testLtWord(op2)) {
			negativeWord(w2, op2);
			resultNegative = !resultNegative;
		} else {
			setWord(w2, op2);
		}

		//
		for (int b = wordtopbit; b >= 0; b--) {
			lshWord(result, result, 1);	// shift result for next bit
			lshWord(remainder, remainder, 1);	// position remainder
			bit t = getBitOfWord(w1, b);	// bring down next bit
			setBitOfWord(remainder, 0, t);

			word test;
			subWord(test, remainder, w2);  // do trial subtract
			if (testGeWord(test)) {	// division successful if still positive
				setBitOfWord(result, 0, 1);	// shift bit into result
				setWord(remainder, test);   // update remainder
			}
		}

		if (resultNegative) {	// set correct sign of result
			negativeWord(result, result);
		}

		if (testLtWord(op1)) { // remainder negative if op1 is negative
			negativeWord(remainder, remainder);
		}
	}
}

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
void divWord(word result, const word op1, const word op2) {
	word remainder;
	div2Word(result, remainder, op1, op2);
}

/**
 * Remainder of two word operands.
 * The sign of the remainder matches the sign of the dividend.
 *
 * @param result the result
 * @param op1 the first operand
 * @param op2 the second operand
 */
void remainderWord(word result, const word op1, const word op2) {
	word quotient;
	div2Word(quotient, result, op1, op2);
}
