/*  Copyright 2021 Safayet N Ahmed */

#ifndef __BOW_H__
#define __BOW_H__

#include <stdio.h>
#include <stdint.h>

/*  magic number: "BOW\0" in little-endian byte order */
#define BOW_MAGIC 0x00574F42

typedef struct {
    uint32_t MAGIC;
    uint32_t doc_count;
    uint32_t vocab_size;
    uint32_t record_count;
} bow_header_t;

typedef struct {
    uint32_t doc_id;
    uint32_t word_id;
    uint32_t count;
} bow_record_t;

/*extern void
init_bow_header(bow_header_t *header_in_p); */

#define init_bow_header(header_in_p)            \
        do{                                     \
            (header_in_p)->MAGIC = BOW_MAGIC;   \
            (header_in_p)->doc_count = 0;       \
            (header_in_p)->vocab_size = 0;      \
            (header_in_p)->record_count = 0;    \
        }while(0)

extern int
getline_bow_header_txt (FILE*           filep,
                        char            **buf_p,
                        size_t          *buf_sz_p,
                        bow_header_t    *header_out_p);

/*extern int
fprintf_bow_header_txt (FILE*           filep,
                        bow_header_t    *header_in_p); */

#define fprintf_bow_header_txt(filep, header_in_p)          \
        fprintf (   filep,                                  \
                    "%u\n%u\n%u\n",                         \
                    (unsigned)(header_in_p)->doc_count,     \
                    (unsigned)(header_in_p)->vocab_size,    \
                    (unsigned)(header_in_p)->record_count)

extern int
getline_bow_record_txt (FILE*           filep,
                        char            **buf_p,
                        size_t          *buf_sz_p,
                        bow_record_t    *count);

/*extern int
fprintf_bow_record_txt (FILE*           filep,
                        bow_record_t    *record_p); */

#define fprintf_bow_record_txt(filep, record_in_p)      \
        fprintf (   filep,                              \
                    "%u %u %u\n",                       \
                    (unsigned)(record_in_p)->doc_id,    \
                    (unsigned)(record_in_p)->word_id,   \
                    (unsigned)(record_in_p)->count)

/*extern int
fread_bow_header_bin (  FILE*           filep,
                        bow_header_t    *header_out_p); */

#define fread_bow_header_bin(filep, header_out_p)   \
        ((1 == fread (  header_out_p,               \
                        sizeof(bow_header_t),       \
                        1,                          \
                        filep))? 0 : -1)

/*extern int
fwrite_bow_header_bin ( FILE*           filep,
                        bow_header_t    *header_in_p);*/

#define fwrite_bow_header_bin(filep, header_in_p)   \
        ((1 == fwrite ( (header_in_p),              \
                        sizeof(bow_header_t),       \
                        1,                          \
                        filep))? 0 : -1)


/*extern int
fread_bow_record_bin (  FILE*           filep,
                        bow_record_t    *record_out_p); */

#define fread_bow_record_bin(filep, record_out_p)   \
        ((1 == fread (  record_out_p,               \
                        sizeof(bow_record_t),       \
                        1,                          \
                        filep))? 0 : -1)

/*extern int
fwrite_bow_record_bin ( FILE*           filep,
                        bow_record_t    *record_in_p);*/

#define fwrite_bow_record_bin(filep, record_in_p)   \
        ((1 == fwrite ( (record_in_p),              \
                        sizeof(bow_record_t),       \
                        1,                          \
                        filep))? 0 : -1)

#endif

