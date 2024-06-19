#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFFER_SIZE 1024

// ANSI color codes for colored output in the terminal
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

// Function to print the correct usage of the program
void print_usage(const char *prog_name) {
    fprintf(stderr, YELLOW "Usage: %s <source_file> <destination_file>\n" RESET, prog_name);
}

int main(int argc, char *argv[]) {
    // Check if the correct number of arguments are provided
    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Extract source and destination file paths from the arguments
    const char *source_file = argv[1];
    const char *destination_file = argv[2];

    // Open the source file for reading
    int src_fd = open(source_file, O_RDONLY);
    if (src_fd < 0) {
        // Print error message in red if the source file cannot be opened
        fprintf(stderr, RED "Error: Unable to open source file '%s'\n" RESET, source_file);
        perror(RED "open source file" RESET);
        return EXIT_FAILURE;
    }

    // Open (or create) the destination file for writing
    int dest_fd = open(destination_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (dest_fd < 0) {
        // Print error message in red if the destination file cannot be opened or created
        fprintf(stderr, RED "Error: Unable to open/create destination file '%s'\n" RESET, destination_file);
        perror(RED "open/create destination file" RESET);
        close(src_fd);
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];  // Buffer to hold file data during copying
    ssize_t bytes_read, bytes_written;

    // Read from the source file and write to the destination file in chunks
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dest_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            // Print error message in red if writing to the destination file fails
            fprintf(stderr, RED "Error: Unable to write to destination file '%s'\n" RESET, destination_file);
            perror(RED "write to destination file" RESET);
            close(src_fd);
            close(dest_fd);
            return EXIT_FAILURE;
        }
    }

    // Check for read errors after the loop
    if (bytes_read < 0) {
        // Print error message in red if reading from the source file fails
        fprintf(stderr, RED "Error: Unable to read source file '%s'\n" RESET, source_file);
        perror(RED "read source file" RESET);
    }

    // Close the file descriptors
    close(src_fd);
    close(dest_fd);

    // Check if there was a read error
    if (bytes_read < 0) {
        return EXIT_FAILURE;
    }

    // Print success message in green
    printf(GREEN "File '%s' copied to '%s' successfully.\n" RESET, source_file, destination_file);
    return EXIT_SUCCESS;
}