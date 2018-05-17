#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <cassert>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>

typedef unsigned char      uint8;
typedef unsigned short int uint16;
typedef signed   long  int  int32;
typedef unsigned long  int uint32;

class VirtualMachine
{

public:

    VirtualMachine();

    void LoadFile( std::string &fileName );

    void Run( bool debugEnabled, uint32 stepFrom = 0 );

    void ShutDown();

    void Disassemble( const std::string &fileName );

    void DebugHandleOpCode( std::string &opName, uint16 opCode, uint16 *args, int32 ArgCount );

private:

    uint16 GetLiteral( uint16 input ); 

    void WriteRegister( uint16 reg, uint16 value );

    void Jump( uint16 pos );

    void HandleOPCode( uint16 opCode, uint16 *args );

    void PrintStack();

    uint16 *Memory;
    uint32  MemorySize;
    uint32  MemoryOffset;
    uint16  Registers[8];
    std::stack<uint16> Stack;
    bool    Executing;
    bool    DebugEnabled;
    bool    DebugActive;

    const uint16 register_0 = 32768;
    const uint16 register_1 = 32769;
    const uint16 register_2 = 32770;
    const uint16 register_3 = 32771;
    const uint16 register_4 = 32772;
    const uint16 register_5 = 32773;
    const uint16 register_6 = 32774;
    const uint16 register_7 = 32775;

    uint16 ArgCount[22] = {
        0, // halt: 0
        2, // set: 1 a b
        1, // push: 2 a
        1, // pop: 3 a
        3, // eq: 4 a b c
        3, // gt: 5 a b c
        1, // jmp: 6 a
        2, // jt: 7 a b
        2, // jf: 8 a b
        3, // add: 9 a b c
        3, // mult: 10 a b c
        3, // mod: 11 a b c
        3, // and: 12 a b c
        3, // or: 13 a b c
        2, // not: 14 a b
        2, // rmem: 15 a b
        2, // wmem: 16 a b
        1, // call: 17 a
        0, // ret: 18
        1, // out: 19 a
        1, // in: 20 a
        0, // noop: 21
    };

    enum class OPCODE
    {
        HALT = 0,
        SET,
        PUSH,
        POP,
        EQ,
        GT,
        JMP,
        JT,
        JF,
        ADD,
        MULT,
        MOD,
        AND,
        OR,
        NOT,
        RMEM,
        WMEM,
        CALL,
        RET,
        OUT,
        IN,
        NOOP
    };

    std::string OpNames[22] = {
        "HALT",
        "SET",
        "PUSH",
        "POP",
        "EQ",
        "GT",
        "JMP",
        "JT",
        "JF",
        "ADD",
        "MULT",
        "MOD",
        "AND",
        "OR",
        "NOT",
        "RMEM",
        "WMEM",
        "CALL",
        "RET",
        "OUT",
        "IN",
        "NOOP"
    };

    std::string RegisterNames[8] = {
        "R0",
        "R1",
        "R2",
        "R3",
        "R4",
        "R5",
        "R6",
        "R7",
    };
};

#endif /* VIRTUAL_MACHINE_H */