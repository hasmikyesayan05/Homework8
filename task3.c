
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char filename[256];
    char ch;
    int fd;

    printf("Enter file path: ");
    fflush(stdout);
    if (scanf("%255s", filename) != 1) {
        fprintf(stderr, "Invalid path\n");
        return 1;
    }

    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Open failed");
        return 1;
    }

    off_t size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("lseek failed");
        close(fd);
        return 1;
    }

    for (off_t i = size; i > 0; i--) {
        if (lseek(fd, i - 1, SEEK_SET) == -1) {
            perror("lseek failed");
            close(fd);
            return 1;
        }

        if (read(fd, &ch, 1) != 1) {
            perror("Read failed");
            close(fd);
            return 1;
        }

        if (write(STDOUT_FILENO, &ch, 1) != 1) {
            perror("Write failed");
            close(fd);
            return 1;
        }
    }

    if (write(STDOUT_FILENO, "\n", 1) != 1) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    return 0;
}

