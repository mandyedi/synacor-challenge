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

const uint16 register_0 = 32768;
const uint16 register_1 = 32769;
const uint16 register_2 = 32770;
const uint16 register_3 = 32771;
const uint16 register_4 = 32772;
const uint16 register_5 = 32773;
const uint16 register_6 = 32774;
const uint16 register_7 = 32775;

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

class VirtualMachine
{

public:

    VirtualMachine()
    : Memory( nullptr )
    , MemorySize( 0 )
    , MemoryOffset( 0 )
    , Executing( false )
    , DebugEnabled( false )
    , DebugActive( false )
    {
        for ( int i = 0; i < 8; i++ )
        {
            Registers[i] = 0;
        }
    }

    void LoadFile( std::string &fileName )
    {
        std::ifstream inputStream( fileName, std::ifstream::binary );

        if ( inputStream )
        {
            inputStream.seekg( 0, inputStream.end );
            int32 length = inputStream.tellg();
            inputStream.seekg( 0, inputStream.beg );
            MemorySize = length / 2;
            Memory = new uint16[MemorySize];

            uint16 chunk;
            uint32 i = 0;
            while( inputStream.read( reinterpret_cast<char*>(&chunk), sizeof chunk ) )
            {
                Memory[i] = chunk;
                i++;
            }

            std::cout << "[VM] MemorySize " << MemorySize << std::endl;
        }
        else
        {
            // TODO
            return;
        }
    }

    void Run( bool debugEnabled, uint32 stepFrom = 0 )
    {
        uint16 args[3];
        Executing = true;
        int32 cycle = 0;

        while( MemoryOffset < MemorySize && Executing /*&& cycle < 1000*/ )
        {
            // std::cout << MemoryOffset << ": " << Memory[ MemoryOffset ] << std::endl;

            uint32 debugAddress = MemoryOffset;

            uint16 opCode       = Memory[ MemoryOffset ];
            uint16 numberOfArgs = ArgCount[ opCode ];
            assert( numberOfArgs <= 3 );

            // std::cout << "opCode: " << opCode << " args: ";
            for ( int i = 0; i < numberOfArgs; i++ )
            {
                MemoryOffset++;
                args[i] = Memory[ MemoryOffset ];
                // std::cout << args[0] << " ";
            }
            // std::cout << "\n";

            MemoryOffset++;

            // TODO when fixed, find "\nThere" and debug from it
            if ( debugEnabled && ( ( !DebugActive && debugAddress == stepFrom ) || DebugActive ) )
            {
                DebugActive = true;
                std::cout << "Registers:\n";
                for ( int i = 0; i < 8; i++ )
                {
                    std::cout << RegisterNames[i] << ": " << Registers[i] << "\n";
                }

                std::cout << "Stack:\n";
                PrintStack();

                std::cout << "Execute next: " << debugAddress << ": " << OpNames[opCode];
                for ( int i = 0; i < numberOfArgs; i++ )
                {
                    if ( (OPCODE)opCode == OPCODE::OUT )
                    {
                        std::cout << " " << "'" << (char)args[i] << "'";
                    }
                    else if ( args[i] >= register_0 && args[i] <= register_7 )
                    {
                        std::cout << " " << RegisterNames[ args[i] % register_0 ];
                    }
                    else
                    {
                        std::cout << " " << args[i];
                    }
                }
                std::cout << "\n"; 
                std::cin.get();
            }

            HandleOPCode( opCode, args );

            cycle++;
        }
    }

    void ShutDown()
    {
        delete[] Memory;
    }

    void Disassemble( const std::string &fileName )
    {
        std::ofstream f( fileName );
        if ( f.is_open() )
        {
            std::cout << "[VM] Start disassembling.\n";

            std::vector<int32> charCodes;
            uint32 offset = 0;
            while( offset < MemorySize )
            {
                uint16 opCode = Memory[ offset ];

                if ( opCode < 22 && opCode != 19 )
                {
                    f << offset << ": ";
                    f << OpNames[opCode] << " ";
                    uint16 numberOfArgs = ArgCount[ opCode ];
                    assert( numberOfArgs <= 3 );
                    for ( int i = 0; i < numberOfArgs; i++ )
                    {
                        offset++;
                        uint16 arg = Memory[ offset ];
                        if ( arg >= register_0 && arg <= register_7 )
                        {
                            f << RegisterNames[arg % register_0] << " ";
                        }
                        else
                        {
                            f << arg << " ";
                        }
                    }
                    f << "\n";
                }
                else if ( opCode == 19 )
                {
                    offset++;
                    if ( Memory[offset + 1] != 19 )
                    {
                        f << offset << ": ";
                        f << OpNames[opCode] << " \"";
                        for (int i = 0; i < charCodes.size(); i++)
                        {
                            int32 charCode = charCodes[i];
                            switch(charCode)
                            {
                                case 10:
                                    f << "\\n";
                                    break;
                                default:
                                    f << (char)charCode;
                                    break;        
                            }
                                
                        }
                        charCodes.clear();
                        f << "\"\n";
                    }
                    else
                    {
                        charCodes.push_back( (int32)Memory[offset] );
                    }
                }
                else
                {
                    f << offset << ": ";
                    f << opCode;
                    f << "\n";
                }

                offset++;
            }
            f.close();
        }
        else
        {
            std::cout << "[VM] Error: Could not open file: " << fileName << std::endl;
        }

        std::cout << "[VM] Disassemble is done. Output file: " << fileName << "\n";
    }

    void DebugHandleOpCode( std::string &opName, uint16 opCode, uint16 *args, int32 ArgCount )
    {
        std::ofstream f( "log_opcode.txt", std::ofstream::app );
        if ( f.is_open() )
        {
            f << opName << " (" << opCode << ")";

            for ( int32 i = 0; i < ArgCount; i++ )
            {
                f << " " << args[i];
            }

            f << "\n";

            f.close();
        }
        else
        {
            std::cout << "[VM] Error: Could not open file: log_opcode.txt\n";
        }
    }

private:

    uint16 GetLiteral( uint16 input )
    {
        assert( input <= register_7 );
        if ( input < register_0 )
        {
            return input;
        }
        else if ( input <= register_7 )
        {
            return Registers[input % register_0];
        }
        else
        {
            // TODO handle error, input is out of bounds
        }
    } 

    void WriteRegister( uint16 reg, uint16 value )
    {
        assert( reg >= register_0 && reg <= register_7 );
        Registers[reg % register_0] = value;
    }

    void Jump( uint16 pos )
    {
        assert( pos < MemorySize );
        MemoryOffset = pos;
    }

    void HandleOPCode( uint16 opCode, uint16 *args )
    {
        // MemoryOffset is set to the next opCode
        // DO NOT change the offset here, use Jump( uint16 pos) instead 

        // args[0] a
        // args[1] b
        // args[2] c

        switch ( opCode )
        {
            // halt
            case 0:
            {
                std::cout << "[VM] Terminate program with opCode 0.\n";
                Executing = false;
                break;
            }

            // set a b
            // set register <a> to the value of <b>
            case 1:
            {
                WriteRegister( args[0], GetLiteral( args[1] ) );
                break;
            }

            // push a
            // push <a> onto the stack
            case 2:
            {
                Stack.push( GetLiteral( args[0]) );
                break;
            }

            // remove 3 a
            // remove the top element from the stack and write it into <a>; empty stack = error
            case 3:
            {
                if ( Stack.empty() )
                {
                    std::cout << "[VM] Error: empty stack.\n";
                }
                uint16 top = Stack.top();
                WriteRegister( args[0], top );
                Stack.pop();
                break;
            }

            // eq a b c
            // set <a> to 1 if <b> is equal to <c>; set it to 0 otherwise
            case 4:
            {
                uint16 b = GetLiteral( args[1] );
                uint16 c = GetLiteral( args[2] );
                b == c ? WriteRegister( args[0], 1 ) : WriteRegister( args[0], 0 );
                break;
            }

            // gt a b c
            // set <a> to 1 if <b> is greater than <c>; set it to 0 otherwise
            case 5:
            {
                uint16 b = GetLiteral( args[1] );
                uint16 c = GetLiteral( args[2] );
                b > c ? WriteRegister( args[0], 1 ) : WriteRegister( args[0], 0 );
                break;
            }

            // jmp a
            // jump to <a>
            case 6:
            {
                Jump( args[0] );
                break;
            }

            // jt a b
            // if <a> is nonzero, jump to <b>
            case 7:
            {
                if ( GetLiteral( args[0] ) != 0)
                {
                    Jump( args[1] );
                }
                break;
            }

            // jf a b
            // if <a> is zero, jump to <b>
            case 8:
            {
                if ( GetLiteral( args[0] ) == 0)
                {
                    Jump( args[1] );
                }
                break;
            }

            // add a b c
            // assign into <a> the sum of <b> and <c> (modulo 32768)
            case 9:
            {
                WriteRegister( args[0], ( GetLiteral( args[1] ) + GetLiteral( args[2] ) ) % register_0 );
                break;
            }

            // mult a b c
            // store into <a> the product of <b> and <c> (modulo 32768)
            case 10:
            {
                WriteRegister( args[0], ( GetLiteral( args[1] ) * GetLiteral( args[2] ) ) % register_0 );
                break;
            }

            // mod a b c
            // store into <a> the remainder of <b> divided by <c>
            case 11:
            {
                WriteRegister( args[0], GetLiteral( args[1] ) % GetLiteral( args[2] ) );
                break;
            }

            // and a b c
            // stores into <a> the bitwise and of <b> and <c>
            case 12:
            {
                WriteRegister( args[0], GetLiteral( args[1] ) & GetLiteral( args[2] ) );
                break;
            }

            // or a b c
            // stores into <a> the bitwise or of <b> and <c>
            case 13:
            {
                WriteRegister( args[0], GetLiteral( args[1] ) | GetLiteral( args[2] ) );
                break;
            }

            // not a b
            // stores 15-bit bitwise inverse of <b> in <a>
            case 14:
            {
                // WriteRegister( args[0], ~GetLiteral( args[1] ) & ( ( 1 << 15) - 1 ) );
                WriteRegister( args[0], GetLiteral( args[1] ) ^ 0x7fff );
                break;
            }

            // rmem a b
            // read memory at address <b> and write it to <a>
            case 15:
            {
                WriteRegister( args[0], Memory[ GetLiteral( args[1] ) ] );
                break;
            }

            // wmem a b
            // write the value from <b> into memory at address <a>
            case 16:
            {
                Memory[ GetLiteral( args[0] ) ] = GetLiteral( args[1] );
                break;
            }

            // call a
            // write the address of the next instruction to the stack and jump to <a>
            case 17:
            {
                Stack.push( MemoryOffset );
                Jump( GetLiteral( args[0] ) );
                break;
            }

            // ret
            // remove the top element from the stack and jump to it; empty stack = halt
            case 18:
            {
                if ( Stack.empty() )
                {
                    std::cout << "[VM] Error: empty stack.\n";
                    Executing = false;
                    break;
                }
                uint16 top = Stack.top();
                Jump( top );
                Stack.pop();
                break;
            }

            // out a
            // write the character represented by ascii code <a> to the terminal
            case 19:
            {
                if ( !DebugActive )
                {
                    std::cout << (char)args[0];
                }
                break;
            }

            // in a
            // read a character from the terminal and write its ascii code to <a>;
            // it can be assumed that once input starts, it will continue until a newline is encountered;
            // this means that you can safely read whole lines from the keyboard and trust that they will be fully read
            case 20:
            {
                char c;
                std::cin.get( c );
                WriteRegister( args[0], (uint16)c );
                break;
            }

            // noop
            case 21:
            {
                break;
            }

            default:
            {
                std::cout << "[VM] Error! OpCode is invalid.\n";
                break;
            }
        }
    }

    void PrintStack()
    {
        // Copy Stack to a std::vector
        // Print std::vector
        // Copy std::vector content back to Stack

        if ( Stack.empty() )
        {
            std::cout << "empty\n";
        }

        std::vector<uint16> v;

        while ( !Stack.empty() )
        {
            v.push_back( Stack.top() );
            Stack.pop();
        }

        std::vector<uint16>::iterator it = v.begin();
        for ( ; it != v.end(); ++it )
        {
            std::cout << *it << "\n";
        }

        std::vector<uint16>::reverse_iterator rit = v.rbegin();
        for ( ; rit != v.rend(); ++rit )
        {
            Stack.push( *rit );
        }
    }

    uint16 *Memory;
    uint32  MemorySize;
    uint32  MemoryOffset;
    uint16  Registers[8];
    std::stack<uint16> Stack;
    bool    Executing;
    bool    DebugEnabled;
    bool    DebugActive;

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