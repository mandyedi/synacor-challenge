#include "virtual_machine.h"

int main( int argc, char *argv[] )
{
    if ( argc < 2)
    {
        std::cout << "There is nothing to execute.\n";
        return 0;
    }

    VirtualMachine vm;

    std::string arg1( argv[1] );
    vm.LoadFile( arg1 );

    if ( argc == 3 )
    {
        std::string arg2( argv[2] );
        if ( arg2.compare( "asm" ) == 0 )
        {
            vm.Disassemble( arg1 + ".asm" );
        }
    }
    else
    {
        vm.Run();
    }

    vm.ShutDown();

    return 0;
}