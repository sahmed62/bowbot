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
    
    char *buf;
    size_t buf_sz;

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
                buf = NULL;
                buf_sz = 0;
                ret = getline_bow_header_txt (  src_filep,
                                                &buf,
                                                &buf_sz,
                                                &header);
                if (-1 == ret) {
                    fprintf (stderr, "getline_bow_header_txt failed\n");
                } else {
                    ret = fwrite_bow_header_bin(dst_filep, &header);
                    if (-1 == ret) {
                        perror ("fwrite_bow_header_bin failed\n");
                    } else {
                        for (r=0; (0 == ret) && (r<header.record_count); r++) {
                            ret = getline_bow_record_txt (  src_filep,
                                                            &buf,
                                                            &buf_sz,
                                                            &record);
                            if (-1 == ret) {
                                fprintf (stderr, "getline_bow_record_txt failed\n");
                            } else {
                                ret = fwrite_bow_record_bin(dst_filep, &record);
                                if (-1 == ret) {
                                    perror ("fwrite_bow_record_bin failed\n");
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

