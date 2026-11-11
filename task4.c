
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main() {
    int fd;
    char line[256];
    char prefix[64];

    fd = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("Open failed");
        return 1;
    }

    printf("Enter log message: ");
    fflush(stdout);

    ssize_t n = read(STDIN_FILENO, line, sizeof(line) - 1);
    if (n == -1) {
        perror("Read failed");
        close(fd);
        return 1;
    }

    line[n] = '\0';

    if (n > 0 && line[n - 1] == '\n') {
        line[--n] = '\0';
    }

    int prefixLen = snprintf(prefix, sizeof(prefix),
                             "PID=%ld: ", (long)getpid());
    if (prefixLen < 0 || prefixLen >= (int)sizeof(prefix)) {
        fprintf(stderr, "Prefix formatting failed\n");
        close(fd);
        return 1;
    }

    char result[sizeof(prefix) + sizeof(line) + 1];
    size_t total = (size_t)prefixLen;

    memcpy(result, prefix, total);
    memcpy(result + total, line, (size_t)n);
    total += (size_t)n;
    result[total++] = '\n';

    size_t written = 0;
    while (written < total) {
        ssize_t w = write(fd, result + written, total - written);
        if (w <= 0) {
            if (w == -1) perror("Write failed");
            else fprintf(stderr, "Write returned zero\n");
            close(fd);
            return 1;
        }
        written += (size_t)w;
    }

    // O_APPEND moves the offset to EOF before each write.
    // After writing, SEEK_CUR therefore reports the updated
    // offset, which grows as entries are appended.
    off_t position = lseek(fd, 0, SEEK_CUR);
    if (position == -1) {
        perror("lseek failed");
        close(fd);
        return 1;
    }

    printf("Final file offset: %lld\n", (long long)position);

    if (close(fd) == -1) {
        perror("Close failed");
        return 1;
    }

    return 0;
}

