#ifndef LIBFUNC_H
#define LIBFUNC_H

void grayscale_file(char *src_filename, char *dst_filename);
void sobel_file(char *filename, char *out_filename);
void laplace_file(char *filename, char *out_filename);
void gauss_file(char *filename, char *out_filename, int size);
void custom_kernel_file(char *filename, char *out_filename);
void edges_file(char *filename, char *out_filename);

#endif /* LIBFUNC_H */
