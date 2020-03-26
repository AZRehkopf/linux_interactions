#include "common.h"

int main() {
    int resource_id, resource_quant, file_id;
    struct sembuf sem_op;
    struct stat file_info;
    
    struct control_info info = init_mmap_file();
    int sem_id = info.sem_id;
    char *addr = info.addr;

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
        printf("Enter resource id to access: ");
        scanf("%d", &resource_id);

        // Check if resource exsists
        if (resource_id > ((int)file_info.st_size / 4)) {
            printf("Error: Failed to find resource\n");
            continue;
        }

        printf("Quantity of resource to request: ");
        scanf("%d", &resource_quant);

        sem_wait(sem_op, sem_id);

        // Determine if there is enough of the resource left to satisfy the request
        if ((addr[(resource_id << 2) + 2] - resource_quant) >= 48) {
            addr[(resource_id << 2) + 2] -= resource_quant;
            if (msync(addr, file_info.st_size, MS_SYNC) == -1) {
                perror("Error: Could not sync file with memory");
                exit(1);
            }
            printf("Success: %d units of resource %d have been allocated\n\n", resource_quant, resource_id);
        } else {
            printf("Error: Insufficent units of resource %d to satisfy request\n\n", resource_id);
        }
        
        sem_signal(sem_op, sem_id);
    }

    return 0;
}