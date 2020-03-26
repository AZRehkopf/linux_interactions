// file_trav.cpp
// Recursively traverses a given directory and prints information about the contained files

#include "file_trav.h"

// Funciton to show usage rules when incorrect args are passed
void show_help() {
    printf("Usage file_trav [entry path]\n");
}

// Changes working directory to a given path and checks for any errors
int change_dir(string path) {
    char char_path[path.length()];
    strcpy(char_path, path.c_str());
    
    if (chdir(char_path) != 0) {
        perror("Error chdir");
        return 1;
    }

    return 0;
}

// Displays information about a given file in the CWD, adapted from an example funtion in the linux documentation
int show_file_info(string file) {
    struct stat file_info;
    char file_name[file.length()];
    strcpy(file_name, file.c_str()); 
    int dir_flag = 0;
    
    // lstat file and print error if lstat fails
    if (lstat(file_name, &file_info) == -1) {
        perror("lstat");
    } else {
        struct passwd pwd;
        struct passwd *result;
        struct group *group_res;
        char *buf;
        size_t bufsize;
        int s;
        

        bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
        if (bufsize == -1)          
            bufsize = 16384;        

        buf = (char *)malloc(bufsize);
        if (buf == NULL) {
            perror("malloc");
        }

        // Get username from UID perror if fails 
        s = getpwuid_r(file_info.st_uid, &pwd, buf, bufsize, &result);
        if (result == NULL) {
            if (s == 0)
                printf("Not found\n");
            else {
                errno = s;
                perror("getpwnam_r");
            }
        }

        // Get group name from GID perror if fails
        group_res = getgrgid(file_info.st_gid);
        if (group_res == NULL) {
            perror("getgrgid");
        }
        
        // Print required info abpupt the file
        printf("\nFile type:                ");
        switch (file_info.st_mode & S_IFMT) {
        case S_IFBLK:  printf("block device\n");            break;
        case S_IFCHR:  printf("character device\n");        break;
        case S_IFDIR:  
            // If filetype is directory set a flag so the function can be called recursively
            printf("directory\n");
            // Ignore '.' and '..' to avoid infinte recursion and segfault
            if (strncmp(file_name, ".", 1) == 0) {
                dir_flag = 0;  
            } else if (strncmp(file_name, "..", 2) == 0) {
                dir_flag = 0;
            } else {
                dir_flag = 1;
            }
            break;
        case S_IFIFO:  printf("FIFO/pipe\n");               break;
        case S_IFLNK:  printf("symlink\n");                 break;
        case S_IFREG:  printf("regular file\n");            break;
        case S_IFSOCK: printf("socket\n");                  break;
        default:       printf("unknown?\n");                break;
        }
        printf("File Name:                %s\n", file_name);
        printf("Mode:                     %lo\n", (unsigned long) file_info.st_mode);
        printf("Link count:               %ld\n", (long) file_info.st_nlink);
        printf("Owner User Name:          %s\n", result->pw_name);
        printf("Owner Group Name:         %s\n", group_res->gr_name);
        printf("Owner IDs:                UID=%ld   GID=%ld\n",(long) file_info.st_uid, (long) file_info.st_gid);
        printf("File size:                %lld bytes\n", (long long) file_info.st_size);
        printf("Blocks allocated:         %lld\n", (long long) file_info.st_blocks);
        printf("Last file modification:   %s\n", ctime(&file_info.st_mtime));
    }

    // Return whether or not this file was a directory
    return dir_flag;
}

// Loop through files in a directory
void scan_dir(string dir_str) {
    DIR *dir;
    struct dirent *dp;
    char dir_name[dir_str.length()];
    strcpy(dir_name, dir_str.c_str());

    // Open the direcotry stream and print an error if it fails
    if ((dir = opendir (dir_name)) == NULL) {
        perror("Error opendir");
    }

    // While there are files left in the directory stream print info about them
    while ((dp = readdir (dir)) != NULL) {
        int dir_flag = show_file_info(dp->d_name);
        
        // If the file was a directory call scan_dir on that directory
        if (dir_flag) {
            long size;
            char *buf;
            char *ptr;

            size = pathconf(".", _PC_PATH_MAX);

            if ((buf = (char *)malloc((size_t)size)) != NULL)
                ptr = getcwd(buf, (size_t)size);

            string current_dir(ptr);
            
            string new_target_dir;
            new_target_dir.assign(current_dir);
            new_target_dir.append("/").append(dp->d_name);
            
            // Change working direcotory to the new directory
            change_dir(new_target_dir);
            // Scan the new directory
            scan_dir(new_target_dir);
            // Channge the working directory back and continue printing info
            change_dir(current_dir);
        }
    }

    // Close the directory stream once all files have been checked
    (void) closedir(dir);
}

int main(int argc, char const *argv[]) {   
    string entry_path = "";
    
    // Make sure arguments passes are valid
    if (argc == 1) {
        printf("Error: must specify entry location\n");
        show_help();
        exit(1);
    } else if (argc == 2) {
        entry_path = argv[1];
    } else {
        printf("Error: too many arguments\n");
        show_help();
        exit(1);
    }

    // If it cannot change the CWD to specified path, then the path does not exsist or we do not have permission 
    if (change_dir(entry_path) != 0) {
        show_help();
        // Exit because we cannot continue if the inital path is invalid
        exit(1);
    }
    
    // Start scanning the directory 
    scan_dir(entry_path);

    return 0;
}