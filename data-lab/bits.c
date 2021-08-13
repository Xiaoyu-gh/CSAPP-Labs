/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
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
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


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

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

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
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
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
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    // "not all 1" AND "not all 0"
    return (~(x & y)) & (~(~x & ~y));
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    // exploit rule that Tmax + 1 = Tmin
    // ~Tmax ^ Tmin = 0
    // so !(~x ^ (x + 1)) judges this
    // while -1(1...1) has same property
    // so !!(x + 1) exclude it
    // Crucial: ! transfer any size int to 0 or 1 
    // so !! normolazied x+1 to 0 or 1
    return !(~x ^ (x + 1)) & !!(x + 1);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    // create a bit pattern "mask": 0xaaaaaaaa(1010...10)
    // whose odd bits is 1, even bits is 0
    // if x & mask == mask, then return true
    // but large const is ilegal,
    // so use 0xaa to construct mask
    int a = 0xaa;
    int b = a + (a << 8);
    int mask = b + (b << 16);
    return !((x & mask) ^ mask);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    int lastHex = x ^ 0x30;
    int flag = lastHex + 6; // 9 + 6 = 15, if lastHex is > 9, it will overflow 
    flag >>= 4; // to check if there is a overflow or not begin with 0x3...
    return !flag & !(x >> 6);
    // !(x >> 6) is to exculde 0xffffff..
    // if x is 0x3.., x>>6 should be 0
    // otherwise, false
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    // transfer x to 00..0(if x=0), or 11...1(if x!=0)
    x = !! x; // turn x into 0 or 1
    x = ~x + 1; // negative
    
    return (x & y) | (~x & z); // x is like a mask
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    // make judgment according to the sign of
    // x, y and y-x
    // to deal with overflow of y-x which cause sign changed
    // use logic expression to exclude that situation

    int xsign = (x >> 31) & 1;
    int ysign = (y >> 31) & 1;
    int sub = y + (~x + 1); // may cause overflow, eg. Tmin - Tmax
    int subsign = (sub >> 31) & 1; // which return a postive result

    // Core trick!
    // since each "sign" is just one bit value(0 or 1)
    // & and | operands have same effect of logic && and ||
    // then this expression stands for
    // (x<0 && y>=0) or (x and y has same sign AND y-x >= 0)
    return (xsign & !ysign) | ((!xsign ^ ysign) & !subsign);
    
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
    // compute negtive x, then compare their sign bit
    // if x = 0, negx is still 0, which has same sign
    // otherwise, it will change
    // the only exception is Tmin(-Tmin = Tmin)
    // so exclude 
    int negx = ~x + 1;
    int xsign = (x >> 31) & 1;
    int negxsign = (negx >> 31) & 1;
    // Can't use ~
    // use ^1 to instead !, which turn 0 to 1; 1 to 0
    // xsign^1 is to exclude negtive case
    return (xsign ^ negxsign ^ 1) & (xsign ^ 1);
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    // The goal is find the most significant bit 1
    // similar to binary search: 16 -> 8 -> 4 -> 2 -> 1
    // b16 is 16 or 0, b8 is 8 or 0, etc
    // accumulate them to get the index of  most significant bit 1

    int b16, b8, b4, b2, b1, b0;
    int flag = x >> 31; // 0x0 or 0xff..f
    // if x is negtive, take complement
    // if x is positive or 0, keep unchanged
    // exploit "conditional" function
    x = (~x & flag) | (x & ~flag);

    b16 = !!(x >> 16) << 4; // b16 = 0 or 16
    x >>= b16; // shift 16 bits or unchanged

    b8 = !!(x >> 8) << 3;
    x >>= b8;

    b4 = !!(x >> 4) << 2;
    x >>= b4;

    b2 = !!(x >> 2) << 1;
    x >>= b2;

    b1 = !!(x >> 1); // b1 = 0 or 1
    x >>= b1;

    b0 = x; // now x is 0 or 1
    
    // the last 1 is sign bit
    return b16 + b8 + b4 + b2 + b1 + b0 + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
    const unsigned posinf = 0x7f800000;    
    unsigned sign = uf & 0x80000000;
    unsigned exp = uf & posinf; 
    unsigned m = uf & 0x007fffff;
    
    // uf is NaN
    if ((exp >> 23) == 0xff && m != 0) {
        return uf;
    }
    
    // denormalized case
    if (exp == 0) {
        // magic property!
        m <<= 1; // whether m will overflow, it's correct
    } 
    // normalized that without overflow
    // exp is less than 1111 1110(0xfe)
    else if ((exp >> 23) < 0xfe) {
        exp += (1 << 23); // exp's bits pattern + 1
    }
    // large norm that 2uf causes overfolw
    // return infinity(keep sign bit)
    else return sign | posinf;
    
    // return modified bits pattern
    return sign | exp | m;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    // float -> int:
    // truncate fractional part and round towards 0
    
    // bias = 127
    // Norm.: E = -126 ~ 127, exp = 0x01 ~ 0xfe
    // Denorm.: E = -126, exp = 0, frac = 2^-23 ~ (1-2^-23)
    // the range of int is [-2^31, 2^31 -1]
    
    const int bias = 127;
    // use masks to extract each part of uf
    const unsigned posinf = 0x7f800000;    
    unsigned s = uf & 0x80000000;
    unsigned exp = uf & posinf; 
    unsigned frac = uf & 0x007fffff;

    int sign = s >> 31;
    int E = (exp >> 23) - bias;
   
    // denormalized, or int(f) < 0
    // round to 0
    if (exp == 0 || E < 0) {
        return 0;
    } 

    // normalized
    // if positive
    if (sign == 0) {
        // if out of range
        if (E > 30) {
            return 0x80000000u;
        }
        // compute 1.frac * 2^E
        else return (0x00800000 | frac) >> (23 - E);
    }
    else { // negtive
        if (E > 31) {
            return 0x80000000u;
        }
        // compute the int value and turn to negtive
        else return ~((0x00800000 | frac) >> (23 - E)) + 1;
    } 
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    // bias = 127
    // Norm.: E = -126 ~ 127, exp = 0x01 ~ 0xfe
    // for 2^x, frac = 0, M = 1
    // thus the range of norm is [2^-126, 2^127] 
    // Denorm.: E = -126, smallest frac = 0x01(M = 2^-23)
    // thus the range of denorm is [2^-149, 2^-126)

    const int bias = 127, smallestE = -149, largestE = 127;
    // bits pattern to construct result
    unsigned sign = 0, exp, frac;
    
    // if out of range, return 0 or +INF
    if (x < smallestE) {
        return 0;
    }
    if (x > largestE) {
        return 0x7f800000;
    }

    // denormalized
    if (x < -126) {
        exp = 0;
        // since target is power of 2
        // so frac has only one bit be 1, others be 0
        // 126 - x is the index of bit 1, starting from left to right
        // so 23 - (126 - x) is the bits needs to be shift leftwards
        frac = (1 << (23 - (-126 - x)));
        return sign | exp | frac;
    }

    // normalized
    exp = (x + bias) << 23;
    frac = 0;
    return sign | exp | frac;

}
