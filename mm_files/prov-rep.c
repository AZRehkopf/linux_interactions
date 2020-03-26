#include "common.h"

struct sembuf sem_op;
struct stat file_info;
char *mincore_mem, *addr;
int req_pages, mem_resident, sem_id;
pid_t pid;

void reporter_process() {
    while (1) {
        sem_wait(sem_op, sem_id);

        // Get current time
        time_t mytime = time(NULL);
        char * time_str = ctime(&mytime);
        int i;
        time_str[strlen(time_str)-1] = '\0';
        
        // Display required information
        printf("%s - [INFO]: Reporter process is starting\n", time_str);
        req_pages = file_info.st_size / getpagesize() + 1;
        printf("%s - [INFO]: Page size: %d\n", time_str, getpagesize());
        printf("%s - [INFO]: Required pages: %d\n", time_str, req_pages);
        printf("%s - [INFO]: Below is the current resource state:\n%s\n", time_str, addr);

        unsigned char vec[req_pages];
        mem_resident = mincore((void *)addr, file_info.st_size, vec);
        
        // Determine whether pages are resident in memory
        if (mem_resident == 0) {
            unsigned short int safe = 1;
            
            for (i = 0; i < req_pages; i++) {
                if ((vec[i] & 0x1) != 0) safe = 0;
            }
            if (safe) printf("%s - [INFO]: Page is resident in core (RAM)\n", time_str);
            else printf("%s - [WARNING]: Page is non-resident in core (RAM)\n", time_str);
        
        } else printf("%s - [WARNING]: Page is non-resident in core (RAM)\n", time_str);

        printf("%s - [INFO]: Reporter process is finishing\n", time_str);
        printf("%s - [INFO]: Press 1 at any time to add resources, or 0 to exit\n", time_str);
        
        sem_signal(sem_op, sem_id);
        
        // Wait for 10 seconds before runnning again
        sleep(10);
    }
}

void provider_process() {
    int resource_id, resource_units, selection, file_id;

    // Open file and check for errors
    file_id = open(FILE_NAME, O_RDWR);
    if (file_id == -1) {
        perror("Error: Could not open file\n");
        exit(1);
    }
    
    // Returns statistics about the file and stores them in file_info or throws error
    if (fstat(file_id, &file_info) == -1) { 
        perror("Error: Could not get file statistics alloc\n");
        exit(1);
    }

    while (1) {
        scanf("%d", &selection);
        
        if (selection == 0) {
            kill(pid, SIGTERM);
            break;
        } else if (selection != 0 && selection != 1) {
            continue;
        }
        
        sem_wait(sem_op, sem_id);
        printf("Enter the id of the resource that you would like to allocate more resources to: ");
        scanf("%d", &resource_id);

        // Verify resource id is valid
        if (resource_id > ((int)file_info.st_size / 4)) {
            printf("Error: resource not found");
            sem_signal(sem_op, sem_id);
            continue;
        }

        printf("Units of resource to allocate: ");
        scanf("%d", &resource_units);

        // Limits resource to less than 10
        if ((addr[(resource_id << 2) + 2] + resource_units) < 58) {
            addr[(resource_id << 2) + 2] += resource_units;
            if (msync(addr, file_info.st_size, MS_SYNC) == -1) {
                perror("Error: Could not sync file with memory\n");
                exit(1);
            }
            printf("Success: %d units of resource %d have been added\n", resource_units, resource_id);
        } else {
            printf("Error: Cannot allocate more than 10 units of resource to single recource id\n");
        }

        sem_signal(sem_op, sem_id);
    }
}

int main() {   
    union semun sem_val;
    int file_id;
    
    struct control_info info = init_mmap_file();
    sem_id = info.sem_id;
    addr = info.addr;

    // Fork child to create reporter process
    if ((pid = fork()) < 0) {
        printf("Error: Fork failed\n");
        return 1;
    }
    
    // Spawn provider and reporter based on parent and child
    if (pid == 0) reporter_process();
    else provider_process();
    
    // If user has pressed 0 free memory and exit
    free(mincore_mem);
    return 0;
}