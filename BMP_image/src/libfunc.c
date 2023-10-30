#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* __attribute__((aligned)) */
typedef unsigned int BYTE;

#pragma pack(push, 1)
typedef struct {
    uint16_t type;
    uint32_t file_size;
    uint32_t reserved;
    uint32_t off_bits;
} Header;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    uint32_t bSize;
    uint32_t width;
    uint32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t size_image;  // size of img in bytes
    uint32_t xpm;
    uint32_t ypm;
    uint32_t clr_used;
    uint32_t clr_important;
} BmpInfo;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;
#pragma pack(pop)

typedef struct
{
    Header h;
    BmpInfo info;
    /* BYTE *data; */
    BYTE *r;
    BYTE *g;
    BYTE *b;
} Image;


typedef struct
{
    int w;
    int h;
    /* unsigned char **data; */
    float **data;
} Kernel;

Kernel laplace()
{
    Kernel k;
    k.w = 3;
    k.h = 3;
    k.data = malloc(k.h * sizeof(float *));
    for (int i = 0; i < k.h; ++i)
        k.data[i] = malloc(k.w * sizeof(float));

    k.data[0][0] = 0; k.data[0][1] = 1; k.data[0][2] = 0;
    k.data[1][0] = 1; k.data[1][1] = -4; k.data[1][2] = 1;
    k.data[2][0] = 0; k.data[2][1] = 1; k.data[2][2] = 0;

    return k;
}

Kernel laplace_invert()
{
    Kernel k;
    k.w = 3;
    k.h = 3;
    k.data = malloc(k.h * sizeof(float *));
    for (int i = 0; i < k.h; ++i)
        k.data[i] = malloc(k.w * sizeof(float));

    k.data[0][0] = 0; k.data[0][1] = -1; k.data[0][2] = 0;
    k.data[1][0] = -1; k.data[1][1] = 4; k.data[1][2] = -1;
    k.data[2][0] = 0; k.data[2][1] = -1; k.data[2][2] = 0;

    return k;
}

Kernel kernel_sobel_x()
{
    Kernel k;
    k.w = 3;
    k.h = 3;
    k.data = malloc(k.h * sizeof(float *));
    for (int i = 0; i < k.h; ++i)
        k.data[i] = malloc(k.w * sizeof(float));

    k.data[0][0] = -1; k.data[0][1] = 0; k.data[0][2] = 1;
    k.data[1][0] = -2; k.data[1][1] = 0; k.data[1][2] = 2;
    k.data[2][0] = -1; k.data[2][1] = 0; k.data[2][2] = 1;

    return k;
}

Kernel kernel_sobel_y()
{
    Kernel k;
    k.w = 3;
    k.h = 3;
    k.data = malloc(k.h * sizeof(float *));
    for (int i = 0; i < k.h; ++i)
        k.data[i] = malloc(k.w * sizeof(float));

    k.data[0][0] = -1; k.data[0][1] = -2; k.data[0][2] = -1;
    k.data[1][0] = 0;  k.data[1][1] = 0; k.data[1][2] = 0;
    k.data[2][0] = 1;  k.data[2][1] = 2; k.data[2][2] = 1;

    return k;
}


float gg(int x, int y)
{
    float sigma = 10;
    float pi = 3.14;
    return 1/(2*pi*sigma*sigma) * exp(- (x*x + y*y)/(2*sigma*sigma));
}

Kernel gauss3()
{
    Kernel k;
    k.w = 3;
    k.h = 3;
    k.data = malloc(k.h * sizeof(int *));
    for (int i = 0; i < k.h; ++i)
        k.data[i] = malloc(k.w * sizeof(int));

    k.data[0][0] = 1; k.data[0][1] = 1; k.data[0][2] = 1;
    k.data[1][0] = 1; k.data[1][1] = 8; k.data[1][2] = 1;
    k.data[2][0] = 1; k.data[2][1] = 1; k.data[2][2] = 1;

    return k;
}

Kernel gauss(int size)
{
    Kernel k;
    k.w = size;
    k.h = size;
    k.data = malloc(k.h * sizeof(int *));
    for (int i = 0; i < k.h; ++i)
        k.data[i] = malloc(k.w * sizeof(int));

    for (int i = 0; i < k.h; ++i)
        for (int j = 0; j < k.w; ++j)
            k.data[i][j] = gg(k.h / 2 - i, k.w / 2 - j);

    /* k.data[0][0] = 1; k.data[0][1] = 1; k.data[0][2] = 1; k.data[0][3] = 1; k.data[0][4] = 1; */
    /* k.data[1][0] = 1; k.data[1][1] = 1; k.data[1][2] = 1; k.data[1][3] = 1; k.data[1][4] = 1; */
    /* k.data[2][0] = 1; k.data[2][1] = 1; k.data[2][2] = 1; k.data[2][3] = 1; k.data[2][4] = 1; */
    /* k.data[3][0] = 1; k.data[3][1] = 1; k.data[3][2] = 1; k.data[3][3] = 1; k.data[3][4] = 1; */
    /* k.data[4][0] = 1; k.data[4][1] = 1; k.data[4][2] = 1; k.data[4][3] = 1; k.data[4][4] = 1; */

    return k;
}


Image *load(char *filename)
{
    Image *img = malloc(sizeof(Image));

    FILE *f = fopen(filename, "rb");

    fread(&img->h, sizeof(Header), 1, f);

    // verify that this is a .BMP file by checking bitmap id
    if (img->h.type != 0x4D42)
    {
        printf("Incorrect BMP file\n");
        fclose(f);
        return NULL;
    }

    fread(&img->info, sizeof(BmpInfo), 1, f);

    /* img->data = malloc(img->info.size_image); */
    img->r = malloc((img->info.size_image / 3) * sizeof(int));
    img->g = malloc((img->info.size_image / 3) * sizeof(int));
    img->b = malloc((img->info.size_image / 3) * sizeof(int));

    /* printf("Size: %d bytes\n", h.file_size); */
    /* printf("%d\n", h.off_bits); */
    /* printf("width: %d\n", info.width); */

    // move file pointer to the beginning of bitmap data
    fseek(f, img->h.off_bits, SEEK_SET);

    // read in the bitmap image data
    /* fread(img->data, img->info.size_image, 1, f); */
    for (unsigned int i = 0; i < img->info.size_image / 3; ++i)
    {
        fread(&img->r[i], 1, 1, f);
        fread(&img->g[i], 1, 1, f);
        fread(&img->b[i], 1, 1, f);
    }

    fclose(f);
    return img;
}

/* int load_bmp(char *filename, bitmap_t *bitmap) */
/* { */
/*     FILE *file; */
/*     BITMAPFILEHEADER bitmapFileHeader; */
/*     BITMAPINFOHEADER bitmapInfoHeader; */
/*     file = fopen(filename, "r"); */
/*     int rc = EXIT_SUCCESS; */
/*     rc = read_headers(&bitmapFileHeader, &bitmapInfoHeader, file); */

/*     BYTE *blue, *green, *red; */

/*     fseek(file, bitmapFileHeader.bfOffBits, SEEK_SET); */
/*     if ((rc = alloc_channels(&blue, &green, &red, bitmapInfoHeader.biHeight * bitmapInfoHeader.biWidth)) != EXIT_SUCCESS) */
/*     { */
/*         fclose(file); */
/*         return rc; */
/*     } */

/*     rc = read_channels(blue, green, red, bitmapInfoHeader.biHeight * bitmapInfoHeader.biWidth, file); */
/*     fclose(file); */
/*     bitmap->imageData.blue = blue; */
/*     bitmap->imageData.green = green; */
/*     bitmap->imageData.red = red; */
/*     return EXIT_SUCCESS; */
/* } */


void grayscale(Image *src, Image *dst)
{
    for (unsigned int i = 0; i < src->info.size_image / 3 - 1; ++i)
    {
        BYTE r = src->r[i];
        BYTE g = src->g[i];
        BYTE b = src->b[i];

        BYTE gr = (r + g + b) / 3;

        dst->r[i] = gr;
        dst->g[i] = gr;
        dst->b[i] = gr;
    }
}

void save(Image *img, char *filename)
{
    /* printf("Saving img (size: %d bytes, %dx%d)\n", */
    /*        img->h.file_size, */
    /*        img->info.width, */
    /*        img->info.height */
    /*     ); */

    FILE *f = fopen(filename, "w");
    fwrite(&img->h, sizeof(Header), 1, f);
    fwrite(&img->info, sizeof(BmpInfo), 1, f);

    fseek(f, img->h.off_bits, SEEK_SET);

    /* fwrite(img->data, img->info.size_image, 1, f); */
    for (unsigned int i = 0; i < img->info.size_image / 3; ++i)
    {
        fwrite(&img->r[i], 1, 1, f);
        fwrite(&img->g[i], 1, 1, f);
        fwrite(&img->b[i], 1, 1, f);
    }

    fclose(f);
}


/* Header read_header(FILE *f) */
/* { */
/*     Header h; */
/*     fread(&h.type, sizeof(h.type), 1, f); */
/*     fread(&h.file_size, sizeof(h.file_size), 1, f); */
/*     fread(&h.reserved, sizeof(h.reserved), 1, f); */
/*     fread(&h.off_bits, sizeof(h.off_bits), 1, f); */
/*     return h; */
/* } */

/* BmpInfo read_bmp_info(FILE *f) */
/* { */
/*     BmpInfo info; */
/*     fread(&info.bSize, sizeof(info.bSize), 1, f); */
/*     fread(&info.width         , sizeof(info.width)         , 1, f); */
/*     fread(&info.height        , sizeof(info.height)        , 1, f); */
/*     fread(&info.planes        , sizeof(info.planes)        , 1, f); */
/*     fread(&info.bit_count     , sizeof(info.bit_count)     , 1, f); */
/*     fread(&info.compression   , sizeof(info.compression)   , 1, f); */
/*     fread(&info.size_image    , sizeof(info.size_image)    , 1, f); */
/*     fread(&info.xpm           , sizeof(info.xpm)           , 1, f); */
/*     fread(&info.ypm           , sizeof(info.ypm)           , 1, f); */
/*     fread(&info.clr_used      , sizeof(info.clr_used)      , 1, f); */
/*     fread(&info.clr_important , sizeof(info.clr_important) , 1, f); */
/*     return info; */
/* } */

/* Pixel get_px(Image *img, int y, int x) */
/* { */
/*     Pixel p; */
/*     int i = y * img->info.width + x; */
/*     p.r = img->data[i]; */
/*     p.g = img->data[i + 1]; */
/*     p.b = img->data[i + 2]; */
/*     return p; */
/* } */

/* Свертка */
float calc_kernel(Image *bitmap, Kernel *kernel, int posX, int posY, BYTE *channel)
{
    float sum = 0;
    float maskSum = 0;

    for (int i = 0, ind_x = posX - kernel->h / 2; i < kernel->h; i++, ind_x++)
    {
        int pixel_pos_x = ind_x;
        for (int j = 0, ind_y = posY - kernel->w / 2; j < kernel->w; j++, ind_y++)
        {
            int pixel_pos_y = ind_y;

            if (ind_x < 0)
                pixel_pos_x = 0;
            if (ind_y < 0)
                pixel_pos_y = 0;
            if (ind_x > bitmap->info.height - 1)
                pixel_pos_x = bitmap->info.height - 1;
            if (ind_y > bitmap->info.width - 1)
                pixel_pos_y = bitmap->info.width - 1;

            sum += kernel->data[i][j] * channel[pixel_pos_x * bitmap->info.width + pixel_pos_y];
            maskSum += kernel->data[i][j];
        }
    }
    /* printf("%f\n", maskSum); */
    if (maskSum <= 0)
        maskSum = 1;
    return sum / maskSum ;
}

void convolution(Image *src, Image *dst, Kernel *mask)
{
    for (int i = 0; i < src->info.height; i++)
    {
        for (int j = 0; j < src->info.width; j++)
        {
            /* printf("%d, %f\n", */
            /*        dst->b[i * src->info.width + j], */
            /*        calc_kernel(src, mask, i, j, src->b)); */

            float r = calc_kernel(src, mask, i, j, src->r);
            float g = calc_kernel(src, mask, i, j, src->g);
            float b = calc_kernel(src, mask, i, j, src->b);

            if (b < 0)
                b = 0;
            if (b > 255)
                b = 255;

            if (g < 0)
                g = 0;
            if (g > 255)
                g = 255;

            if (r < 0)
                r = 0;
            if (r > 255)
                r = 255;

            dst->r[i * src->info.width + j] = r;
            dst->g[i * src->info.width + j] = g;
            dst->b[i * src->info.width + j] = b;

            /* printf("%d\n", dst->b[i * src->info.width + j]); */
            /* dst->b[i * src->info.width + j] = 0; */
            /* dst->g[i * src->info.width + j] = calc_kernel(src, mask, i, j, src->g); */
            /* dst->r[i * src->info.width + j] = calc_kernel(src, mask, i, j, src->r); */
        }
    }
}

/* Image convolution(Image *img) */
/* { */
/*     Image out; */
/*     out.h = img->h; */
/*     out.info = img->info; */
/*     out.data = malloc(img->info.size_image); */
/*     memcpy(out.data, img->data, img->info.size_image); */

/*     int n = 3; */
/*     int a[3][3] = { */
/*         {-2, -1, 0}, */
/*         {-1,  1, 1}, */
/*         { 0,  1, 2}, */
/*     }; */
/*     /\* int a[n][n]; *\/ */
/*     /\* for (int i = 0; i < n; ++i) *\/ */
/*     /\*     a[i][i] = 1; *\/ */


/*     for (int row = 0; row < img->info.height - n; ++row) */
/*         for (int col = 0; col < img->info.width - n; ++col) */
/*         { */
/*             int i = row * img->info.width + col; */
/*             unsigned char r = img->data[i]; */
/*             unsigned char g = img->data[i + 1]; */
/*             unsigned char b = img->data[i + 2]; */

/*             /\* */
/*               (row, col) - - - - - - (row, col + n) */

/*                               x */

/*               (row + n, col) */
/*              *\/ */

/*             int sum[3] = { 0, 0, 0 }; */
/*             for (int krow = 0; krow < n; ++krow) */
/*                 for (int kcol = 0; kcol < n; ++kcol) */
/*                 { */
/*                     Pixel p = get_px(img, row + krow, col + kcol); */
/*                     sum[0] += p.r * a[krow][kcol]; */
/*                     sum[1] += p.g * a[krow][kcol]; */
/*                     sum[2] += p.b * a[krow][kcol]; */
/*                 } */

/*             for (int j = 0; j < 3; ++j) */
/*             { */
/*                 if (sum[j] > 255) */
/*                     sum[j] = 255; */
/*                 if (sum[j] < 0) */
/*                     sum[j] = 0; */
/*             } */

/*             Pixel p = get_px(img, row + n / 2, col + n / 2); */
/*             i = (row + n / 2) * img->info.width + col + n / 2; */
/*             /\* out.data[i] = sum[0]; *\/ */
/*             /\* out.data[i + 1] = sum[1]; *\/ */
/*             /\* out.data[i + 2] = sum[2]; *\/ */
/*             out.data[i] = p.r; */
/*             out.data[i + 1] = 0; */
/*             out.data[i + 2] = p.r; */
/*         } */
/*     return out; */
/* } */

void sort(BYTE a[100], int len)
{
    for (int i = 0; i < len; ++i)
        for (int j = i + 1; j < len; ++j)
            if (a[j] < a[i])
            {
                int tmp = a[i];
                a[i] = a[j];
                a[j] = tmp;
            }
}

void print_arr(BYTE *a, int len)
{
    printf("%d: ", len);
    for (int i = 0; i < len; ++i)
        printf("%d ", a[i]);
    printf("\n");
}

void print_kernel(Kernel *k)
{
    for (int i = 0; i < k->h; ++i)
    {
        for (int j = 0; j < k->w; ++j)
            printf("%f ", k->data[i][j]);
        printf("\n");
    }
}


int cmpfunc (const void * a, const void * b) {
    return ( *(int*)a - *(int*)b );
}

void median(Image *src, Image *dst, int size)
{
    BYTE r[100];
    BYTE g[100];
    BYTE b[100];

    for (int i = 0; i < src->info.height; i++)
    {
        for (int j = 0; j < src->info.width; j++)
        {
            int x_min = j - size;
            int x_max = j + size;
            int y_min = i - size;
            int y_max = i + size;
            if (x_min < 0)
                x_min = 0;
            if (y_min < 0)
                y_min = 0;

            if (x_max > src->info.width)
                x_max = src->info.width - 1;
            if (y_max >= src->info.height)
                y_max = src->info.height - 1;

            /* int k = (x_max - x_min) * (y_max - y_min); */
            /* if (k == 0) */
            /*     k = 1; */


            int k = 0;

            for (int y = y_min; y <= y_max; ++y)
                for (int x = x_min; x <= x_max; ++x)
                {
                    r[k] = src->r[y * src->info.width + x];
                    g[k] = src->g[y * src->info.width + x];
                    b[k] = src->b[y * src->info.width + x];
                    k++;
                }

            /* for (int x = x_min; x <= x_max; ++x) */
            /* { */
            /*     r[k] = src->r[i * src->info.width + x]; */
            /*     g[k] = src->g[i * src->info.width + x]; */
            /*     b[k] = src->b[i * src->info.width + x]; */
            /*     k++; */
            /* } */

            if (k == 0)
            {
                /* r[] */
                k = 1;
            }

            qsort(r, k, sizeof(int), cmpfunc);
            qsort(g, k, sizeof(int), cmpfunc);
            qsort(b, k, sizeof(int), cmpfunc);

            /* sort(r, k); */
            /* sort(g, k); */
            /* sort(b, k); */

            /* print_arr(r, k); */


            /* int s[3] = { 0, 0, 0 }; */
            /* k = 0; */
            /* for (int y = y_min; y <= y_max; ++y) */
            /*     for (int x = x_min; x <= x_max; ++x) */
            /*     { */
            /*         s[0] += src->r[y * src->info.width + x]; */
            /*         s[1] += src->g[y * src->info.width + x]; */
            /*         s[2] += src->b[y * src->info.width + x]; */
            /*         k++; */
            /*     } */
            /* if (k == 0) */
            /*     k = 1; */
            /* dst->r[i * src->info.width + j] = s[0] / k; */
            /* dst->g[i * src->info.width + j] = s[1] / k; */
            /* dst->b[i * src->info.width + j] = s[2] / k; */

            dst->r[i * src->info.width + j] = r[k / 2];
            dst->g[i * src->info.width + j] = g[k / 2];
            dst->b[i * src->info.width + j] = b[k / 2];
        }
    }
}


void sobel(Image *src, Image *Gx, Image *Gy, Image *G)
{
    Kernel sobel_x = kernel_sobel_x();
    Kernel sobel_y = kernel_sobel_y();
    convolution(src, Gx, &sobel_x);
    convolution(src, Gy, &sobel_y);
    for (uint32_t i = 0; i < src->info.height; i++)
    {
        for (uint32_t j = 0; j < src->info.width; j++)
        {
            int p = i * src->info.width + j;
            G->r[p] = sqrt(Gx->r[p]*Gx->r[p] + Gy->r[p]*Gy->r[p]);
            G->g[p] = sqrt(Gx->g[p]*Gx->g[p] + Gy->g[p]*Gy->g[p]);
            G->b[p] = sqrt(Gx->b[p]*Gx->b[p] + Gy->b[p]*Gy->b[p]);

            if (G->r[p] > 255)
                G->r[p] = 255;
            if (G->g[p] > 255)
                G->g[p] = 255;
            if (G->b[p] > 255)
                G->b[p] = 255;
        }
    }
}


void sobel_file(char *filename, char *out_filename)
{
    Image *src = load(filename);
    /* Image *dst = load(filename); */
    Image *Gx = load(filename);
    Image *Gy = load(filename);
    Image *G = load(filename);
    sobel(src, Gx, Gy, G);
    save(G, out_filename);
}

void laplace_file(char *filename, char *out_filename)
{
    Image *src = load(filename);
    Image *dst = load(filename);

    Kernel kernel_laplace = laplace();
    /* Kernel kernel_laplace_invert = laplace_invert(); */
    /* Kernel kernel_gauss = gauss(5); */
    convolution(src, dst, &kernel_laplace);

    save(dst, out_filename);
}

void gauss_file(char *filename, char *out_filename, int size)
{
    Image *src = load(filename);
    Image *dst = load(filename);

    /* Kernel kernel_laplace = laplace(); */
    /* Kernel kernel_laplace_invert = laplace_invert(); */
    Kernel kernel_gauss = gauss(size);
    /* print_kernel(&kernel_gauss); */

    convolution(src, dst, &kernel_gauss);
    save(dst, out_filename);
}

Kernel read_custom_kernel()
{
    Kernel k;
    printf("Enter kernel width and height: ");
    scanf("%d %d", &k.w, &k.h);
    printf("Enter %d numbers:\n", k.w * k.h);
    k.data = malloc(k.h * sizeof(float *));
    for (int i = 0; i < k.h; ++i)
        k.data[i] = malloc(k.w * sizeof(float));

    for (int i = 0; i < k.h; ++i)
        for (int j = 0; j < k.w; ++j)
            scanf("%f", &k.data[i][j]);

    return k;
}

void custom_kernel_file(char *filename, char *out_filename)
{
    Image *src = load(filename);
    Image *dst = load(filename);
    Kernel kernel = read_custom_kernel();
    convolution(src, dst, &kernel);
    save(dst, out_filename);
}


void edges_file(char *filename, char *out_filename)
{
    Image *src = load(filename);
    Image *dst = load(filename);

    grayscale(src, src);

    Image *blur = load(filename);
    Kernel kernel_gauss = gauss(5);
    convolution(src, blur, &kernel_gauss);

    Image *Gx = load(filename);
    Image *Gy = load(filename);
    sobel(src, Gx, Gy, dst);

    save(dst, out_filename);
}

/* int main() */
/* { */
/*     /\* char *filename = "e.bmp"; *\/ */
/*     char *filename = "Lenna.bmp"; */
/*     /\* char *filename = "m.bmp"; *\/ */
/*     Image *src = load(filename); */
/*     Image *dst = load(filename); */

/*     /\* grayscale(src, dst); *\/ */

/*     Kernel kernel_laplace = laplace(); */
/*     Kernel kernel_laplace_invert = laplace_invert(); */
/*     Kernel kernel_gauss = gauss(5); */


/*     /\* convolution(src, dst, &kernel_laplace_invert); *\/ */
/*     /\* median(src, dst, 3); *\/ */


/*     /\* Image out = convolution(img); *\/ */

/*     /\* save(dst, "Lenna-grayscale.bmp"); *\/ */
/*     save(G, "Lenna-grayscale.bmp"); */
/* } */



void grayscale_file(char *src_filename, char *dst_filename)
{
    printf("Load: %s\n", src_filename);
    printf("Save: %s\n", dst_filename);
    Image *src = load(src_filename);
    Image *dst = load(src_filename);
    grayscale(src, dst);
    save(dst, dst_filename);
}
