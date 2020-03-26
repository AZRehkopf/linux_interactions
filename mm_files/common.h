#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h> 
#include <math.h>
#include <time.h>

#define KEY 1492
#define SEM_MUTEX 0 
#define FILE_NAME "res.txt"

typedef struct control_info {
	int sem_id;
	char *addr;
} control_info;

struct control_info init_mmap_file();
void sem_wait(struct sembuf sem_op, int sem_id);
void sem_signal(struct sembuf sem_op, int sem_id);