/*  Copyright 2021 Safayet N Ahmed */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <bow.h>

int
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

int
fopen_bow_stream (  const char      *src_filename,
                    bow_stream_t    *ctx_p)
{
    int ret;
    FILE* filep;

    filep = fopen(src_filename, "r");
    if (NULL == filep) {
        perror ("fopen_bow_docstream: fopen failed");
        ret = -1;
    } else {
        ret = fread_bow_header_bin(filep, &(ctx_p->header));
        if (ret < 0) {
            perror ("fopen_bow_docstream: fread_bow_header_bin failed");
            ret = -1;
        } else {
            ctx_p->filep      = filep;
            /*  record ids and document ids begin at 1 */
            ctx_p->r          = 1;
            ctx_p->filled     = 0;
            /*  indices begin at 1 */
            ctx_p->i          = 0;

            ret = 0;
        }

        if (-1 == ret) {
            fclose (filep);
        }
    }

    return ret;
}

int
fgetrecord_bow_stream ( bow_stream_t        *ctx_p,
                        const bow_record_t  **record_pp)
{
    int ret;

    /*  check we haven't exhausted the document */
    if (!bow_stream_notempty(ctx_p)) {
        fprintf (   stderr,
                    "getdoc_bow_docstream: returned all %u records\n",
                    (unsigned)ctx_p->header.record_count);
        ret = -1;
    } else {
        /* refill the block if needed */
        if (ctx_p->i >= ctx_p->filled) {
            ret = fread (   ctx_p->block,               \
                            sizeof(bow_record_t),       \
                            512,                        \
                            ctx_p->filep);
            if (ret < 0) {
                perror ("getdoc_bow_docstream: fread failed");
                ret = -1;
            } else {
                ctx_p->filled = ret;
                ctx_p->i = 0;
                ret = 0;
            }
        }

        if (0 == ret) {
            *record_pp = &(ctx_p->block[ctx_p->i]);
            ctx_p->i++;
            ctx_p->r++;
        }
    }

    return ret;
}

void
fclose_bow_stream (bow_stream_t *ctx_p)
{
    fclose (ctx_p->filep);
    /* zeroise (initialize) the bowdoc array */
    memset (ctx_p,
            0,
            sizeof(bow_stream_t));
}

