#include "common.h"

struct control_info init_mmap_file() {
    union semun sem_val;
    struct stat file_info;
    int sem_id, file_id;
    char *addr;

    // Open file and check for errors
    file_id = open(FILE_NAME, O_RDWR);
    if (file_id == -1) {
        perror("Error: Could not open file\n");
        exit(1);
    }
    
    // Initalize and sync semaphore then check for errors
    sem_id = semget(KEY, 2, IPC_CREAT | 0666);
    if (sem_id == -1) {
        perror("Error: Could not intialize semaphore\n");
        exit(1);
    }
    
    // Set and broadcast update to semaphore
    sem_val.val = 1;
    semctl(sem_id, 0, SETVAL, sem_val);

    // Returns statistics about the file and stores them in file_info or throws error
    if (fstat(file_id, &file_info) == -1) { 
        perror("Error: Could not get file statistics\n");
        exit(1);
    }

    // Make sure file is a regular file if not throw error 
    if (!S_ISREG(file_info.st_mode)) {
        perror("Error: Invalid file type, expected regular file\n");
        exit(1);
    }

    // Map file to memory and return appropriate memory address, throw error if this fails
    
    addr = mmap(0, file_info.st_size, PROT_WRITE | PROT_READ, MAP_SHARED, file_id, 0);
    if (addr == MAP_FAILED) {
        perror("Error: Failed to map file\n");
        exit(1);
    }

    // Attempt to close file and throw error if this fails
    if (close(file_id) == -1) {
        perror("Error: Could not close file\n");
        exit(1);
    }

    struct control_info info = {sem_id, addr};
    return info;
}

void sem_wait(struct sembuf sem_op, int sem_id) {
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}

void sem_signal(struct sembuf sem_op, int sem_id) {
    sem_op.sem_num = 0;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(sem_id, &sem_op, 1);
}