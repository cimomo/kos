#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main()
{
    unsigned char buffer[512];
    int floppy_desc, file_desc;

    file_desc = open("./boot", O_RDONLY);

    if(file_desc<0){
        printf("Error loading file.\n");
        exit(1);
    }

    read(file_desc, buffer, 512);
    close(file_desc);

    floppy_desc = open("/dev/fd0", O_RDWR);

    if(floppy_desc<0){
        printf("Error opening floppy.\n");
        exit(1);
    }

    lseek(floppy_desc, 0, SEEK_CUR);
    write(floppy_desc, buffer, 512);
    close(floppy_desc);
}
