//   Stack frame after setup (use BP to access activation frame elements):
//        SP->  local varN-1    (BP+N)
//              ...
//              local var0      (BP+1)
//        BP->  old BP
//              retaddr         (BP-1)
//              parameter0      (BP-2)
//              ...
//              parameterN-1    (BP-(N+2))

    @L<retaddr>     // push retaddr
    D=A
    @SP
    M=M+1               
    A=M
    M=D
    @fctname        // unconditional jump to fctname
    0;JMP
(retaddr)

