
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char buffer[100];
    char ch;
    off_t position = 0;
    int newlines = 0;

    fd = open("numbers.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Open failed");
        return 1;
    }

    for (int i = 1; i <= 10; i++) {
        char line[10];
        int len = snprintf(line, sizeof(line), "%d\n", i);
        if (len < 0 || len >= (int)sizeof(line)) {
            fprintf(stderr, "Formatting failed\n");
            close(fd);
            return 1;
        }
        if (write(fd, line, len) != len) {
            perror("Write failed");
            close(fd);
            return 1;
        }
    }

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    fd = open("numbers.txt", O_RDWR);
    if (fd == -1) {
        perror("Reopen failed");
        return 1;
    }

    // Scan to find the start of line 4.
    while (newlines < 3) {
        ssize_t n = read(fd, &ch, 1);
        if (n != 1) {
            fprintf(stderr, "Could not find line 4\n");
            close(fd);
            return 1;
        }

        position++;
        if (ch == '\n')
            newlines++;
    }

    off_t line4 = position;

    // Skip the original fourth line.
    do {
        ssize_t n = read(fd, &ch, 1);
        if (n != 1) {
            fprintf(stderr, "Could not read line 4\n");
            close(fd);
            return 1;
        }
    } while (ch != '\n');

    // Read the remainder after line 4.
    ssize_t remaining = read(fd, buffer, sizeof(buffer));
    if (remaining == -1) {
        perror("Read failed");
        close(fd);
        return 1;
    }

    if (lseek(fd, line4, SEEK_SET) == -1) {
        perror("lseek failed");
        close(fd);
        return 1;
    }

    if (write(fd, "100\n", 4) != 4) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    if (write(fd, buffer, remaining) != remaining) {
        perror("Write failed");
        close(fd);
        return 1;
    }

    off_t end = lseek(fd, 0, SEEK_CUR);
    if (end == -1 || ftruncate(fd, end) == -1) {
        perror("Resize failed");
        close(fd);
        return 1;
    }

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("Rewind failed");
        close(fd);
        return 1;
    }

    printf("Final file content:\n");

    ssize_t n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(STDOUT_FILENO,
                              buffer + written, n - written);
            if (w <= 0) {
                perror("Output failed");
                close(fd);
                return 1;
            }
            written += w;
        }
    }

    if (n == -1) {
        perror("Read failed");
        close(fd);
        return 1;
    }

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    return 0;
}

