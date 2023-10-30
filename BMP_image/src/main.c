#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../inc/libfunc.h"


int main(int argc, char **argv)
{
    char *filename;
    char *out_file;
    char *cmd;

    if (argc != 4 && argc != 5)
    {
        printf("Usage: ./app.exe -FLAG IN_FILE OUT_FILE\n");
        printf("  --gr      - grayscale\n");
        printf("  --ga SIZE - gauss blur (matrix SIZE x SIZE)\n");
        printf("  -s        - sobel\n");
        printf("  -l        - laplace\n");
        printf("  -k        - custom kernel\n");
        printf("  -e        - edges\n");
        return EXIT_FAILURE;
    }

    cmd = argv[1];
    filename = argv[2];
    out_file = argv[3];

    if (strcmp(cmd, "--gr") == 0)
        grayscale_file(filename, out_file);
    else if (strcmp(cmd, "-s") == 0)
        sobel_file(filename, out_file);
    else if (strcmp(cmd, "-l") == 0)
        laplace_file(filename, out_file);
    else if (strcmp(cmd, "--ga") == 0)
    {
        char *size_str = argv[2];
        int size = strtol(size_str, NULL, 10);
        filename = argv[3];
        out_file = argv[4];
        gauss_file(filename, out_file, size);
    } else if (strcmp(cmd, "-k") == 0)
        custom_kernel_file(filename, out_file);
    else if (strcmp(cmd, "-e") == 0)
        edges_file(filename, out_file);
    else
        printf("Unknown flag\n");

    return 0;
}
