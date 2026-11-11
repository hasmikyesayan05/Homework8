
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char file1[256], file2[256];
    unsigned char buf1[1024], buf2[1024];
    int fd1, fd2;
    long long offset = 0;

    printf("Enter first file path: ");
    fflush(stdout);
    if (scanf("%255s", file1) != 1)
        return 1;

    printf("Enter second file path: ");
    fflush(stdout);
    if (scanf("%255s", file2) != 1)
        return 1;

    fd1 = open(file1, O_RDONLY);
    if (fd1 == -1) {
        perror("First file open failed");
        return 1;
    }

    fd2 = open(file2, O_RDONLY);
    if (fd2 == -1) {
        perror("Second file open failed");
        close(fd1);
        return 1;
    }

    while (1) {
        // Fill each buffer up to 1024 bytes or EOF.
        ssize_t n1 = 0, n2 = 0;

        while (n1 < sizeof(buf1)) {
            ssize_t r = read(fd1, buf1 + n1, sizeof(buf1) - n1);
            if (r == -1) {
                perror("Read first file failed");
                close(fd1);
                close(fd2);
                return 1;
            }
            if (r == 0) break;
            n1 += r;
        }

        while (n2 < sizeof(buf2)) {
            ssize_t r = read(fd2, buf2 + n2, sizeof(buf2) - n2);
            if (r == -1) {
                perror("Read second file failed");
                close(fd1);
                close(fd2);
                return 1;
            }
            if (r == 0) break;
            n2 += r;
        }

        ssize_t min = n1 < n2 ? n1 : n2;

        for (ssize_t i = 0; i < min; i++) {
            if (buf1[i] != buf2[i]) {
                printf("Files differ at byte %lld\n", offset + i);
                close(fd1);
                close(fd2);
                return 1;
            }
        }

        if (n1 != n2) {
            printf("Files differ at byte %lld\n", offset + min);
            close(fd1);
            close(fd2);
            return 1;
        }

        if (n1 == 0) break;

        offset += n1;
    }

    if (close(fd1) == -1) {
        perror("Close first file failed");
        close(fd2);
        return 1;
    }
    if (close(fd2) == -1) {
        perror("Close second file failed");
        return 1;
    }

    printf("Files are identical\n");
    return 0;
}

