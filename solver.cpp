#include <cmath>
#include <cstdlib>
#include <cstring>
#include <random>
#include <ctime>
#include <climits>
#include <bitset>
#include <iostream>


#define NUM_COLS 5
#define NUM_LINES 5

#define forlines(xx) for(int xx=0; xx < NUM_LINES; ++xx)
#define forcols(xx) for(int xx=0; xx < NUM_COLS; ++xx)
#define forall(xx,yy, kernel) forlines(xx){forcols(yy) {kernel}}
void initS(int8_t S[5][5])
{   time_t currentTime;
    time(&currentTime);
    srand(currentTime % INT_MAX);
    forall(i,j, {S[i][j] = rand()%3-1;})
    forcols(j)
    {
        S[0][j] = (S[0][j] > 0)?0:S[0][j];
        S[NUM_LINES-1][j] = (S[0][j] < 0)?0:S[0][j];
    }
}

char trS(int8_t s)
{
    switch(s)
    {
        case -1:
            return '\\';
        case 1:
            return '/';
        case 0:
            return '=';
        default:
            return 'X';
    }
}

void printMap(int8_t Z[5][6], int8_t S[5][5])
{
    forlines(i)
    {
        forcols(j)
        {
            std::cout << trS(S[i][j]);
            std::cout << Z[i][j+1];
        }
        std::cout << '\n';
    }

}

void buildZ(int8_t Z[5][6], int8_t S[5][5], int8_t startx)
{
    Z[startx][0] = 1;
    int newk;
    int k = startx;
    for(int c = 1; c < NUM_COLS + 1; ++c)
    {
       newk = k;
       switch(S[k][c-1])
       {
        case 0:
            {
            Z[k][c] = Z[k][c-1];
            newk = k;
            break;
            }
        case -1:
            if (k + 1 < 5) {
                Z[k+1][c] = Z[k][c-1];
                newk = k +1;
            }
            break;
        case 1:
            if (k -1 >= 0)
            {
                Z[k-1][c] = Z[k][c-1];
                newk = k -1;
            }
            break;
        default:
            std::cout << "ERROR at \n";
       }
       k = newk;
    }
}

#define __min(a, b) (((a) < (b)) ? (a) : (b))
#define ___min(a,b,c) __min((a),(__min(b,c)))
#define BIG_FAT_VALUE 9
void buildD(int8_t D[5][6], int8_t Z[5][6], int8_t S[5][5], int8_t endx)
{
    forlines(l)
    {
        D[l][5] = BIG_FAT_VALUE;
    }

    D[endx][5] = 0;

    for(int c = NUM_COLS-1; c >= 1; --c)
    {
        forlines(l)
        {
            const int d1 = (l+1 < NUM_LINES)?(D[l+1][c+1]+(S[l][c]==-1?0:1)):BIG_FAT_VALUE;
            const int d2 = D[l][c+1] + (S[l][c]==0?0:1);
            const int d3 = (l>0)?(D[l-1][c+1] + (S[l][c]==1?0:1)):BIG_FAT_VALUE;
            D[l][c] = ___min(
                d1,
                d2,
                d3
            );
        }
    }
}

void compressMap(const int8_t S[5][5], const int8_t startx, const int8_t endx, uint64_t& result)
{
    result = 0;
    int k = 0;
    forall(i,j,
        uint64_t mm;
        if (S[i][j] == 0)
        {
            mm = 0;
        }
        else if (S[i][j] < 0)
        {
            mm = 0b01;
        }
        else
        {
            mm = 0b10;
        }
        result |= (mm << k);
        // std::cout << std::bitset<64>(result) << "---(" << (int)S[i][j] << ")\n";
        k += 2;
    )
    result |= ((uint64_t)startx) << k;
    k += 3;
    result |= ((uint64_t)endx) << k;
}

void uncompressMap(const uint64_t compressedMap, int8_t S[5][5], int8_t& startx, int8_t& endx)
{
    int k = 0;
    forall(i,j,
        const uint8_t mm = (compressedMap >> k) & 0b11;
        if (mm == 0)
        {
            S[i][j] = 0;
        }
        else if (mm == 1)
        {
            S[i][j] = -1;
        }
        else if (mm == 2)
        {
            S[i][j] = 1;
        }
        else
        {
            std::cout << "ERROR mm =" << (int)mm << '\n';
        }
        k += 2;
    )
    startx = (int8_t) ((compressedMap >> k) & 0b111);
    k += 3;
    endx = (int8_t) ((compressedMap >> k) & 0b111);
}

int main()
{
    int8_t D[5][6] = {0};
    int8_t Z[5][6] = {0};
    int8_t S[5][5] = {0};
    int8_t startx = 3;
    int8_t endx = 3;
    initS(S);
    buildZ(Z, S, startx);
    printMap(Z, S);
    buildD(D, Z, S, endx);
    std::cout << "\n\n";
    // printMap(D, S);
    uint64_t compressedMap;
    compressMap(S, startx, endx, compressedMap);
    std::cout << std::bitset<64>(compressedMap) << "\n";
    std::cout << (int)startx << " " << (int)endx << "\n";
    forall(i,j, S[i][j]=0;);
    uncompressMap(compressedMap, S, startx, endx);
    std::cout << std::bitset<64>(compressedMap) << "\n";
    printMap(Z, S);
    std::cout << (int)startx << " " << (int)endx << "\n";
    compressMap(S, startx, endx, compressedMap);
    std::cout << std::bitset<64>(compressedMap) << "\n";
    return 0;
}