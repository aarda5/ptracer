# ptracer - systemcall tracer

---

A minimal x86-64 Linux syscall tracer using **ptrace()**


The tracer launches another program, stops it at syscall boundaries, reads its CPU registers, and prints the syscall it makes.


## Build

`gcc -Wall -Wextra -Wpedanctic tracer.c -o tracer`

## Usage

`./tracer <program> [arguments...]`

### Example:

`./tracer pwd`

Output:

brk
access
openat
fstat
mmap
close
openat
read
mmap
getcwd
write
/home/user/project
close
exit_group
child exited.

The target program's own output also appears along with the tracer output.

##How It Works

First, the tracer creates a child process with **fork()**.

The child calls `PTRACE_TRACEME()` which allows the parent to trace the child process.

The parent uses `PTRACE_SYSCALL()` in order for child to continue until it reaches the next syscall boundary.

When the child stops, the tracer uses `PTRACE_GETREGS` to retrieve its CPU register.

The retrieved register is in integer format. Therefore, it also gets mapped to the proper syscall name from the names[] array.





