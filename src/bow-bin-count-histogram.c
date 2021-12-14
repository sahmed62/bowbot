/*  Copyright 2021 Safayet N Ahmed */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <bow.h>

const char*
Usage = "<src> <dst>";

static int
populate_count_histogram (  FILE*           src_filep,
                            bow_header_t*   header_p,
                            uint32_t        *max_count_p,
                            uint32_t        **count_histogram_p)
{
    int ret;

    uint32_t r;

    bow_record_t record;

    uint32_t max_count = 0;
    uint32_t *old_count_histogram;
    uint32_t *count_histogram = NULL;

    for (ret = 0, r=0; (0 == ret) && (r<header_p->record_count); r++) {
        ret = fread_bow_record_bin(src_filep, &record);
        if (-1 == ret) {
            perror ("populate_count_histogram: fread_bow_record_bin failed");
        } else {
            /*  check if we have enough histogram bins */
            if (record.count > max_count) {
                old_count_histogram = count_histogram;
                /*  allocate enough histogram bins starting with 0 counts */
                count_histogram = realloc (  old_count_histogram,
                                        (sizeof(uint32_t)*(record.count+1)));
                if (NULL == count_histogram) {
                    perror ("populate_count_histogram: realloc failed");
                    ret = -1;
                } else {
                    /* zero out all new entries
                        new entries are the ones after previous max count */
                    memset (&(count_histogram[max_count+1]),
                            0,
                            ((record.count-max_count)*sizeof(uint32_t)));
                    max_count = record.count;
                }
            }

            if (0 == ret) {
                count_histogram[record.count]++;
            }
        }
    }

    if (0 != ret) {
        free (count_histogram);
        count_histogram = NULL;
        max_count = 0;
    }

    *max_count_p = max_count;
    *count_histogram_p = count_histogram;
    return ret;
}

static int
fprintf_count_histogram (   FILE*       dst_filep,
                            uint32_t    max_count,
                            uint32_t    *count_histogram)
{
    int ret;
    uint32_t c;
    for (ret = 0, c=1; (0 == ret) && (c<=max_count); c++) {
        ret = fprintf ( dst_filep,
                        "%10u : %10u\n",
                        (unsigned)c,
                        (unsigned)count_histogram[c]);
        if (ret < 0) {
            perror ("fprintf failed in fprintf_count_histogram");
            ret = -1;
        } else {
            ret = 0;
        }
    }

    return ret;
}

int main (int argc, const char* argv[])
{
    int ret;
    const char *src_filename;
    const char *dst_filename;

    FILE *src_filep;
    FILE *dst_filep;

    bow_header_t header;

    uint32_t max_count;
    uint32_t *count_histogram;

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
                    ret = populate_count_histogram (src_filep,
                                                    &header,
                                                    &max_count,
                                                    &count_histogram);
                    if (-1 == ret) {
                        fprintf (stderr, "populate_count_histogram failed\n");
                    } else {
                        ret = fprintf_count_histogram ( dst_filep,
                                                        max_count,
                                                        count_histogram);
                        if (-1 == ret) {
                            fprintf (stderr, "fprintf_count_histogram failed\n");
                        }

                        if (NULL != count_histogram) {
                            free (count_histogram);
                            count_histogram = NULL;
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

