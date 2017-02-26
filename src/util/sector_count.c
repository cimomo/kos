#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char** argv)
{
    struct stat filestat;
    long unsigned int sectors;

    if (argc != 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return 0;
    }
    
    if (stat(argv[1], &filestat) != 0){
        printf("Unable to get the size of %s\n", argv[1]);
        return 1;
    }

    sectors = filestat.st_size / 512;
    
    if (filestat.st_size % 512 != 0) sectors++;

    printf("%lu\n", sectors);
    
    return 0;
}
