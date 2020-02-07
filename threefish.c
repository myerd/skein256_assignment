#include <stdint.h>
#include "threefish.h"
#include <string.h>
#include <inttypes.h>
#include <stdio.h>

#if 0 /* TODO change to 0 after you complete this task */
/*

TODO FREE TEXT ASSIGNMENT

If there was a FAQ for this assignment, what question and answer would've helped you the most?

Q: How to get started, what to do first and what all signs in algorithm mean?

A: Figuring what symbols and signs means, then write how algorithm works in plaintext.
 After that figure how to make keyschedule in c, probably find help in google.

*/
#warning "FAIL you have not completed the free text assignment"
#endif

/* Under key at k and tweak at t, encrypt 32 bytes of plaintext at p and store it at c. */
void threefish(unsigned char *c, const unsigned char *p, const unsigned char *k, const unsigned char *t) {

    //Generating variables and casting to uint64

    static const uint8_t rot[8][2] = {
            {14, 16},
            {52, 57},
            {23, 40},
            {5,  37},
            {25, 33},
            {46, 12},
            {58, 22},
            {32, 32},
    };
    static const uint8_t perm[4][4] = {
            {0, 1, 2, 3},
            {0, 3, 2, 1},
            {0, 1, 2, 3},
            {0, 3, 2, 1},
    };
    int i, j;

//#if 1
//printf("p: ");
//for(i=0; i<32; i++) printf("%02x ", p[i]);
//printf("\nk: ");
//for(i=0; i<32; i++) printf("%02x ", k[i]);
//printf("\nt: ");
//for(i=0; i<16; i++) printf("%02x ", t[i]);
//printf("\n");
//#endif

    int NR = 72;
    uint64_t key[4];
    uint64_t subkey[19][4];
    uint64_t twe[2];
    uint64_t plain[4];


    uint64_t cipher[4];
    memcpy(key, k, 32);
    memcpy(twe, t, 16);
    memcpy(plain, p, 32);

    //Rotation function
    uint64_t rotate(uint64_t x, uint8_t r) {
        return (x << (r & 63)) | (x >> (64 - (r & 63)));
    }
    //mix function
    void mix(uint64_t x0, uint64_t x1, uint64_t *y0, uint64_t *y1, uint8_t r) {
        uint64_t temp = x0 + x1;
        *y1 = rotate(x1, r) ^ temp;
        *y0 = temp;
    }
    //Keyschedule
    uint64_t CN = 0x1BD11BDAA9FC1A22;
    uint64_t xkey[5];
    uint64_t xtweak[3] = {twe[0], twe[1], twe[0] ^ twe[1]};
    xkey[4] = CN;

    for (i = 0; i < 4; i++) {
        xkey[i] = key[i];
        xkey[4] ^= key[i];
    }
    for (i = 0; i < 19; i++) {
        for (j = 0; j < 4; j++) {
            subkey[i][j] = xkey[(i + j) % 5];
        }

        subkey[i][1] += xtweak[i % 3];
        subkey[i][2] += xtweak[(i + 1) % 3];
        subkey[i][3] += i;
    }

    //Rounds: 8 rounds NR/8 times
    for (i = 0; i < NR; i += 8) {
        //subkey:
        for (j = 0; j < 4; j++) {
            plain[j] += subkey[i / 4][j];
        }

        // Round 1
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[0][r]], plain[perm[0][r + 1]], &plain[perm[0][r]], &plain[perm[0][r + 1]],
                rot[0][r / 2]);
        }
        uint64_t temp1 = plain[1];
        uint64_t temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;
        //Round 2
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[0][r]], plain[perm[0][r + 1]], &plain[perm[0][r]], &plain[perm[0][r + 1]],
                rot[1][r / 2]);
        }
        temp1 = plain[1];
        temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;
        //Round 3
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[2][r]], plain[perm[2][r + 1]], &plain[perm[2][r]], &plain[perm[2][r + 1]],
                rot[2][r / 2]);
        }
        temp1 = plain[1];
        temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;
        //Round 4
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[0][r]], plain[perm[0][r + 1]], &plain[perm[0][r]], &plain[perm[0][r + 1]],
                rot[3][r / 2]);
        }
        temp1 = plain[1];
        temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;

        // New subkey
        for (j = 0; j < 4; j++) {
            plain[j] += subkey[i / 4 + 1][j];
        }
        // 4 rounds with new subkey
        //Round 5
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[0][r]], plain[perm[0][r + 1]], &plain[perm[0][r]], &plain[perm[0][r + 1]],
                rot[4][r / 2]);
        }
        temp1 = plain[1];
        temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;

        //Round 6
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[0][r]], plain[perm[0][r + 1]], &plain[perm[0][r]], &plain[perm[0][r + 1]],
                rot[5][r / 2]);
        }
        temp1 = plain[1];
        temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;
        //Round 7
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[2][r]], plain[perm[2][r + 1]], &plain[perm[2][r]], &plain[perm[2][r + 1]],
                rot[6][r / 2]);
        }
        temp1 = plain[1];
        temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;
        //Round 8
        for (unsigned r = 0; r < 4; r += 2) {
            mix(plain[perm[0][r]], plain[perm[0][r + 1]], &plain[perm[0][r]], &plain[perm[0][r + 1]],
                rot[7][r / 2]);
        }
        temp1 = plain[1];
        temp2 = plain[3];
        plain[1] = temp2;
        plain[3] = temp1;

    }

    for (i = 0; i <4 ; i++)
    {
        cipher[i] = plain[i] + subkey[NR/4][i];
    }
    memcpy(c, cipher, 32);
}

