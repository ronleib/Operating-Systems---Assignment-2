#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // for fork()
#include <signal.h>
#include <sys/types.h>
//---------------------------------
void catch_sigusr1(int sig_num) ;
void do_son() ;
void do_dad(pid_t son_pid) ;
int i=0 ;
int pipe_descs[2];
//----------main------------------
int main() {
    pid_t pid ;

    signal(SIGUSR1, catch_sigusr1) ;

    if (pipe(pipe_descs) == -1) {
        fprintf(stderr, "cannot open pipe\n");
        exit(1);
    }

    pid = fork() ;

    switch(pid) {
        case -1 : perror("fork() failed") ;
                exit(EXIT_FAILURE) ;
        case 0  : do_son() ;
                exit(EXIT_SUCCESS) ;
            kill(getppid(), SIGUSR1) ;
        default: do_dad(pid) ;
                exit(EXIT_SUCCESS) ;
    }
    return 1 ;
}
//----------do_son----------------
void do_son() {
    printf("\n") ;
    sleep(4) ;
    while (i < 5) {

        write(pipe_descs[1], &i, sizeof(int));
        kill(getppid(), SIGUSR1) ;
        pause();
        int size=read(pipe_descs[0], &i, sizeof(int));
        printf("Parent Write: %d\n",i);
        i++;
    }
    printf("\nChild is going to be terminated") ;
//    kill(getppid(), SIGUSR1) ;
}

//----------do_dad------------------
void do_dad(pid_t son_pid) {
    while (i < 5) {
        pause();
        int size=read(pipe_descs[0], &i, sizeof(int));
        printf("Son Write:    %d\n",i);
        i++;
        write(pipe_descs[1], &i, sizeof(int));
        kill(son_pid, SIGUSR1);
    }
    sleep(1) ;
       printf("\nParent is going to be terminated \n") ;
    printf("\n") ;
}

//-------catch_sigusr1-----------------
void catch_sigusr1(int sig_num) {
    signal(SIGUSR1, catch_sigusr1);
    printf("    **process = %d, caught signal SIGUSR1** \n", getpid());
}

//--------------------------------- 