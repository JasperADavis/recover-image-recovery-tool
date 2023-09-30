#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Ensure proper usage
    if (argc != 2)
    {
        printf("Usage: ./recover RAWDATA\n");
        return 1;
    }

    // Remember filenames
    char *infile = argv[1];

    // Open input file
    FILE *card_file = fopen(infile, "rb");

    if (infile == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 1;
    }

    // Read through raw file,
    // check first four bytes for pattern
    // if exists, copy string of data until next pattern
    // save that string to a file as a jpg

    const char file_extension[] = ".jpg"; // File extension
    int current_image_num = -1;
    char filename[8];

    unsigned char buffer[512];
    int BLOCK_SIZE = 512;
    FILE *img_file = NULL;

    char target_bits = 0x0e;

    while (fread(buffer, sizeof(char), BLOCK_SIZE, card_file) == BLOCK_SIZE)
    {
        // Check for special sequence
        char fourth_byte_left_4_bits = buffer[3] >> 4;

        // If special sequence detected, create new jpg
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && fourth_byte_left_4_bits == target_bits)
        {

            // If first time, create .jpg
            if (current_image_num == -1)
            {
                current_image_num++;
                // Create file
                snprintf(filename, sizeof(filename), "%03d%s", current_image_num, file_extension);
                img_file = fopen(filename, "ab");
                if (img_file == NULL)
                {
                    printf("Error creating file.\n");
                    return 1;
                }

                fwrite(buffer, sizeof(char), 512, img_file);
            }
            // If NOT first time, close previous .jpg THEN create new jpg
            else
            {
                fclose(img_file);
                current_image_num++;

                snprintf(filename, sizeof(filename), "%03d%s", current_image_num, file_extension);
                img_file = fopen(filename, "ab");
                if (img_file == NULL)
                {
                    printf("Error creating file.\n");
                    return 1;
                }
                fwrite(buffer, sizeof(char), 512, img_file);
            }
        }

        // If not the special sequence, add data to previous image file
        else
        {
            if (current_image_num > -1)
            {
                fwrite(buffer, sizeof(char), 512, img_file);
            }
        }
    }

    // Close infile
    if (img_file != NULL)
    {
        fclose(img_file);
    }
    fclose(card_file);
}