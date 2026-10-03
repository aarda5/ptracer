#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>

const char *names[] = {
    [0] = "read",
    [1] = "write",
    [2] = "open",
    [3] = "close",
    [5] = "fstat",
    [8] = "lseek",
    [9] = "mmap",
    [10] = "mprotect",
    [11] = "munmap",
    [12] = "brk",
    [13] = "rt_sigaction",
    [14] = "rt_sigprocmask",
    [16] = "ioctl",
    [17] = "pread64",
    [18] = "pwrite64",
    [19] = "readv",
    [20] = "writev",
    [21] = "access",
    [22] = "pipe",
    [32] = "dup",
    [33] = "dup2",
    [35] = "nanosleep",
    [39] = "getpid",
    [41] = "socket",
    [42] = "connect",
    [43] = "accept",
    [44] = "sendto",
    [45] = "recvfrom",
    [46] = "sendmsg",
    [47] = "recvmsg",
    [49] = "bind",
    [50] = "listen",
    [54] = "setsockopt",
    [55] = "getsockopt",
    [56] = "clone",
    [57] = "fork",
    [58] = "vfork",
    [59] = "execve",
    [60] = "exit",
    [61] = "wait4",
    [62] = "kill",
    [63] = "uname",
    [72] = "fcntl",
    [74] = "fsync",
    [78] = "getdents",
    [79] = "getcwd",
    [80] = "chdir",
    [83] = "mkdir",
    [87] = "unlink",
    [89] = "readlink",
    [90] = "chmod",
    [95] = "umask",
    [96] = "gettimeofday",
    [101] = "ptrace",
    [102] = "getuid",
    [104] = "getgid",
    [107] = "geteuid",
    [108] = "getegid",
    [110] = "getppid",
    [131] = "sigaltstack",
    [157] = "prctl",
    [158] = "arch_prctl",
    [186] = "gettid",
    [202] = "futex",
    [217] = "getdents64",
    [218] = "set_tid_address",
    [228] = "clock_gettime",
    [231] = "exit_group",
    [232] = "epoll_wait",
    [233] = "epoll_ctl",
    [234] = "tgkill",
    [257] = "openat",
    [258] = "mkdirat",
    [262] = "newfstatat",
    [263] = "unlinkat",
    [267] = "readlinkat",
    [269] = "faccessat",
    [271] = "ppoll",
    [273] = "set_robust_list",
    [281] = "epoll_pwait",
    [291] = "epoll_create1",
    [292] = "dup3",
    [293] = "pipe2",
    [302] = "prlimit64",
    [318] = "getrandom",
    [319] = "memfd_create",
    [322] = "execveat",
    [332] = "statx",
    [334] = "rseq",
    [435] = "clone3",
    [436] = "close_range",
    [437] = "openat2",

};

int main(int argc, char *argv[]){



    if(argc < 2){

        fprintf(stderr, "To start using tracer, type a command to trace.\n"); 
        return EXIT_FAILURE;
    
    }

    pid_t pid = fork();

    if (pid < 0){
        perror("Fork failed.");
        exit(1);
    }



    else if(pid==0){
        if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
            perror("ptrace");
            _exit(1);
        }

        execvp(argv[1], &argv[1]);
        perror("execvp");
        _exit(1);
        
    }


    else{
        int status;
        if(waitpid(pid, &status, 0 ) == -1){
            perror("waitpid");
            _exit(1);
        }

        
        int entry = 1;
        while(1){
        if(ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1){
            perror("syscall");
            _exit(1);
        }
        if(waitpid(pid, &status, 0 ) == -1){
            perror("waitpid");
            _exit(1);
        }

        if(WIFEXITED(status)){
            printf("child exited.\n");
            _exit(0);
        }

        if(WIFSIGNALED(status)){
            printf("child signaled.\n");
            _exit(0);
        }

        if(WIFSTOPPED(status)){
        struct user_regs_struct regs;
        if(ptrace(PTRACE_GETREGS, pid, NULL, &regs)== -1){
            perror("PTRACE_GETREGS");
            _exit(1);
        }
        unsigned long long n = regs.orig_rax;
        size_t table_size = sizeof(names) / sizeof(names[0]);

        if (n < table_size && names[n] && entry)
            printf("%s\n", names[n]);
        else if (entry)
            printf("%llu\n", n);

        entry = !entry;


        

        }


        
        }
    }



    return 0;
}
