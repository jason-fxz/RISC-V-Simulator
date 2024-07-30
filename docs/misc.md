

## Issue

在使用 ROB 的 Tomasulo 中，指令能否发射主要观察两点：**保留站**是否有空闲和 **ROB** 是否有空闲，如果都有空闲，就可以发射，发射之后指令会分别占据保留站、ROB 一个表项，并用 **ROB 的编号**标记寄存器结果状态表；

条件：ROB 和对应指令的 RS（对于要 ALU 的指令） / LSB（对于内存读写的指令） 都有空闲。
操作：
- 同时通知 ROB 和 RS / LSB



## ALU

包含 ADD(ADD/SUB), CAMP(SLT/SLTU), LOGIC(XOR/OR/AND), SHIFT(SLL/SRL/SRA) 的运算器，

每个运算单元是独立的，可以并行，运算器需要的时钟周期可以不同。


过程：
1. 某个计算器从对应 RS 接受到操作数，A,B 和对应操作 OP，设置 busy
2. 运算数个周期。
3. 计算完成，将结果写回 ROB 和 RS（总线通知），清空 busy


busy 为 true 说明运算器下一个周期不接受新的输入。

```plaintext
  
    Adder
+----------+
|          | [reg]A  <-------
|          | [reg]B  <-------
|          | [reg]OP  <-------
|          | [wire]busy ------->
+----------+
    |
    |
    +----->  Result
```