#ifndef _MM_H
#define _MM_H

typedef struct FrameFlag
{
    unsigned char flag;
    unsigned char order;
    unsigned short ref_count;
    unsigned char chunk_order;
} FrameFlag;

typedef struct Chunk
{
    struct Chunk *next;
} Chunk;


void test_buddy();
void test_dynamic_alloc();
void *kmalloc(unsigned int size);
void kfree(void *ptr);
void init_buddy();

#endif