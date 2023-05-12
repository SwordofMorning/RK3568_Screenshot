#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <png.h>

#define FB "/dev/fb0"
#define Screen_Width 1280
#define Screen_Height 800

int main()
{
    // Open the framebuffer device
    int fbfd = open(FB, O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    // Get the fixed screen information
    struct fb_fix_screeninfo fixed_info;
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fixed_info) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get the variable screen information
    struct fb_var_screeninfo var_info;
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    var_info.xres = Screen_Width;
    var_info.yres = Screen_Height;
    var_info.xres_virtual = Screen_Width;
    var_info.yres_virtual = Screen_Height;

    // Calculate the size to mmap
    size_t screen_size = var_info.yres_virtual * fixed_info.line_length;
    void* fbptr = mmap(NULL, screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbptr == MAP_FAILED) {
        perror("Error: failed to mmap framebuffer");
        exit(4);
    }

    // Allocate memory for the PPM image
    char* pixels = (char*)malloc(var_info.xres * var_info.yres * 3);

    // Copy the framebuffer pixels into the PPM image
    for (int y = 0; y < var_info.yres; y++) {
        for (int x = 0; x < var_info.xres; x++) {
            char* fb_pixel = (char*)fbptr + y * fixed_info.line_length + x * 4;
            char* pixel = pixels + (y * var_info.xres + x) * 3;
            pixel[0] = fb_pixel[2];  // Red
            pixel[1] = fb_pixel[1];  // Green
            pixel[2] = fb_pixel[0];  // Blue
        }
    }

/*
    // Write the PPM image to a file
    FILE* outfile = fopen("screenshot.ppm", "wb");
    fprintf(outfile, "P6\n%d %d\n255\n", var_info.xres, var_info.yres);
    fwrite(pixels, 1, var_info.xres * var_info.yres * 3, outfile);
    fclose(outfile);

*/

    // Create a PNG file
    FILE* outfile = fopen("screenshot.png", "wb");
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, outfile);
    png_set_IHDR(png_ptr, info_ptr, var_info.xres, var_info.yres, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png_ptr, info_ptr);
    png_bytep row_pointers[var_info.yres];
    for (int y = 0; y < var_info.yres; y++) {
        row_pointers[y] = (png_bytep)&pixels[y * var_info.xres * 3];
    }
    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, NULL);
    fclose(outfile);
    png_destroy_write_struct(&png_ptr, &info_ptr);

    // Free resources
    munmap(fbptr, screen_size);
    free(pixels);
    close(fbfd);

    return 0;
}