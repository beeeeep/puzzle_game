#include <bitset>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <random>

#define NUM_COLS  5
#define NUM_LINES 5

#define forlines(xx) for (int xx = 0; xx < NUM_LINES; ++xx)
#define forcols(xx)  for (int xx = 0; xx < NUM_COLS; ++xx)
#define forall(xx, yy, kernel) \
    forlines(xx) {             \
        forcols(yy) {          \
            kernel             \
        }                      \
    }


bool decompressMap(const uint64_t compressedMap, int8_t S[5][5], int& startx, int& endx) {
    int k     = 0;
    bool isOk = true;
    forall(
        i, j, const uint8_t mm = (compressedMap >> k) & 0b11;
        if (mm == 0) { S[i][j] = 0; } else if (mm == 1) { S[i][j] = -1; } else if (mm == 2) { S[i][j] = 1; } else {
            std::cout << "ERROR mm =" << (int) mm << '\n';
            isOk = false;
        } k += 2;) startx = ((compressedMap >> k) & 0b111);
    k += 3;
    endx = ((compressedMap >> k) & 0b111);
    if (startx > NUM_LINES) {
        std::cout << "startx =" << startx << '\n';
        isOk = false;
    }
    if (endx > NUM_LINES) {
        std::cout << "endx =" << endx << '\n';
        isOk = false;
    }
    return isOk;
}

int main() {
    std::string baseFileName = "moves";
    std::ifstream moves[]    = {
        std::ifstream(baseFileName + "1_finalt.in", std::ios::binary),
        std::ifstream(baseFileName + "2_finalt.in", std::ios::binary),
        std::ifstream(baseFileName + "3_finalt.in", std::ios::binary),
        std::ifstream(baseFileName + "4_finalt.in", std::ios::binary),
    };
    constexpr int numberOfMapsPerMovements = 1 << 16;
    int8_t S[5][5];
    char compressedMapBuffer[sizeof(uint64_t)];
    for (int i = 0; i < numberOfMapsPerMovements; ++i) {
        moves[0].read(compressedMapBuffer, sizeof(uint64_t));
        uint64_t* compressedMap = reinterpret_cast<uint64_t*>(compressedMapBuffer);
        std::cout << std::bitset<64>(*compressedMap) << '\n';
        int startx, endx;

        if (!decompressMap(*compressedMap, S, startx, endx)) {
            break;
        }
    }
    return 0;
}
