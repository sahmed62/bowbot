/*  Copyright 2021 Safayet N Ahmed */

#include <stdlib.h>
#include <errno.h>

#include <bow.h>

static int
strtouint32 (   const char  *str,
                char        **endptr_p,
                uint32_t    *out_p)
{
    int ret;
    long int conv; 
    char *endptr;
    
    errno = 0;
    conv = strtol (str, &endptr, 0);
    if (0 != errno) {
        ret = -1;
    } else {
        if (endptr == str) {
            errno = EINVAL;
            ret = -1;
        } else {
            *out_p = (uint32_t)conv;
            if (NULL != endptr_p) {
                *endptr_p = endptr;
            }
            ret = 0;
        }
    }
    
    return ret;
}

int
getline_bow_header_txt (FILE*           filep,
                        char            **buf_p,
                        size_t          *buf_sz_p,
                        bow_header_t    *header_out_p)
{
    int ret;

    ssize_t bytes_read;

    bytes_read = getline (buf_p, buf_sz_p, filep);
    if (-1 == bytes_read) {
        perror ("Failed to read header line 1");
        ret = -1;
    } else {
        ret = strtouint32 ( *buf_p,
                            NULL,
                            &(header_out_p->doc_count));
        if (-1 == ret) {
            perror ("Failed to parse document count");
        } else {
            bytes_read = getline (buf_p, buf_sz_p, filep);
            if (-1 == bytes_read) {
                perror ("Failed to read header line 2");
                ret = -1;
            } else {
                ret = strtouint32 ( *buf_p,
                                    NULL,
                                    &(header_out_p->vocab_size));
                if (-1 == ret) {
                    perror ("Failed to parse vocabulary size");
                } else {
                    bytes_read = getline (buf_p, buf_sz_p, filep);
                    if (-1 == bytes_read) {
                        perror ("Failed to read header line 3");
                        ret = -1;
                    } else {
                        ret = strtouint32 ( *buf_p,
                                            NULL,
                                            &(header_out_p->record_count));
                        if (-1 == ret) {
                            perror ("Failed to parse recourd count");
                        } else {
                            header_out_p->MAGIC = BOW_MAGIC;
                        }
                    }                
                }
            }
        }
    }
    return 0;
}

int
getline_bow_record_txt (FILE*           filep,
                        char            **buf_p,
                        size_t          *buf_sz_p,
                        bow_record_t    *record_out_p)
{
    int ret;

    ssize_t bytes_read;

    char *startptr;
    char *endptr;

    bytes_read = getline (buf_p, buf_sz_p, filep);
    if (-1 == bytes_read) {
        perror ("Failed to read record line");
        ret = -1;
    } else {
        startptr = *buf_p;
        ret = strtouint32 ( startptr,
                            &endptr,
                            &(record_out_p->doc_id));
        if (-1 == ret) {
            perror ("Failed to parse document id");
        } else {
            startptr = endptr;
            ret = strtouint32 ( startptr,
                                &endptr,
                                &(record_out_p->word_id));
            if (-1 == ret) {
                perror ("Failed to parse word id");
            } else {
                startptr = endptr;
                ret = strtouint32 ( startptr,
                                    &endptr,
                                    &(record_out_p->count));
                if (-1 == ret) {
                    perror ("Failed to parse word count");
                }
            }
        }
    }

    return ret;
}

