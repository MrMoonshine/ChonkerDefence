#include <texture.h>

static const char* TAG = "Texture load";
int texture_create(Texture* texture, const char* filename){
    FILE* fp;
    fp = fopen(filename, "r");
    if(!fp){
        fprintf(stderr,"\e[0;31m[ERROR] %s: Unable to open file %s\e[0m\n", TAG, filename);
        return -1;
    }

    //int is_png;
    int bit_depth, color_type, row_bytes, i;
    png_infop info_ptr, end_info;
    png_uint_32 t_width, t_height;
    png_byte header[8], *image_data;
    png_bytepp row_pointers;
    png_structp png_ptr;
    //GLuint texture;
    int alpha;
    // Check if PNG file
    fread( header, 1, 8, fp);
    if(png_sig_cmp(header, 0, 8)){ // Returns zero if the supplied bytes match the 8-byte PNG * signature, and non-zero otherwise.
        fclose(fp);
        fprintf(stderr,"\e[0;31m[ERROR] %s: %s is not a PNG file!\e[0m\n", TAG, filename);
        return -1;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr){
        fclose(fp);
        fprintf(stderr,"\e[0;31m[ERROR] %s: png_ptr png_create_read_struct | %s\e[0m\n", TAG, filename);
        return -2;
    }

    info_ptr = png_create_info_struct( png_ptr );
    if ( !info_ptr ) {
        png_destroy_read_struct( &png_ptr, (png_infopp) NULL, (png_infopp) NULL );
        fclose(fp);
        fprintf(stderr,"\e[0;31m[ERROR] %s: info_ptr png_create_info_struct | %s\e[0m\n", TAG, filename);
        return -3;
    }

    end_info = png_create_info_struct( png_ptr );
    if ( !end_info ) {
        fprintf(stderr,"\e[0;31m[ERROR] %s: end_info png_create_info_struct | %s\e[0m\n", TAG, filename);
        png_destroy_read_struct( &png_ptr, (png_infopp) NULL, (png_infopp) NULL );
        fclose(fp);
        return -4;
    }

    if ( setjmp( png_jmpbuf( png_ptr ) ) ) {
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
        fclose(fp);
        fprintf(stderr,"\e[0;31m[ERROR] %s: setjmp(png_jmpbuf) | %s\e[0m\n", TAG, filename);
        return -5;
    }

    png_init_io( png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);
    png_get_IHDR( png_ptr, info_ptr, &t_width, &t_height, &bit_depth, &color_type, NULL, NULL, NULL );

    texture->width = t_width;
    texture->height = t_height;

    // Convert Palette buffers to RGBA to allow loading by opengl
    if(PNG_COLOR_TYPE_PALETTE == png_get_color_type( png_ptr, info_ptr)){
        png_set_palette_to_rgb(png_ptr);
    }

    png_read_update_info( png_ptr, info_ptr );
    row_bytes = png_get_rowbytes( png_ptr, info_ptr );
    image_data = (png_bytep) malloc( row_bytes * t_height * sizeof(png_byte));
    if ( !image_data ) {
        fprintf(stderr,"\e[0;31m[ERROR] %s: Malloc image_data %s\e[0m\n", TAG, filename);
        png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
        fclose(fp);
        return -6;
    }

    row_pointers = (png_bytepp) malloc( t_height * sizeof(png_bytep) );
    if ( !row_pointers ) {
        fprintf(stderr,"\e[0;31m[ERROR] %s: Malloc row_pointers %s\e[0m\n", TAG, filename);
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        free(image_data);
        fclose(fp);
        return -7;
    }

    for ( i = 0; i < t_height; ++i ) {
        row_pointers[t_height - 1 - i] = image_data + i * row_bytes;
    }

    png_read_image( png_ptr, row_pointers );
    switch ( png_get_color_type( png_ptr, info_ptr ) ) {
        case PNG_COLOR_TYPE_RGBA:
            alpha = GL_RGBA;
            break;
        case PNG_COLOR_TYPE_RGB:
            alpha = GL_RGB;
            break;
        case PNG_COLOR_TYPE_PALETTE:
            alpha = GL_RGBA;
            break;
        default:
            fprintf(stderr,"\e[0;31m[ERROR] %s: Color type %d not supported!\e[0m\n", TAG, png_get_color_type( png_ptr, info_ptr));
            png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
            return -8;
    }

    GLuint textureID;
	glGenTextures(1, &textureID);
    glBindTexture( GL_TEXTURE_2D, textureID );
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, t_width, t_height, 0, alpha, GL_UNSIGNED_BYTE, (GLvoid *) image_data );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    /*Cleanup*/
    png_destroy_read_struct( &png_ptr, &info_ptr, &end_info );
    free( image_data );
    free( row_pointers );

    fclose(fp);
    texture->bufferID = textureID;
    return 0;
}

void texture_destroy(Texture* texture){
    glDeleteTextures(1, &texture->bufferID);
}
