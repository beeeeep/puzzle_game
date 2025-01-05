#include <bitset>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <random>



#define BIG_FAT_VALUE   9
#define NUM_COLS  5
#define NUM_LINES 5
#define NUM_FINAL_MAPS 1 << 11
// #define OUTPUT_MAPS

#define __min(a, b)     (((a) < (b)) ? (a) : (b))
#define ___min(a, b, c) __min((a), (__min(b, c)))
#define forlines(xx) for (int xx = 0; xx < NUM_LINES; ++xx)
#define forcols(xx)  for (int xx = 0; xx < NUM_COLS; ++xx)
#define forall(xx, yy, kernel) \
    forlines(xx) {             \
        forcols(yy) {          \
            kernel             \
        }                      \
    }
void initS(int8_t S[5][5]) {
    time_t currentTime;
    time(&currentTime);
    srand(currentTime % INT_MAX);
    forall(i, j, { S[i][j] = rand() % 3 - 1; }) forcols(j) {
        S[0][j]             = (S[0][j] > 0) ? 0 : S[0][j];
        S[NUM_LINES - 1][j] = (S[0][j] < 0) ? 0 : S[0][j];
    }
}

char trS(int8_t s) {
    switch (s) {
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

void printMap(int8_t D[5][6], int8_t S[5][5]) {
    forlines(i) {
        std::cout << (int) D[i][0];
        forcols(j) {
            std::cout << trS(S[i][j]);
            std::cout << (int) D[i][j + 1];
        }
        std::cout << '\n';
    }
}

void buildZ(int8_t Z[5][6], int8_t S[5][5], int8_t startx) {
    Z[startx][0] = 1;
    // int newk;
    for (int c = 1; c < NUM_COLS + 1; ++c) {
        for (int k = 0; k < NUM_LINES; ++k) {
            switch (S[k][c - 1]) {
            case 0:
                {
                    Z[k][c] = Z[k][c - 1];
                    std::cout << "Z[k][c] = " << (int) Z[k][c] << '\n';
                    // newk = k;
                    break;
                }
            case -1:
                if (k + 1 < 5) {
                    Z[k + 1][c] = Z[k][c - 1];
                    std::cout << "Z[k+1][c] = " << (int) Z[k + 1][c] << '\n';
                    // newk = k +1;
                }
                break;
            case 1:
                if (k - 1 >= 0) {
                    Z[k - 1][c] = Z[k][c - 1];
                    std::cout << "Z[k-1][c] = " << (int) Z[k - 1][c] << '\n';
                    // newk = k -1;
                }
                break;
            default:
                std::cout << "ERROR at \n";
            }
        }
    }
}


void buildD(int8_t D[5][6], int8_t S[5][5], int8_t endx) {
    forlines(l) {
        D[l][5] = BIG_FAT_VALUE;
    }

    D[endx][5] = 0;

    int d1,d2,d3;
    int conflictWithSwitch = 0;
    for (int c = NUM_COLS - 1; c >= 0; --c) {
        conflictWithSwitch = 0;
        forlines(l) {
            if (l + 1 < NUM_LINES) {
                if (l + 2 < NUM_LINES)
                {
                    conflictWithSwitch = (S[l+1][c] == 1)?1:0;
                }
                d1 = D[l + 1][c + 1] + abs(S[l][c] - (-1)) + conflictWithSwitch;
            } else {
                d1 = BIG_FAT_VALUE;
            }
            d2 = D[l][c + 1] + abs(S[l][c] - 0);
            if (l > 0)
            {
                if (l > 1)
                {
                    conflictWithSwitch = (S[l-1][c] == -1)?1:0;
                }
                else
                {
                    conflictWithSwitch = 0;
                }
                d3 = D[l - 1][c + 1] + abs(S[l][c] - 1) + conflictWithSwitch;
            }
            else
            {
                d3 = BIG_FAT_VALUE;
            }
            D[l][c]      = ___min(d1, d2, d3);
        }
    }
}

void compressMap(const int8_t S[5][5], const int8_t startx, const int8_t endx, uint64_t& result) {
    result = 0;
    int k  = 0;
    forall(
        i, j, uint64_t mm;
        if (S[i][j] == 0) { mm = 0; } else if (S[i][j] < 0) { mm = 0b01; } else { mm = 0b10; } result |= (mm << k);
        // std::cout << std::bitset<64>(result) << "---(" << (int)S[i][j] << ")\n";
        k += 2;) result |= ((uint64_t) startx) << k;
    k += 3;
    result |= ((uint64_t) endx) << k;
}

bool decompressMap(const uint64_t compressedMap, int8_t S[5][5], int8_t& startx, int8_t& endx) {
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
        std::cout << "startx =" << (int) startx << '\n';
        isOk = false;
    }
    if (endx > NUM_LINES) {
        std::cout << "endx =" << (int) endx << '\n';
        isOk = false;
    }
    return isOk;
}

void testCompressionDecompression() {
    int8_t D[5][6] = {0};
    int8_t Z[5][6] = {0};
    int8_t S[5][5] = {0};
    int8_t startx  = 3;
    int8_t endx    = 3;
    initS(S);
    buildZ(Z, S, startx);
    printMap(Z, S);
    buildD(D, S, endx);
    std::cout << "\n\n";
    // printMap(D, S);
    uint64_t compressedMap;
    compressMap(S, startx, endx, compressedMap);
    std::cout << std::bitset<64>(compressedMap) << "\n";
    std::cout << (int) startx << " " << (int) endx << "\n";
    forall(i, j, S[i][j] = 0;);
    decompressMap(compressedMap, S, startx, endx);
    std::cout << std::bitset<64>(compressedMap) << "\n";
    printMap(Z, S);
    std::cout << (int) startx << " " << (int) endx << "\n";
    compressMap(S, startx, endx, compressedMap);
    std::cout << std::bitset<64>(compressedMap) << "\n";
}

bool isMapValid(const int8_t S[NUM_LINES][NUM_COLS]) {
    for (int i = 0; i < NUM_COLS; ++i) {
        if (S[0][i] == 1 || S[NUM_LINES - 1][i] == -1) {
            return false;
        }
    }
    for (int l = 0; l < NUM_LINES - 1; ++l) {
        for (int c = 0; c < NUM_COLS; ++c) {
            if (S[l][c] == -1 && S[l + 1][c] == 1) {
                return false;
            }
        }
    }
    return true;
}


bool forStateFunction(int8_t S[5][5], int indexNum, std::function<bool(void)> bodyLogic) {
    const int i = indexNum / NUM_COLS;
    const int j = indexNum % NUM_COLS;
    if (indexNum < NUM_LINES * NUM_COLS) {
        for (S[i][j] = -1; S[i][j] <= 1; ++S[i][j]) {
            if (!forStateFunction(S, indexNum + 1, bodyLogic)) {
                return false;
            }
        }
    } else {
        if (isMapValid(S)) {
            return bodyLogic();
        }
    }
    return true;
}


void outputMaps(const std::string fileName, int numSolutionsPerMove[5]) {
    std::ofstream moves[] = {std::ofstream(fileName + "0.in", std::ios::binary),
        std::ofstream(fileName + "1.in", std::ios::binary), std::ofstream(fileName + "2.in", std::ios::binary),
        std::ofstream(fileName + "3.in", std::ios::binary), std::ofstream(fileName + "4.in", std::ios::binary),
        std::ofstream(fileName + "5.in", std::ios::binary), std::ofstream(fileName + "6.in", std::ios::binary),
        std::ofstream(fileName + "7.in", std::ios::binary), std::ofstream(fileName + "8.in", std::ios::binary),
        std::ofstream(fileName + "9.in", std::ios::binary), std::ofstream(fileName + "10.in", std::ios::binary)};
    int8_t S[5][5]        = {0};
    int index             = 0;
    uint64_t result;
    // constexpr int64_t actualNumberOfStates = std::pow(3, 25);
    constexpr int64_t statesLimit = std::pow(2, 25);
    auto getSolutionsPerState     = [&]() {
        for (int endx = 0; endx < NUM_LINES; ++endx) {
            int8_t D[5][6] = {0};
            int8_t t_startx, t_endx;
            buildD(D, S, endx);
            // printMap(D, S);
            for (int k = 0; k < NUM_LINES; ++k) {
                compressMap(S, k, endx, result);
                decompressMap(result, S, t_startx, t_endx);
                moves[D[k][0]].write(reinterpret_cast<char*>(&result), sizeof result);
                numSolutionsPerMove[D[k][0]]++;
                // std::cout << (int)D[k][0] << ',';
            }
            // std::cout << '\n';
        }
        index++;
        std::cout << '\r' << index << '/' << statesLimit << "      ";
        return index < statesLimit;
    };
    forStateFunction(S, 0, getSolutionsPerState);
    std::ofstream numMovementsFile("numMovements.txt");
    for (int i = 0; i < 11; ++i) {
        numMovementsFile << i << "-->" << numSolutionsPerMove[i] << "\n";
    }
    numMovementsFile.flush();
}

void pickMaps(const std::string& fileName, const int numSolutionsPerMove[]) {
    std::ifstream movesIn[] = {
        std::ifstream(fileName + "1.in", std::ios::binary),
        std::ifstream(fileName + "2.in", std::ios::binary),
        std::ifstream(fileName + "3.in", std::ios::binary),
        std::ifstream(fileName + "4.in", std::ios::binary),
        std::ifstream(fileName + "5.in", std::ios::binary),
        std::ifstream(fileName + "6.in", std::ios::binary),
        std::ifstream(fileName + "7.in", std::ios::binary),
        std::ifstream(fileName + "8.in", std::ios::binary),
        std::ifstream(fileName + "9.in", std::ios::binary),
        std::ifstream(fileName + "10.in", std::ios::binary),
    };
    std::ofstream movesOut[] = {
        std::ofstream(fileName + "1_final.in"),
        std::ofstream(fileName + "2_final.in"),
        std::ofstream(fileName + "3_final.in"),
        std::ofstream(fileName + "4_final.in"),
        std::ofstream(fileName + "5_final.in"),
        std::ofstream(fileName + "6_final.in"),
        std::ofstream(fileName + "7_final.in"),
        std::ofstream(fileName + "8_final.in"),
        std::ofstream(fileName + "9_final.in"),
        std::ofstream(fileName + "10_final.in"),
    };
    {
        movesOut[0] << "uint64_t moves1[]={";
        movesOut[1] << "uint64_t moves2[]={";
        movesOut[2] << "uint64_t moves3[]={";
        movesOut[3] << "uint64_t moves4[]={";
        movesOut[4] << "uint64_t moves5[]={";
        movesOut[5] << "uint64_t moves6[]={";
        movesOut[6] << "uint64_t moves7[]={";
        movesOut[7] << "uint64_t moves8[]={";
        movesOut[8] << "uint64_t moves9[]={";
        movesOut[9] << "uint64_t moves10[]={";
    }
    const int finalNumberOfMaps        = *std::min_element(numSolutionsPerMove, numSolutionsPerMove + 10);
    const int numberOfMapsPerMovements = std::min(NUM_FINAL_MAPS, finalNumberOfMaps);
    int* nums                          = new int[numberOfMapsPerMovements];
    std::random_device rd;
    std::mt19937 mte(rd());
    char buffer[sizeof(uint64_t)];
    for (int j = 0; j < 10; ++j)
    {
        std::uniform_int_distribution<int> dist(0, numSolutionsPerMove[j]);
        std::generate(nums, nums + numberOfMapsPerMovements, [&]() { return dist(mte); });
        std::sort(nums, nums + numberOfMapsPerMovements);
        for (int i = 0; i < numberOfMapsPerMovements; ++i)
        {
            const std::streampos n = ((uint64_t) (nums[i])) * sizeof(uint64_t);
            // std::cout << "n=" << n << '\n';
            movesIn[j].seekg(n);
            movesIn[j].read(buffer, sizeof(uint64_t));
            movesOut[j] << *reinterpret_cast<uint64_t*>(buffer) << ",";
        }
    }
    for (int j = 0; j < 10; ++j)
    {
        movesOut[j] << "0};";
    }
    delete nums;
}


int main() {
    #ifdef OUTPUT_MAPS
        int numSolutionsPerMove[10] = {0};
        outputMaps("moves", numSolutionsPerMove);
    #else
        int numSolutionsPerMove[] = {
            281342008, 
            221497799, 
            108705497, 
            48151685, 
            19172755, 
            6757898, 
            1975830, 
            453294, 
            75670,
            48
        };
    #endif
    std::cout << "Done making maps" << std::endl;
    pickMaps("moves", numSolutionsPerMove);
    std::cout << "Done picking maps" << std::endl;
    return 0;
}
