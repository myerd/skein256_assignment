#include <stdint.h>

typedef struct  {
    uint64_t T[2];
    uint64_t X[4];
    uint64_t states[4];
    unsigned char buffer[32];
    int count;
} context;

/* Initialize fresh context ctx */
void init(context *ctx);

/* Under context ctx, incrementally process len bytes at a */
void update(unsigned char *a, int len, context *ctx);

/* Under context ctx, finalize the hash and place the digest at a */
void finalize(unsigned char *a, context *ctx);

