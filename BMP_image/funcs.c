/* Градация в серый */
void grayscale(bitmap_t *src, bitmap_t *res, BYTE(*grayscale_method)(BYTE, BYTE, BYTE))
{
    for (int i = 0; i < src->bitmapInfoHeader.biHeight * src->bitmapInfoHeader.biWidth; i++)
    {
        BYTE b = src->imageData.blue[i];
        BYTE g = src->imageData.green[i];
        BYTE r = src->imageData.red[i];
        BYTE color = grayscale_method(b, g, r);
        res->imageData.blue[i] = color;
        res->imageData.green[i] = color;
        res->imageData.red[i] = color;
    }
}

/* Свертка */
float calc_kernel(bitmap_t *bitmap, kernel_t *kernel, int posX, int posY, BYTE *channel)
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
            if (ind_x > bitmap->bitmapInfoHeader.biHeight - 1)
                pixel_pos_x = bitmap->bitmapInfoHeader.biHeight - 1;
            if (ind_y > bitmap->bitmapInfoHeader.biWidth - 1)
                pixel_pos_y = bitmap->bitmapInfoHeader.biWidth - 1;

            sum += kernel->data[i][j] * channel[pixel_pos_x * bitmap->bitmapInfoHeader.biWidth + pixel_pos_y];
            maskSum += kernel->data[i][j];
        }
    }

    return sum / maskSum ;
}


/* Размытие по Гаусу */
int gaussian_blur(bitmap_t *bitmap, bitmap_t *res,  int h, int w)
{
    kernel_t *kernel = get_gaussian_blur_kernel(h, w);
    if(kernel == NULL)
        return ERR_ALLOC_KERNEL;

    convolution(bitmap, res, kernel);

    free_kernel(kernel);

    return EXIT_SUCCESS;
}

/* Оператор Лапласа */
int normalize_laplace(bitmap_t *src, bitmap_t *res, kernel_t *(get_kernel)())
{
    kernel_t *kernel = get_kernel();
    if(kernel == NULL)
        return ERR_ALLOC_KERNEL;

    convolution(src, res, kernel);
    normalize(res);

    free_kernel(kernel);

    return EXIT_SUCCESS;
}


int sobel(bitmap_t *bitmap, bitmap_t *res)
{
    bitmap_t *bx = get_out_bitmap(bitmap);
    bitmap_t *by = get_out_bitmap(bitmap);
    if (bx == NULL || by == NULL)
        return ERR_ALLOC_BITMAP;

    kernel_t *kx =  get_sobel_x_kernel();
    kernel_t *ky = get_sobel_y_kernel();
    convolution(bitmap, bx, kx);
    convolution(bitmap, by, ky);
    merge_bitmaps(bx, by, res, quad_avg);

    free_bitmap(bx);
    free_bitmap(by);

    return EXIT_SUCCESS;
}

/* Выделение границ */
int edge_detection(bitmap_t *src, bitmap_t *res)
{
    bitmap_t *blured = get_out_bitmap(src);

    if (blured == NULL)
        return ERR_ALLOC_KERNEL;

    int rc = EXIT_SUCCESS;

    classic_grayscale(src, src);
    rc = gaussian_blur(src, blured, 1, 1);
    rc = rc || sobel(blured, res);
    if (rc == EXIT_SUCCESS)
    {
        normalize(res);
        apply_func_bitmap(res, threshold);
    }

    return EXIT_SUCCESS;
}
