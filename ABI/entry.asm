//   Stack frame after setup (use BP to access activation frame elements):
//        SP->  local varN-1    (BP+N)
//              ...
//              local var0      (BP+1)
//        BP->  old BP
//              retaddr         (BP-1)
//              parameter0      (BP-2)
//              ...
//              parameterN-1    (BP-(N+2))

// main	entry	$$1
(main)
    @BP                 //  - push current BP
    D=M
    @SP
    M=M+1
    A=M
    M=D    
    @SP                 //  - move SP to BP
    D=M
    @BP
    M=D
    @$$1                //  - adjust SP to point to TOS by adding $$1
    D=A
    @SP
    M=M+D

