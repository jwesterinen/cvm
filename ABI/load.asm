//   Stack frame after setup (use BP to access activation frame elements):
//        SP->  local varN-1    (BP+N)
//              ...
//              local var0      (BP+1)
//        BP->  old BP
//              retaddr         (BP-1)
//              parameter0      (BP-2)
//              ...
//              parameterN-1    (BP-(N+2))

// load	global
    @g
    D=M
    @SP
    M=M+1
    A=M
    M=D
    
// load	constant
    @5
    D=A
    @SP
    M=M+1
    A=M
    M=D
    
// load param 0
    @2
    D=A
    @BP
    A=M-D
    D=M
    @SP
    M=M+1
    A=M
    M=D
    
// load local (1)
    @2	// y
    D=A
    @BP
    A=M+D
    D=M
    @SP
    M=M+1
    A=M
    M=D
    

