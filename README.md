# Synacor Challenge
<a href="https://challenge.synacor.com" target="_blank">https://challenge.synacor.com</a>

> In this challenge, your job is to use this architecture spec to create a virtual machine capable of running the included binary.  Along the way, you will find codes; submit these to the challenge website to track your progress.  Good luck! 

My solution written in c++. See detailed spec in **arch-spec**.

##### Usage

Execute challenge:

synacor challenge.bin

Disassemble:

synacor challenge.bin asm

Debug:

synacor challenge.bin debug x
x: breakpoint at memory address
Use Enter to step over next opcode

##### What works:
* Disassembler
* Debug with step over. Prints registers and stack.

##### Known bugs:
* Self test messes up the output
