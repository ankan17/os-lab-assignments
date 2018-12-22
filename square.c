#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>

char* itoa(int val, int base){
    
    static char buf[32] = {0};    
    int i = 30;    
    for(; val && i ; --i, val /= base)    
        buf[i] = "0123456789abcdef"[val % base];   
    return &buf[i+1];
    
}

int main(int argc, char* argv[]) {
    if (argc == 1)
        exit(0);
        
    int n = atoi(argv[argc - 1]);
    int res = n * n;
    printf("Squaring, Current process id: %u, Current result: %d\n", getpid(), res);

    char* v[argc];
    for (int i = 1; i < (argc-1) ; i++) {
        v[i-1] = argv[i];
    }
        
    v[argc-2] = itoa(res, 10);
    v[argc-1] = NULL;
    
    execvp(v[0], v);
}
    
