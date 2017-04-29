/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * DUONG NGUYEN - p20150003
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* 
 * bitNor - ~(x|y) using only ~ and & 
 *   Example: bitNor(0x6, 0x5) = 0xFFFFFFF8
 *   Legal ops: ~ &
 *   Max ops: 8
 *   Rating: 1
 */
int bitNor(int x, int y) {
	/*Evaluate ~(x|y) by applying De Morgan's law: ~(x|y) = ~x & ~y*/
	return (~x & ~y);
}

/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int copyLSB(int x) {
	/* exploit ability of shifts to replace all bits by the least significant one */
	return ((x << 31) >> 31);
}

/* 
 * isEqual - return 1 if x == y, and 0 otherwise 
 *   Examples: isEqual(5,5) = 1, isEqual(4,5) = 0
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int isEqual(int x, int y) {
	/*Check whether x == y using the fact that x == y iff x XOR y == 0*/
	return (!(x^y));
}

/* 
 * bitMask - Generate a mask consisting of all 1's 
 *   lowbit and highbit
 *   Examples: bitMask(5,3) = 0x38
 *   Assume 0 <= lowbit <= 31, and 0 <= highbit <= 31
 *   If lowbit > highbit, then mask should be all 0's
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int bitMask(int highbit, int lowbit) {
	/* exploit ability of shifts and boolean algebra 
	 * high is in the form of 000..000111..111 with highbit + 1 number of 1
	 * low is in the form of 111..111000..000 with lowbit number of 0
	 * */
	int high = ~((~0 << highbit ) << 1);
	int low = (~0 << lowbit);
	return high & low;
}

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
	/* apply divide and conquer tachnique to add the sum of 1's bits in each byte in parallel
	 * then add the sums together*/
	int mask1 = (0x55 << 24);
	int mask2 = (0x33 << 24);
	int mask3 = (0xf << 24);
	int sum1, sum2, sum3, sum4;
	
	mask1 |= (0x55 << 16);
	mask1 |= (0x55 << 8);
	mask1 |= 0x55;
	
	mask2 |= (0x33 << 16);
	mask2 |= (0x33 << 8);
	mask2 |= 0x33;
	
	mask3 |= (0xf << 16);
	mask3 |= (0xf << 8);
	mask3 |= 0xf;
	
	//Calculate total sum of 1's bits of each byte in parallel
	x = (x & mask1) + ((x >> 1) & mask1);
	x = (x & mask2) + ((x >> 2) & mask2);
	x = (x & mask3) + ((x >> 4) & mask3);
	
	//Sums of 1's bits in each byte
	sum1 = (x >> 24);
	sum2 = (x >> 16) & 0xff;
	sum3 = (x >> 8) & 0xff;
	sum4 = x & 0xff;

  	return sum1 + sum2 + sum3 + sum4;
}

/* 
 * TMax - return maximum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmax(void) {
	/* The maximum two's complement integer is 0xEFFFFFFF*/
	return ~(1 << 31);
}

/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
int isNonNegative(int x) {
	/* x is NonNegative iff the first bit is 0*/
	return !(x >> 31);
}

/* 
 * addOK - Determine if can compute x+y without overflow
 *   Example: addOK(0x80000000,0x80000000) = 0,
 *            addOK(0x80000000,0x70000000) = 1, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int addOK(int x, int y) {
	/* overflow occurs when the sign bits of x and y are the same,
	 * and different from the sign bit of x+y */
	int sign_x = x >> 31;
	int sign_y = y >> 31;
	int sign_xy = (x + y) >> 31;
	return !!(sign_x ^ sign_y) | !(sign_x ^ sign_xy);
}

/* 
 * rempwr2 - Compute x%(2^n), for 0 <= n <= 30
 *   Negative arguments should yield negative remainders
 *   Examples: rempwr2(15,2) = 3, rempwr2(-35,3) = -3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int rempwr2(int x, int n) {
	/* x & (2^n) 	= the n least significant bits in x if x >=0 or x % (2^n) == 0
	 *		= the n least significant bits in x - (2^n) otherwise 
	 * Let x & (2^n) be in the form of bbb...bba(n-1)a(n-2)..a1a0*/
	int b = (x >> 31) << n;
	int a = ~(~0 << n) & x;
	int c = ~(((!a) << 31) >> 31); // handle the case when a = 0
	b = b & c;
    	return b + a;
}

/* 
 * isLess - if x < y  then return 1, else return 0 
 *   Example: isLess(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLess(int x, int y) {
	/* check whether x - y < 0 
	 * and when overflow occurs*/
	int diff = x + (~y + 1);
	int sign_x = x >> 31;
	int sign_y = y >> 31;
  	return (!!sign_x & !sign_y) | ((!!(diff >> 31)) & (!!sign_x | !sign_y));
}

/* 
 * absVal - absolute value of x
 *   Example: absVal(-1) = 1.
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 10
 *   Rating: 4
 */
int absVal(int x) {
	/* |x| = x if x >= 0, }x| = ~x+1 if x < 0*/
	int shift = x ^ (x >> 31); // flip x if x < 0
	int plus = (x >> 31) & 1; // add 1 if x < 0. add 0 otherwise
  	return shift + plus;
}

/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int isPower2(int x) {
	/* check whether x !=0, x >= 0 and x is in the form of 0..010..0*/
	return !(!x) & !(!(~(x >> 31))) & !(x & (x + ~0));
}

/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
	/* uf is NaN iff exp = 11..11 and frac !=00..000
	 * if uf is NaN, return uf
	 * else flip the first bit of uf*/
	if (((uf << 1) >> 24 == 0xff) && ((uf >> 23) << 23 != uf))
		return uf;
	return (1 << 31) ^ uf;
}

/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
	/* if exp == 111...1111, return uf
	 * if exp is from 00..10 to 1111....10, exp-1
	 * if exp = 00..01, exp - 1 and frac = 1,frac / 2, frac is rounded to the nearest even
	 * if exp = 00..00, frac >> 1, frac is rounded in the nearest even
	 * */

	unsigned s = (uf >> 31) << 31;
	unsigned exp = (uf << 1) >> 24;
	unsigned frac = (uf << 9) >> 9;
	
	if (exp == 0xff)
		return uf;
	else if (exp != 1 && exp !=0)
		exp -=1;
	else{
		//Round to the nearest even number
		if ((frac & 3) == 3)
			frac = frac + 1;
		frac = frac >> 1;

		if (exp == 1){
			exp = 0;
			return s | (exp << 23) | (1 << 22) | frac;
		}
	}
	return s | (exp << 23) | frac;
}

/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
	/* Factorize x to be in the form of (-1)^s * 2^(exp-127) * 1,frac*/

	int s = (x & (1 << 31));
	int exp = 158;
	int add = 0;
	
	//Flip negative number
	if ((x >> 31) != 0)
		x = ~x+1;
	
	if (x == 0)
		return 0;
	
	//Find exp
	while ((x >> 31) == 0){
		exp -= 1;
		x = x << 1;
	}

	//Rounding
	if ((x & 0x80) != 0){
		if ((x & 0x7f) != 0)
			add = 1;
		else if (((x >> 8) & 1) != 0)
			add = 1;
	}
	
	//Find the fraction
	x = ((x ^ (1 << 31)) >> 8) + add;
	
	//Fraction overflow
	if (x == (1 << 23)){
		x = 0;
		exp += 1;
	}

  	return s | (exp << 23) | x;
}
