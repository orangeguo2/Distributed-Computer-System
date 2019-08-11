/*
 * assignment_1_test.c
 *
 * This file contains test for word and arithmetic_unit functions.
 *
 * @since 2019-01-15
 * @author philip gust
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "alu.h"
#include "CUnit/CUnit.h"
#include "CUnit/Basic.h"

// Compare two words for equality
#define CU_ASSERT_WORD_EQUAL(actual, expected) \
  { CU_assertImplementation((0==memcmp((actual),(expected),wordbytes)), __LINE__, ("CU_ASSERT_WORD_EQUAL(" #actual "," #expected ")"), __FILE__, "", CU_FALSE); }

// Compare two words for equality
#define CU_ASSERT_WORD_EQUAL_FATAL(actual, expected) \
  { CU_assertImplementation((0==memcmp((actual),(expected),wordbytes)), __LINE__, ("CU_ASSERT_WORD_EQUAL_FATAL(" #actual "," #expected ")"), __FILE__, "", CU_TRUE); }

// Compare two words for inequality
#define CU_ASSERT_WORD_NOT_EQUAL(actual, expected) \
  { CU_assertImplementation((0!=memcmp((actual),(expected),wordbytes)), __LINE__, ("CU_ASSERT_WORD_NOT_EQUAL(" #actual "," #expected ")"), __FILE__, "", CU_FALSE); }

// Compare two words for inequality
#define CU_ASSERT_WORD_NOT_EQUAL_FATAL(actual, expected) \
  { CU_assertImplementation((0!=memcmp((actual),(expected),wordbytes)), __LINE__, ("CU_ASSERT_WORD_NOT_EQUAL_FATAL(" #actual "," #expected ")"), __FILE__, "", CU_TRUE); }

/**
 * Test basic word functions
 */
void test_word(void) {
	word w1 = {0x81, 0x7, 0xF0, 0xF7};
	word w2;
	setWord(w2,w1);
	CU_ASSERT_WORD_EQUAL(w2, w1);

	// check that initial bits are set
	CU_ASSERT_EQUAL(getBitOfWord(w1,0), 1);
	CU_ASSERT_EQUAL(getBitOfWord(w1,8), 0);
	CU_ASSERT_EQUAL(getBitOfWord(w1,15), 1);
	CU_ASSERT_EQUAL(getBitOfWord(w1,23), 0);
	CU_ASSERT_EQUAL(getBitOfWord(w1,31), 1);

	// reverse those bits
	setBitOfWord(w1, 0, 0);
	setBitOfWord(w1, 8, 1);
	setBitOfWord(w1, 15, 0);
	setBitOfWord(w1, 23, 1);
	setBitOfWord(w1, 31, 0);

	// check that modified bits are set
	CU_ASSERT_EQUAL(getBitOfWord(w1,0), 0);
	CU_ASSERT_EQUAL(getBitOfWord(w1,8), 1);
	CU_ASSERT_EQUAL(getBitOfWord(w1,15), 0);
	CU_ASSERT_EQUAL(getBitOfWord(w1,23), 1);
	CU_ASSERT_EQUAL(getBitOfWord(w1,31), 0);

	// check that initial and modified words different
	CU_ASSERT_WORD_NOT_EQUAL(w1, w2);
}

/**
 * Test math functions (add, sub, mul, div, negative)
 */
void test_math(void) {
	word w1 = {0xff, 0xff, 0xff, 0xff};  // -1
	word w2 = {0x00, 0x00, 0x00, 0x01};  // 1
	word w3;
	addWord(w3, w1, w2);  // 1 + -1 = 0
	CU_ASSERT_WORD_EQUAL(w3, zeroWord);

	word w4;
	subWord(w4, w1, w2); // -1 - 1 = -2
	word expected4 = {0xFF, 0xFF, 0xFF, 0xFE};
	CU_ASSERT_WORD_EQUAL(w4, expected4);

	word w5;
	subWord(w5, w4, w1);  // -2 - -1 = -1
	word expected5 = {0xff, 0xff, 0xff, 0xff};
	CU_ASSERT_WORD_EQUAL(w5, expected5);

	word w6;
	mulWord(w6, w4, w4);  // -2 * -2 = 4
	word expected6 = {0x00, 0x00, 0x00, 0x04};
	CU_ASSERT_WORD_EQUAL(w6, expected6);

	word w7;
	addWord(w7, w4, w5);  // -2 + -1 = -3
	word expected7 = {0xff, 0xff, 0xff, 0xfd};
	CU_ASSERT_WORD_EQUAL(w7, expected7);

	word w8;
	negativeWord(w8, w4); // - -2 = 2
	word expected8 = {0x00, 0x00, 0x00, 0x02};
	CU_ASSERT_WORD_EQUAL(w8, expected8);

	word w9;
	word w10;
	div2Word(w9, w10, w6, w8);  // 4 / 2 = 2 r 0
	word expected9 = {0x00, 0x00, 0x00, 0x02};
	word expected10 = {0x00, 0x00, 0x00, 0x00};
	CU_ASSERT_WORD_EQUAL(w9, expected9);
	CU_ASSERT_WORD_EQUAL(w10, expected10);

	word w11;
	word w12;
	div2Word(w11, w12, w6, w4);  // 4 / -2 = -2 r 0
	word expected11 = {0xff, 0xff, 0xff, 0xfe};
	word expected12 = {0x00, 0x00, 0x00, 0x00};
	CU_ASSERT_WORD_EQUAL(w11, expected11);
	CU_ASSERT_WORD_EQUAL(w12, expected12);

	word w13;
	word w14;
	div2Word(w13, w14, w7, w4);  // -3 / -2 = 1 r -1
	word expected13 = {0x00, 0x00, 0x00, 0x01};
	word expected14 = {0xff, 0xff, 0xff, 0xff};
	CU_ASSERT_WORD_EQUAL(w13, expected13);
	CU_ASSERT_WORD_EQUAL(w14, expected14);

	word w15;
	word w16;
	div2Word(w15, w16, w7, zeroWord);  // -3 / 0 = wordMin r 0
	word expected16 = {0x00, 0x00, 0x00, 0x00};
	CU_ASSERT_WORD_EQUAL(w15, minWord);
	CU_ASSERT_WORD_EQUAL(w16, expected16);
}

/**
 * Test compare functions (lt, eq, ge)
 */
void test_compare(void) {
	word w0 = {0xff, 0xff, 0xff, 0xfe};  // -2
	word w1 = {0xff, 0xff, 0xff, 0xff};  // -1
	word w2 = {0x00, 0x00, 0x00, 0x00};  // 0
	word w3 = {0x00, 0x00, 0x00, 0x01};  // 1
	word w4 = {0x00, 0x00, 0x00, 0x02};  // 2

	// test LT
	bool ltmin = testLtWord(minWord);  // smallest word
	CU_ASSERT_TRUE(ltmin);

	bool lt0 = testLtWord(w0); // -2
	CU_ASSERT_TRUE(lt0);

	bool lt1 = testLtWord(w1);  // -1
	CU_ASSERT_TRUE(lt1);

	bool lt2 = testLtWord(w2);  // 0
	CU_ASSERT_FALSE(lt2);

	bool lt3 = testLtWord(w3);  // 1
	CU_ASSERT_FALSE(lt3);

	bool lt4 = testLtWord(w4);  // 2
	CU_ASSERT_FALSE(lt4);

	bool ltmax = testLtWord(maxWord);  // largest word
	CU_ASSERT_FALSE(ltmax);

	// test EQ
	bool eqmin = testEqWord(minWord);  // smallest word
	CU_ASSERT_FALSE(eqmin);

	bool eq0 = testEqWord(w0); // -2
	CU_ASSERT_FALSE(eq0);

	bool eq1 = testEqWord(w1);  // -1
	CU_ASSERT_FALSE(eq1);

	bool eq2 = testEqWord(w2);  // 0
	CU_ASSERT_TRUE(eq2);

	bool eq3 = testEqWord(w3);  // 1
	CU_ASSERT_FALSE(eq3);

	bool eq4 = testEqWord(w4);  // 2
	CU_ASSERT_FALSE(eq4);

	bool eqmax = testEqWord(maxWord);  // largest word
	CU_ASSERT_FALSE(eqmax);

	// test GE
	bool gemin = testGeWord(minWord);  // smallest word
	CU_ASSERT_FALSE(gemin);

	bool ge0 = testGeWord(w0); // -2
	CU_ASSERT_FALSE(ge0);

	bool ge1 = testGeWord(w1);  // -1
	CU_ASSERT_FALSE(ge1);

	bool ge2 = testGeWord(w2);  // 0
	CU_ASSERT_TRUE(ge2);

	bool ge3 = testGeWord(w3);  // 1
	CU_ASSERT_TRUE(ge3);

	bool ge4 = testGeWord(w4);  // 2
	CU_ASSERT_TRUE(ge4);

	bool gemax = testGeWord(maxWord);  // largest word
	CU_ASSERT_TRUE(gemax);

}

/**
 * Test shift functions (ash, csh, lsh)
 */
void test_shift(void) {
	word w0 = {0x00, 0x00, 0x00, 0x00};
	word w1 = {0x00, 0x00, 0xff, 0xff};
	word w2 = {0x00, 0xff, 0xff, 0x00};
	word w3 = {0xff, 0xff, 0x00, 0x00};
	word w4 = {0xff, 0x00, 0x00, 0x00};
	word w5 = {0x00, 0x00, 0x00, 0xff};
	word w6 = {0xff, 0x00, 0x00, 0xff};
	word w7 = {0x80, 0x00, 0x00, 0x00};
	word w8 = {0x7f, 0xff, 0x00, 0x00};
	word w9 = {0x7f, 0x00, 0x00, 0x00};
	word w10 = {0xff, 0xff, 0xff, 0xff};
	word w11 = {0xff, 0xff, 0xff, 0x00};

	// test ash
	word ash0;
	ashWord(ash0, w0, 0);   // no rotate
	CU_ASSERT_WORD_EQUAL(ash0, w0);

	word ash1;
	ashWord(ash1, w1, wordsize/2); // preserve 0 sign bit
	CU_ASSERT_WORD_EQUAL(ash1, w8);

	word ash2;
	ashWord(ash2, w2, wordsize/2); // preserve 0 sign bit
	CU_ASSERT_WORD_EQUAL(ash2, w9);

	word ash3;
	ashWord(ash3, w8, wordsize/2); // preserve 0 sign bit
	CU_ASSERT_WORD_EQUAL(ash3, w0);

	word ash4;
	ashWord(ash4, w1, -wordsize/2); // preserve 0 sign bit
	CU_ASSERT_WORD_EQUAL(ash4, w0);

	word ash5;
	ashWord(ash5, w2, -wordsize/2); // preserve 0 sign bit
	CU_ASSERT_WORD_EQUAL(ash5, w5);

	word ash6;
	ashWord(ash6, w3, wordsize/2); // preserve 0 sign bit
	CU_ASSERT_WORD_EQUAL(ash6, w7);

	word ash7;
	ashWord(ash7, w10, wordsize); // preserve 1 sign bit
	CU_ASSERT_WORD_EQUAL(ash7, w7);

	word ash8;
	ashWord(ash8, w3, wordsize/2); // preserve 1 sign bit
	CU_ASSERT_WORD_EQUAL(ash7, w7);

	word ash9; //??
	ashWord(ash9, w3, -wordsize/2); // preserve 1 sign bit
	CU_ASSERT_WORD_EQUAL(ash9, w10);

	word ash10; //??
	ashWord(ash10, w4, -wordsize/2); // preserve 1 sign bit
	CU_ASSERT_WORD_EQUAL(ash10, w11);

	word ash11; //??
	ashWord(ash11, w4, -wordsize/2); // preserve 1 sign bit
	CU_ASSERT_WORD_EQUAL(ash11, w11);

	word ash12; //??
	ashWord(ash12, w7, -wordsize); // preserve 1 sign bit
	CU_ASSERT_WORD_EQUAL(ash12, w10);

	// test csh
	word csh0;
	cshWord(csh0, w0, 0);
	CU_ASSERT_WORD_EQUAL(csh0, w0);

	word csh1;
	cshWord(csh1, w1, wordsize/2);
	CU_ASSERT_WORD_EQUAL(csh1, w3);

	word csh2;
	cshWord(csh2, w1, -wordsize/2);
	CU_ASSERT_WORD_EQUAL(csh2, w3);

	word csh3;
	cshWord(csh3, w1, wordsize);
	CU_ASSERT_WORD_EQUAL(csh3, w1);

	word csh4;
	cshWord(csh4, w1, -wordsize);
	CU_ASSERT_WORD_EQUAL(csh4, w1);


	word csh5;
	cshWord(csh5, w2, 0);
	CU_ASSERT_WORD_EQUAL(csh5, w2);

	word csh6;
	cshWord(csh6, w2, wordsize/2);
	CU_ASSERT_WORD_EQUAL(csh6, w6);

	word csh7;
	cshWord(csh7, w2, wordsize/2);
	CU_ASSERT_WORD_EQUAL(csh7, w6);

	word csh8;
	cshWord(csh8, w2, wordsize);
	CU_ASSERT_WORD_EQUAL(csh8, w2);

	word csh9;
	cshWord(csh9, w2, wordsize);
	CU_ASSERT_WORD_EQUAL(csh9, w2);

	// test lsh
	word lsh0;
	lshWord(lsh0, w0, 0);
	CU_ASSERT_WORD_EQUAL(lsh0, w0);

	word lsh1;
	lshWord(lsh1, w0, wordsize/2);
	CU_ASSERT_WORD_EQUAL(lsh1, w0);

	word lsh2;
	lshWord(lsh2, w0, -wordsize/2);
	CU_ASSERT_WORD_EQUAL(lsh2, w0);

	word lsh3;
	lshWord(lsh3, w1, 0);
	CU_ASSERT_WORD_EQUAL(lsh3, w1);

	word lsh4;
	lshWord(lsh4, w1, wordsize/2);
	CU_ASSERT_WORD_EQUAL(lsh4, w3);

	word lsh5;
	lshWord(lsh5, w1, -wordsize/2);
	CU_ASSERT_WORD_EQUAL(lsh5, w0);

	word lsh6;
	lshWord(lsh6, w2, 0);
	CU_ASSERT_WORD_EQUAL(lsh6, w2);

	word lsh7;
	lshWord(lsh7, w2, wordsize/2);
	CU_ASSERT_WORD_EQUAL(lsh7, w4);

	word lsh8;
	lshWord(lsh8, w2, -wordsize/2);
	CU_ASSERT_WORD_EQUAL(lsh8, w5);
}

/**
 * Test logical functions (and, or, not, xor, mask)
 */
void test_logical(void) {
	word w0 = {0x00, 0x00, 0x00, 0x00};
	word w1 = {0x00, 0x00, 0xff, 0xff};
	word w2 = {0x00, 0xff, 0xff, 0x00};
	word w3 = {0xff, 0xff, 0x00, 0x00};
	word w4 = {0xff, 0x00, 0x00, 0x00};
	word w5 = {0x00, 0x00, 0x00, 0xff};
	word w6 = {0xff, 0x00, 0x00, 0xff};
	word w7 = {0x80, 0x00, 0x00, 0x00};
	word w8 = {0x7f, 0xff, 0x00, 0x00};
	word w9 = {0x7f, 0x00, 0x00, 0x00};
	word w10 = {0xff, 0xff, 0xff, 0xff};
	word w11 = {0xff, 0xff, 0xff, 0x00};
	word w12 = {0x00, 0x00, 0xff, 0x00};

	// test or
	word or1;
	orWord(or1, w0, w1);
	CU_ASSERT_WORD_EQUAL(or1, w1);

	word or2;
	orWord(or2, w1, w3);
	CU_ASSERT_WORD_EQUAL(or2, w10);

	word or3;
	orWord(or3, w1, w11);
	CU_ASSERT_WORD_EQUAL(or3, w10);

	word or4;
	orWord(or4, w7, w8);
	CU_ASSERT_WORD_EQUAL(or4, w3);

	// test and
	word and1;
	andWord(and1, w0, w1);
	CU_ASSERT_WORD_EQUAL(and1, w0);

	word and2;
	andWord(and2, w1, w3);
	CU_ASSERT_WORD_EQUAL(and2, w0);

	word and3;
	andWord(and3, w1, w11);
	CU_ASSERT_WORD_EQUAL(and3, w12);

	word and4;
	andWord(and4, w10, w10);
	CU_ASSERT_WORD_EQUAL(and4, w10);

	// test xor
	word xor1;
	xorWord(xor1, w6, w2);
	CU_ASSERT_WORD_EQUAL(xor1, w10);

	word xor2;
	xorWord(xor2, w10, w11);
	CU_ASSERT_WORD_EQUAL(xor2, w5);

	word xor3;
	xorWord(xor3, w7, w9);
	CU_ASSERT_WORD_EQUAL(xor3, w4);

	word xor4;
	xorWord(xor4, w10, w10);
	CU_ASSERT_WORD_EQUAL(xor4, w0);

	// test not
	word not1;
	notWord(not1, w1);
	CU_ASSERT_WORD_EQUAL(not1, w3);

	word not2;
	notWord(not2, w0);
	CU_ASSERT_WORD_EQUAL(not2, w10);

	word not3;
	notWord(not3, w5);
	CU_ASSERT_WORD_EQUAL(not3, w11);

	word not4;
	notWord(not4, w2);
	CU_ASSERT_WORD_EQUAL(not4, w6);

	// test mask
	word mask1;
	maskWord(mask1, w10, wordsize/2);
	CU_ASSERT_WORD_EQUAL(mask1, w3);

	word mask2;
	maskWord(mask2, w10, wordsize);
	CU_ASSERT_WORD_EQUAL(mask2, w0);

	word mask3;
	maskWord(mask3, w10, wordsize/4);
	CU_ASSERT_WORD_EQUAL(mask3, w11);

	word mask4;
	maskWord(mask4, w10, 0);
	CU_ASSERT_WORD_EQUAL(mask4, w10);

	word mask5;
	maskWord(mask5, w10, -wordsize/2);
	CU_ASSERT_WORD_EQUAL(mask5, w1);

	word mask6;
	maskWord(mask6, w10, -wordsize);
	CU_ASSERT_WORD_EQUAL(mask6, w10);

	word mask7;
	maskWord(mask7, w10, -wordsize/2);
	CU_ASSERT_WORD_EQUAL(mask7, w1);
}
/**
 * Test all the functions for this application.
 *
 * @return test error code
 */
static CU_ErrorCode test_all() {
	// initialize the CUnit test registry -- only once per application
	CU_initialize_registry();

	// add a suite to the registry with no init or cleanup
	CU_pSuite pSuite = CU_add_suite("word_tests", NULL, NULL);

	// add the tests to the suite
	CU_add_test(pSuite, "test_word", test_word);
	CU_add_test(pSuite, "test_math", test_math);
	CU_add_test(pSuite, "test_compare", test_compare);
	CU_add_test(pSuite, "test_shift", test_shift);
	CU_add_test(pSuite, "test_logical", test_logical);

	// run all test suites using the basic interface
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();

	// display information on failures that occurred
	CU_basic_show_failures(CU_get_failure_list());

	// Clean up registry and return status
	CU_cleanup_registry();
	return CU_get_error();
}

/**
 * Main program to invoke test functions
 *
 * @return the exit status of the program
 */
int main(void) {
	// test all the functions
	CU_ErrorCode code = test_all();

	return (code == CUE_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}


