/* VulcanFS is a initrd file system generator written in C
* for the VulcanOS kernel. This software is a slight different
* version of James Molloy's initrd generator(http://www.jamesmolloy.co.uk/).
* Here, a brief explanation of this design(more on the README):
*   The first 64bits(8bytes) are being used to store the number of files on the disk
*   The following space is being used by a fixed number(128) of headers(i.e. name, length, offset and magic number of each file)
*   The rest of the space is used to store the actual file data.
* Developed by Marco C. <ceticamarco@gmail.com> 2021
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#define MAGIC_NUMBER 0xECC
#define INITRD_FILENAME "initrd.img"

struct vulcanfs_header {
    char name[64]; // Name of file
    unsigned int length; // Length of file
    unsigned int offset; // Offset from previous file
    unsigned short magic_number; // Magic number for consistenty check
};

struct vulcanfs_header setup_header(struct vulcanfs_header header, const char *path, const char *name, unsigned int *ofs) {
    printf("Writing local file '%s' as '%s' at 0x%d.\n", path, name, *ofs);
    // Set name of the file
    strcpy(header.name, name); 
    // Set length of the file
    FILE *local_file_stream = fopen(path, "r");
    if(local_file_stream == NULL) {
        printf("Error while opening '%s'. Reason: %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fseek(local_file_stream, 0, SEEK_END); // Set pointer at the end of the stream
    header.length = ftell(local_file_stream); // Compute file length using the pointer
    // Set offset of the file
    header.offset = *ofs;
    *ofs += header.length;
    // Set magic number
    header.magic_number = MAGIC_NUMBER;
    // Close file
    fclose(local_file_stream);

    return header;
}

void helper() {
    puts("VulcanFS is a initrd file system generator for VulcanOS\n"
        "-f,--file | Specify local file(s)\n"
        "-n,--name | Specify remote file name\n"
        "-h,--help | Show this helper");
}

int main(int argc, char **argv) {
    // Check number of parameters
    if(argc < 2) {
        puts("Error, pass argv[1] something(-h for the helper)");
        return -1;
    }

    int opt;
    const char *short_opts = "f:n:h"; // Colon means additional value
    struct option long_opts[] = {
        {"file", required_argument, NULL, 'f'},
        {"name", required_argument, NULL, 'n'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    // Default values
    char *file_path[128], *file_name[128]; // Arrays to store arguments values.
    int local_count = 0, remote_count = 0; // Parameters counter

    while((opt = getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
        switch (opt) {
        case 'f':
            file_path[local_count++] = optarg; // Retrieve argument' values into array
            break;

        case 'n':
            file_name[remote_count++] = optarg; // Retrieve argument' values into array
            break;
        
        case 'h':
            helper();
            return 0;

        case ':':
        case '?':
        default:
            printf("Use '%s -h' to read the helper.\n", argv[0]);
            return 1;
        }
    }

    // If counters are not equals, it means that one argument has been called
    // more times than the other
    if(local_count == remote_count) {
        struct vulcanfs_header vfs_h[128]; // Maximum amount of files
        unsigned int ofs = sizeof(struct vulcanfs_header) * 64 + sizeof(int); // Prepare offset
        for(int i = 0; i < local_count; i++) // For each file, add metadata to header
            vfs_h[i] = setup_header(vfs_h[i], file_path[i], file_name[i], &ofs);
        
        // Write number of files and headers on file system image
        FILE *initrd_stream = fopen(INITRD_FILENAME, "w");
        if(initrd_stream == NULL) {
            printf("Error while opening '%s'. Reason: %s\n", INITRD_FILENAME, strerror(errno));
            exit(EXIT_FAILURE);
        }
        fwrite(&local_count, sizeof(long), 1, initrd_stream); // Write number of files
        fwrite(&vfs_h, sizeof(struct vulcanfs_header), 128, initrd_stream); // Write initrd

        // Finally, write file content at the end of the file system
        for(int i = 0; i < local_count; i++) {
            FILE *local_file_stream = fopen(file_path[i], "r");
            if(local_file_stream == NULL) { // Open local file
                printf("Error while opening '%s'. Reason: %s\n", file_path[i], strerror(errno));
                exit(EXIT_FAILURE);
            }
            // Allocate a buffer for copying
            unsigned char *copy_buffer = (unsigned char*)malloc(vfs_h[i].length);
            fread(copy_buffer, 1, vfs_h[i].length, local_file_stream); // COpy file content to buffer
            fwrite(copy_buffer, 1, vfs_h[i].length, initrd_stream); // Write buffer to initrd file stream
            // Free the buffer and close local file
            free(copy_buffer);
            fclose(local_file_stream);
        }
        // CLose initrd stream
        fclose(initrd_stream);

    } else {
        puts("Please, pass the same amount of values per arguments.");
        return 1;
    }
    return 0;
}