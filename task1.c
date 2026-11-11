
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char source[256], destination[256];
    char buffer[1024];
    int src, dest;
    ssize_t bytesRead;
    long total = 0;

    printf("Enter source file path: ");
    fflush(stdout);
    if (scanf("%255s", source) != 1) {
        fprintf(stderr, "Invalid source path\n");
        return 1;
    }

    printf("Enter destination file path: ");
    fflush(stdout);
    if (scanf("%255s", destination) != 1) {
        fprintf(stderr, "Invalid destination path\n");
        return 1;
    }

    src = open(source, O_RDONLY);
    if (src == -1) {
        perror("Source open failed");
        return 1;
    }

    dest = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest == -1) {
        perror("Destination open failed");
        close(src);
        return 1;
    }

    while ((bytesRead = read(src, buffer, sizeof(buffer))) > 0) {
        ssize_t written = 0;

        while (written < bytesRead) {
            ssize_t n = write(dest, buffer + written,
                              bytesRead - written);
            if (n <= 0) {
                if (n == -1) perror("Write failed");
                else fprintf(stderr, "Write returned zero\n");
                close(src);
                close(dest);
                return 1;
            }
            written += n;
        }

        total += bytesRead;
    }

    if (bytesRead == -1) {
        perror("Read failed");
        close(src);
        close(dest);
        return 1;
    }

    if (close(src) == -1) {
        perror("Close source failed");
        close(dest);
        return 1;
    }
    if (close(dest) == -1) {
        perror("Close destination failed");
        return 1;
    }

    printf("Total bytes copied: %ld\n", total);
    return 0;
}

