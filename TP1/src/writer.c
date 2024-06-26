//-----------------------------------------------------------------------------------------------------------
//cd C:\Users\mpaur\OneDrive\Documentos\ITBA\MESE\SOPG\Practica\SOPG_Practica\app\src>
//C:\Users\mpaur\OneDrive\Documentos\ITBA\MESE\SOPG\Practica\SOPG_Practica\app\src>start prueba.exe
//-----------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------
//En Linux
//se compila por linea de comando con $ gcc -Wall -pedantic ./src/writer.c -o ./build/writer
//Execute on Terminal ./build/writer
//para ver los procesos en otro terminal correr 
//& watch -n 1 "ps -elf | grep a.out"
//la "S" se ve en general porque gran parte del tiempo esta en sleep()
//cuando lo paramos se pone con una "T"
//-----------------------------------------------------------------------------------------------------------

/********************** inclusions *******************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
   
/* Project includes. */

/* Demo includes. */


/* Application & Tasks includes. */


/********************** macros and definitions *******************************/
#define FIFO_NAME           "myfifo"
#define STDIN_FD            0
#define STDOUT_FD           1

#define EXIT_ERROR          1

/********************** internal data declaration ****************************/
pid_t pid_writer;
int fd = 0;
/********************** internal functions declaration ***********************/
void sigint_handler(int sig);
void sigpipe_handler(int sig);
void sigusr1_handler(int sig);
void sigusr2_handler(int sig);

/********************** internal data definition *****************************/


/********************** external data declaration *****************************/


/********************** external functions definition ************************/

int main(void)
{
    //Process information initilize
    pid_writer = getpid();
    printf("PROCESO WRITER PID = %d\n", getpid());

    // Signal Interrupt process inicitilize
	struct sigaction sa_int;
	sa_int.sa_handler = sigint_handler;
	sa_int.sa_flags = 0; //SA_RESTART;
	sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_ERROR);
    }

    // signal pipe process inicitilize (reader() closes)
	struct sigaction sa_pipe;
	sa_pipe.sa_handler = sigpipe_handler;
	sa_pipe.sa_flags = 0; //SA_RESTART;
	sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGPIPE, &sa_pipe, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_ERROR);
    }

    // User 1 signal process inicitilize
	struct sigaction sa_usr1;
	sa_usr1.sa_handler = sigusr1_handler;
	sa_usr1.sa_flags = 0; //SA_RESTART;
	sigemptyset(&sa_usr1.sa_mask);
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_ERROR);
    }

    // User 2 signal process inicitilize
	struct sigaction sa_usr2;
	sa_usr2.sa_handler = sigusr2_handler;
	sa_usr2.sa_flags = 0; //SA_RESTART;
	sigemptyset(&sa_usr2.sa_mask);
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1)
    {
        perror("sigaction");
        exit(EXIT_ERROR);
    }

    //Named FIFO Initialilze

    if(mkfifo(FIFO_NAME, 0666) == -1)
    {
        if(errno == EEXIST)
            printf("FIFO already exists\n"); 
        else
        {
            perror("FIFO doesn't exist");
            exit(EXIT_ERROR);
        }
    }    
    fd = open(FIFO_NAME, O_WRONLY);                 //Keep waiting til there is someone to read
    if(fd == -1)
    {
        perror("FIFO couldn't be open");
        exit(EXIT_ERROR);
    }    
   
    
    
    printf("got a reader--type some stuff\n");


    char s[300];
    strcpy(s,"DATA:");
    int numread,numwrite;
    while (1) 
    {
        if ((numread = read(STDIN_FD, s+5, 295)+5) == 4)
        {   
            if(errno != EINTR) //If not an interrut system call
                perror("DATA MSJ couldn't be read %d");
        }
        else
        {
            s[numread-1] = '\0';
            if ((numwrite = write(fd, s, strlen(s))) == -1)
                perror("DATA MSJ don't sended to FIFO");
            else
                printf("writer: wrote %d bytes\n", numwrite);
        }  
    }
}

/********************** internal functions definitSion ***********************/
void sigint_handler(int sig) 
{
    write(STDOUT_FD,"SIGINT\n",7);
    kill(pid_writer,SIGKILL);
    return;
}

void sigpipe_handler(int sig) 
{
    perror("reader closed");
    kill(pid_writer,SIGKILL);
    return;
}

void sigusr1_handler(int sig) 
{
    if(write(fd,"SIGN:1",7)==-1)
        perror("error on SIGN:1 didn't send to FIFO");      
    write(STDOUT_FD,"SIGUSR1\n",9);
    return;
}

void sigusr2_handler(int sig) 
{
    if(write(fd,"SIGN:2",7)==-1)
        perror("error on SIGN:2 didn't send to FIFO");      
    write(STDOUT_FD,"SIGUSR2\n",9);
    return;
}
