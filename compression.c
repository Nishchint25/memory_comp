#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>       
#include <zlib.h>         

#define CHUNK 16384       // Define constant CHUNK with value 16384

// Function to compress a file
int compress_file(const char *source, const char *dest) {
    FILE *source_file = fopen(source, "rb");      // Open source file in read-binary mode
    FILE *dest_file = fopen(dest, "wb");          // Open destination file in write-binary mode

    // Check if file opening failed
    if (!source_file || !dest_file) {
        perror("Error opening files");            // Print error message
        return -1;                                // Return -1 to indicate failure
    }

    z_stream stream;                              // Declare zlib compression stream
    stream.zalloc = Z_NULL;                       // Set memory allocation function to NULL
    stream.zfree = Z_NULL;                        // Set memory free function to NULL
    stream.opaque = Z_NULL;                       // Set opaque value to NULL

    // Initialize deflate stream with best compression level
    if (deflateInit(&stream, Z_BEST_COMPRESSION) != Z_OK) {
        perror("deflateInit failed");             // Print error message
        return -1;                                // Return -1 to indicate failure
    }

    unsigned char in[CHUNK];                      // Declare input buffer
    unsigned char out[CHUNK];                     // Declare output buffer

    int ret;
    do {
        // Read CHUNK bytes from source file into input buffer
        stream.avail_in = fread(in, 1, CHUNK, source_file);
        // Check for read error
        if (ferror(source_file)) {
            deflateEnd(&stream);                   // Clean up compression stream
            fclose(source_file);                   // Close source file
            fclose(dest_file);                     // Close destination file
            return -1;                            // Return -1 to indicate failure
        }
        stream.next_in = in;                       // Set input buffer pointer

        do {
            stream.avail_out = CHUNK;             // Set available space in output buffer
            stream.next_out = out;                // Set output buffer pointer
            // Compress data from input buffer to output buffer
            ret = deflate(&stream, Z_FINISH);
            // Check for compression error
            if (ret == Z_STREAM_ERROR) {
                deflateEnd(&stream);               // Clean up compression stream
                fclose(source_file);               // Close source file
                fclose(dest_file);                 // Close destination file
                return -1;                        // Return -1 to indicate failure
            }
            // Write compressed data to destination file
            fwrite(out, 1, CHUNK - stream.avail_out, dest_file);
        } while (stream.avail_out == 0);           // Repeat until output buffer is full
    } while (ret != Z_STREAM_END);                 // Repeat until end of input stream

    deflateEnd(&stream);                           // Clean up compression stream
    fclose(source_file);                           // Close source file
    fclose(dest_file);                             // Close destination file

    return 0;                                      // Return 0 to indicate success
}

// Function to decompress a file
int decompress_file(const char *source, const char *dest) {
    FILE *source_file = fopen(source, "rb");      // Open source file in read-binary mode
    FILE *dest_file = fopen(dest, "wb");          // Open destination file in write-binary mode

    // Check if file opening failed
    if (!source_file || !dest_file) {
        perror("Error opening files");            // Print error message
        return -1;                                // Return -1 to indicate failure
    }

    z_stream stream;                              // Declare zlib decompression stream
    stream.zalloc = Z_NULL;                       // Set memory allocation function to NULL
    stream.zfree = Z_NULL;                        // Set memory free function to NULL
    stream.opaque = Z_NULL;                       // Set opaque value to NULL

    // Initialize inflate stream
    if (inflateInit(&stream) != Z_OK) {
        perror("inflateInit failed");             // Print error message
        return -1;                                // Return -1 to indicate failure
    }

    unsigned char in[CHUNK];                      // Declare input buffer
    unsigned char out[CHUNK];                     // Declare output buffer

    int ret;
    do {
        // Read CHUNK bytes from source file into input buffer
        stream.avail_in = fread(in, 1, CHUNK, source_file);
        // Check for read error
        if (ferror(source_file)) {
            inflateEnd(&stream);                   // Clean up decompression stream
            fclose(source_file);                   // Close source file
            fclose(dest_file);                     // Close destination file
            return -1;                            // Return -1 to indicate failure
        }
        // Exit loop if no more input data
        if (stream.avail_in == 0)
            break;
        stream.next_in = in;                       // Set input buffer pointer

        do {
            stream.avail_out = CHUNK;             // Set available space in output buffer
            stream.next_out = out;                // Set output buffer pointer
            // Decompress data from input buffer to output buffer
            ret = inflate(&stream, Z_NO_FLUSH);
            // Check for decompression error
            if (ret == Z_STREAM_ERROR) {
                inflateEnd(&stream);               // Clean up decompression stream
                fclose(source_file);               // Close source file
                fclose(dest_file);                 // Close destination file
                return -1;                        // Return -1 to indicate failure
            }
            // Write decompressed data to destination file
            fwrite(out, 1, CHUNK - stream.avail_out, dest_file);
        } while (stream.avail_out == 0);           // Repeat until output buffer is full
    } while (ret != Z_STREAM_END);                 // Repeat until end of input stream

    inflateEnd(&stream);                           // Clean up decompression stream
    fclose(source_file);                           // Close source file
    fclose(dest_file);                             // Close destination file

    return 0;                                      // Return 0 to indicate success
}

int main() {
    const char *input_file = "BinarySearchR.c";    // Input file name
    const char *compressed_file = "compressed_file.z";  // Compressed file name
    const char *decompressed_file = "decompressed_file.c";  // Decompressed file name

    // Compress the input file
    if (compress_file(input_file, compressed_file) == 0) {
        printf("File compressed successfully.\n");  // Print success message

        // Decompress the compressed file
        if (decompress_file(compressed_file, decompressed_file) == 0) {
            printf("File decompressed successfully.\n");  // Print success message
        } else {
            printf("Error decompressing file.\n");        // Print error message
        }
    } else {
        printf("Error compressing file.\n");              // Print error message
    }

    return 0;  // Return 
