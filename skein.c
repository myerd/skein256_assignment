#include <string.h>
#include <stdio.h>
#include "skein.h"
#include "threefish.h"
#include <inttypes.h>

static const int Nw = 4;     // block length in words
static const int Nb = 32;    // block length in bytes

#define SKEIN_MK_64(hi32,lo32)  ((lo32) + (((uint64_t) (hi32)) << 32))

const uint64_t SKEIN_256_IV_256[] =
        {
                SKEIN_MK_64(0xFC9DA860,0xD048B449),
                SKEIN_MK_64(0x2FCA6647,0x9FA7D833),
                SKEIN_MK_64(0xB33BC389,0x6656840F),
                SKEIN_MK_64(0x6A54E920,0xFDE8DA69)
        };

void init(context *ctx) {
    memcpy(ctx->X,SKEIN_256_IV_256,sizeof(ctx->X));
    ctx->T[0] = 0;
    ctx->count = 0;
    uint64_t t[3];
    uint64_t wa;
    wa = 48;
    t[0] = 0LL;
    t[1] = wa << 56;
    t[1] |= 1LL << 62;
    ctx->T[1] = t[1];
    ctx->T[0] = t[0];
}


void update(unsigned char *a, int len, context *ctx) {
    static const uint64_t notfirst = ~(1LL << 62);
    size_t n;
    if (len + ctx->count > 32) {
        if (ctx->count) {
            ctx->T[0] += 32;
            n = 32 - ctx->count;
            if (n) {
                memcpy(&ctx->buffer[ctx->count], a, n);
                len -= n;
                a += n;
                ctx->count += n;
            }

            threefish((unsigned char *) ctx->X, ctx->buffer, (unsigned char *) ctx->X, (unsigned char *) ctx->T);
            uint64_t buf[sizeof(ctx->buffer)];
            memcpy(buf, ctx->buffer, sizeof(ctx->buffer));
            for (int i = 0; i < Nw; i++)
                ctx->X[i] ^= buf[i];
            ctx->count = 0;
        }
        n = (len - 1) / 32;
        if (n) {
            for (int i = 0; i < n; i++) {
                ctx->T[0] += 32;
                threefish((unsigned char *) ctx->X, a, (unsigned char *) ctx->X, (unsigned char *) ctx->T);
                ctx->T[1] &= notfirst;
                uint64_t x[4];
                memcpy(x, a, 32);
                for (int i = 0; i < Nw; i++)
                    ctx->X[i] ^= x[i];
                len -= n * 32;
                a += n * 32;
            }
        }
    }

    if (len)
    {
        if (len + ctx->count <= 32) {
            memcpy(&ctx->buffer[ctx->count], a, len);
            ctx->count += len;
        }
    }

}


void finalize(unsigned char *a, context *ctx) {

    size_t i,n,byteCnt;
    uint64_t X[4];
    ctx->T[1] |= 1LL << 63;
    if (ctx->count < 32)
        memset(&ctx->buffer[ctx->count],0,32 - ctx->count);
    ctx->T[0] += ctx->count;
    threefish((unsigned char *)ctx->X,ctx->buffer,(unsigned char *)ctx->X,(unsigned char *)ctx->T);
    //XoR
    uint64_t buf[sizeof(ctx->buffer)];
    memcpy(buf,ctx->buffer, sizeof(ctx->buffer));
    for(int i=0; i<Nw; i++)
        ctx->X[i] ^=  buf[i];

    memset(ctx->buffer,0,sizeof(ctx->buffer));
    //Static tweak for last call
    ctx->T[0] = 8;
    ctx->T[1] = 0xFF00000000000000;
    threefish(a, ctx->buffer, (unsigned char *)ctx->X, (unsigned char *)ctx->T);
}

