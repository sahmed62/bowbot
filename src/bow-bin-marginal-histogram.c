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
populate_marginal_histogram (   bow_stream_t    *bow_stream_p,
                                uint32_t        max_count,
                                int32_t         **histogram_p)
{
    int ret;
    uint32_t vocab_sz = bow_stream_p->header.vocab_size;
    uint32_t doc_count= bow_stream_p->header.doc_count;

    int32_t *histogram;

    const bow_record_t *record_p;

    /* document counter and word counter*/
    uint32_t w, c;

    /*  need enough space to count 0 to max_count inclusive */
    histogram = (int32_t*)calloc ((max_count+1)*vocab_sz, sizeof(int32_t));
    if (NULL == histogram) {
        perror ("populate_marginal_histogram: malloc failed for histogram");
        ret = -1;
    } else {
        while ((0 == ret) && bow_stream_notempty(bow_stream_p)) {
            ret = fgetrecord_bow_stream (   bow_stream_p,
                                            &record_p);
            if (-1 == ret) {
                fprintf (   stderr,
                            "populate_marginal_histogram: "
                            "fgetrecord_bow_stream failed\n");
            } else {
                /*  word_id begins at 1; the array index begins at 0 */
                w = record_p->word_id - 1;
                c = record_p->count;
                if (c < max_count) {
                    histogram[w*(max_count+1) + c]++;
                    /*  We "uncount" non-zeros instead of counting zeros
                        because there are fewer non-zeros and this is MUCH
                        faster */
                    histogram[w*(max_count+1) + 0]--;
                }
            }
        }

        /*  Fix the zero counts:
                zeros = doc_count - non-zeros */
        for (w=0; w<vocab_sz; w++) {
            histogram[w*(max_count+1) + 0] += doc_count;
        }

        if (0 != ret) {
            free (histogram);
            histogram = NULL;
        }
    }
    printf ("done counting!\n");
    fflush (stdout);

    *histogram_p = histogram;
    return ret;
}

static int
fprintf_marginal_histogram (FILE*           dst_filep,
                            uint32_t        vocab_sz,
                            uint32_t        max_count,
                            int32_t        *histogram)
{
    int ret;

    uint32_t w, c;
    /*  print the header */
    ret = fprintf (dst_filep, "word id:   ");
    if (ret < 0) {
        perror ("fprintf_marginal_histogram: fprintf failed for header (1)");
        ret = -1;
    } else {
        for (ret = 0, c=1; (0 == ret) && (c<=max_count); c++) {
            ret = fprintf ( dst_filep,
                            "%10u ",
                            (unsigned)c);
            if (ret < 0) {
                perror ("fprintf_marginal_histogram: fprintf failed for header (2)");
                ret = -1;
            } else {
                ret = 0;
            }
        }
        if (0 == ret) {
            ret = fprintf (dst_filep, "\n");
            if (ret < 0) {
                perror ("fprintf_marginal_histogram: fprintf failed for header (3)");
                ret = -1;
            } else {
                ret = 0;
            }
        }
    }
    /* loop through all the words */
    for (w=0; (0 == ret) && (w<vocab_sz); w++) {
        ret = fprintf ( dst_filep,
                        "%10u: ",
                        (w+1));
        if (ret < 0) {
            perror ("fprintf_marginal_histogram: fprintf failed for body (1)");
            ret = -1;
        }
        /* loop through all the count values */
        for (ret = 0, c=0; (0 == ret) && (c<=max_count); c++) {
            /*  each word gets enough space to count 0 to max_count inclusive */
            ret = fprintf ( dst_filep,
                            "%10i ",
                            histogram[w*(max_count+1)+c]);
            if (ret < 0) {
                perror ("fprintf_marginal_histogram: fprintf failed for body (2)");
                ret = -1;
            } else {
                ret = 0;
            }
        }
        if (0 == ret) {
            ret = fprintf (dst_filep, "\n");
            if (ret < 0) {
                perror ("fprintf_marginal_histogram: fprintf failed for body (3)");
                ret = -1;
            } else {
                ret = 0;
            }
        }
    }

    return ret;
}

int main (int argc, const char* argv[])
{
    int ret;
    const char *src_filename;
    const char *dst_filename;

    FILE *dst_filep;

    bow_stream_t bow_stream;

    uint32_t max_count;
    int32_t *histogram;

    if (argc != 4) {
        fprintf (stderr, "Usage: %s <src> <dst> <max count>\n", argv[0]);
        ret = -1;
    } else {
        src_filename = argv[1];
        dst_filename = argv[2];
        ret = strtouint32 ( argv[3],
                            NULL,
                            &max_count);
        if (0 != ret) {
            perror ("Failed to parse max count from 3rd argument");
        } else {
            ret = fopen_bow_stream (src_filename,
                                    &bow_stream);
            if (-1 == ret) {
                fprintf (   stderr,
                            "fopen_bow_stream failed for \"%s\"",
                            src_filename);
                ret = -1;
            } else {
                dst_filep = fopen(dst_filename, "w");
                if (NULL == dst_filep) {
                    perror ("Failed to open destination file");
                    ret = -1;
                } else {
                    ret = populate_marginal_histogram ( &bow_stream,
                                                        max_count,
                                                        &histogram);
                    if (-1 == ret) {
                        fprintf (stderr, "populate_count_histogram failed\n");
                    } else {
                        ret = fprintf_marginal_histogram (  dst_filep,
                                                            bow_stream.header.vocab_size,
                                                            max_count,
                                                            histogram);
                        if (-1 == ret) {
                            fprintf (stderr, "fprintf_count_histogram failed\n");
                        }

                        free (histogram);
                    }

                    fclose (dst_filep);
                }

                fclose_bow_stream (&bow_stream);
            }
        }
    }

    return ret;
}

