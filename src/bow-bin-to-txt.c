/*  Copyright 2021 Safayet N Ahmed */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>

#include <bow.h>

const char*
Usage = "<src> <dst>";

int main (int argc, const char* argv[])
{
    int ret;
    const char *src_filename;
    const char *dst_filename;

    FILE *src_filep;
    FILE *dst_filep;
    
    bow_header_t header;
    bow_record_t record;

    uint32_t r;

    if (argc != 3) {
        fprintf (stderr, "Usage: %s <src> <dst>\n", argv[0]);
        ret = -1;
    } else {
        src_filename = argv[1];
        dst_filename = argv[2];
        
        src_filep = fopen(src_filename, "r");
        if (NULL == src_filep) {
            perror ("Failed to open source file");
            ret = -1;
        } else {
            dst_filep = fopen(dst_filename, "w");
            if (NULL == dst_filep) {
                perror ("Failed to open destination file");
                ret = -1;
            } else {
                ret = fread_bow_header_bin(src_filep, &header);
                if (-1 == ret) {
                    perror ("fread_bow_header_bin failed");
                } else {
                    ret = fprintf_bow_header_txt(dst_filep, &header);
                    if (ret <= 0) {
                        perror ("fprintf_bow_header_txt failed");
                    } else {
                        ret = 0;
                        for (r=0; (0 == ret) && (r<header.record_count); r++) {
                            ret = fread_bow_record_bin(src_filep, &record);
                            if (-1 == ret) {
                                perror ("fread_bow_record_bin failed");
                            } else {
                                ret = fprintf_bow_record_txt(dst_filep, &record);
                                if (ret <= 0) {
                                    perror ("fprintf_bow_record_txt failed\n");
                                } else {
                                    ret = 0;
                                }
                            }
                        }
                    }
                }

                fclose (dst_filep);
            }

            fclose (src_filep);
        }
    }

    return ret;
}

