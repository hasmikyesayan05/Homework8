
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;

    fd = open("sparse.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Open failed");
        return 1;
    }

    if (write(fd, "START", 5) != 5) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    if (lseek(fd, 1024 * 1024, SEEK_CUR) == -1) {
        perror("lseek failed");
        close(fd);
        return 1;
    }

    if (write(fd, "END", 3) != 3) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    fd = open("sparse.bin", O_RDONLY);
    if (fd == -1) {
        perror("Reopen failed");
        return 1;
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("lseek failed");
        close(fd);
        return 1;
    }

    printf("Apparent file size: %lld bytes\n", (long long)size);

    // A sparse file has an unwritten hole.
    // The filesystem usually does not allocate physical disk
    // blocks for the hole, so du reports less than the
    // apparent file size.

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    return 0;
}

