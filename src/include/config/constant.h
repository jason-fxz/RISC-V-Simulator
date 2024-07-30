/**
 * @file constant.h
 * @author JasonFan (jasonfanxz@gmail.com)
 * @brief some config constant definition
 * @version 0.1
 * @date 2024-07-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef CONSTANT_H
#define CONSTANT_H

namespace jasonfxz {

const int MAX_RAM_SIZE = 1 << 20; // 1MB

/**
+*********************************************************************************+
+  #   + Name  + Description                +  #   + Name  + Desc                 +
+******+*******+****************************+******+*******+**********************+
+ x0   + zero  + Constant 0                 + x16  + a6    + Args                 +
+ x1   + ra    + Return Address             + x17  + a7    +                      +
+ x2   + sp    + Stack Pointer              + x18  + s2    + Saved Registers      +
+ x3   + gp    + Global Pointer             + x19  + s3    +                      +
+ x4   + tp    + Thread Pointer             + x20  + s4    +                      +
+ x5   + t0    + Temporary Registers        + x21  + s5    +                      +
+ x6   + t1    +                            + x22  + s6    +                      +
+ x7   + t2    +                            + x23  + s7    +                      +
+ x8   + s0    + Saved Registers            + x24  + s8    +                      +
+ x9   + s1    +                            + x25  + s9    +                      +
+ x10  + a0    + Function Arguments or      + x26  + s10   +                      +
+ x11  + a1    +  Return Values             + x27  + s11   +                      +
+ x12  + a2    + Function Arguments         + x28  + t3    + Temporaries          +
+ x13  + a3    +                            + x29  + t4    +                      +
+ x14  + a4    +                            + x30  + t5    +                      +
+ x15  + a5    +                            + x31  + t6    +                      +
+*********************************************************************************+

Caller saved registers
Callee saved registers (except x0, gp, tp)
*/
const int REG_FILE_SIZE = 32; // 32 registers

const int MAX_RS_SIZE = 8; // Reservation Station : 8 entries

const int MAX_LSB_SIZE = 8; // Load Store Buffer : 8 entries


// const int MAX_ROB_SIZE = 32; // ROB QUEUE : 32 entries



} // namespace jasonfxz


#endif // CONSTANT_H 