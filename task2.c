
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd;
    char buffer[27];

    fd = open("data.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Open failed");
        return 1;
    }

    const char *text = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (write(fd, text, 26) != 26) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    fd = open("data.txt", O_RDWR);
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
    printf("Original size: %lld bytes\n", (long long)size);

    if (ftruncate(fd, 10) == -1) {
        perror("Truncate failed");
        close(fd);
        return 1;
    }

    size = lseek(fd, 0, SEEK_END);
    if (size == -1) {
        perror("lseek failed");
        close(fd);
        return 1;
    }
    printf("New size: %lld bytes\n", (long long)size);

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("Rewind failed");
        close(fd);
        return 1;
    }

    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n == -1) {
        perror("Read failed");
        close(fd);
        return 1;
    }

    buffer[n] = '\0';
    printf("Remaining content: %s\n", buffer);

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    return 0;
}

