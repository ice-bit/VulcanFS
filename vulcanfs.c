/* VulcanFS is a initrd file system generator written in C
* for the VulcanOS kernel. This software is a slight different
* version of James Molloy's initrd generator(http://www.jamesmolloy.co.uk/).
* Here, a brief explanation of this design(more on the README):
*   The first 64bits(8bytes) are being used to store the number of files on the disk
*   The following space is being used by a fixed number(128) of headers(i.e. name, length, offset and magic number of each file)
*   The rest of the space is used to store the actual file data.
* Developed by Marco C. <ceticamarco@gmail.com>
*
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

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
    size_t local_count = 0, remote_count = 0; // Parameters counter

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
        
    } else {
        puts("Please, pass the same amount of values per arguments.");
        return 1;
    }

    // printf("Local files: ");
    // for(size_t i = 0; i < local_count; i++)
    //     printf("%s ", file_path[i]);

    // printf("\nRemote files: ");
    // for(size_t i = 0; i < remote_count; i++)
    //     printf("%s ", file_name[i]);

    // puts("");

    return 0;
}