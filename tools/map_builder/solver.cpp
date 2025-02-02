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
#include <set>
#include <unordered_set>
#include <utility>

#include "switches/red_switches.h"


struct pair_comparator {
    bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const {
        return lhs.first < rhs.first || (lhs.first == rhs.first && lhs.second < rhs.second);
    }
};

std::set<std::pair<int, int>, pair_comparator> redSwitchesIndices;

struct pair_hash {
    std::size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ std::hash<int>()(p.second);
    }
};

struct pair_equal {
    bool operator()(const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) const {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }
};

std::unordered_set<std::pair<int, int>, pair_hash, pair_equal> redSwitchControlled;
std::unordered_map<std::pair<int,int>, std::pair<int,int>, pair_hash, pair_equal> redSwitchesConnections;
std::unordered_map<std::pair<int,int>, std::pair<int,int>, pair_hash, pair_equal> redSwitchesConnectionsInverted;


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

#define __min(a, b)     (((a) < (b)) ? (a) : (b))
#define ___min(a, b, c) __min((a), (__min(b, c)))
#define BIG_FAT_VALUE   9
void buildD(int8_t D[5][6], const int8_t S[5][5], const int8_t endx) {
    forlines(l) {
        D[l][5] = BIG_FAT_VALUE;
    }

    D[endx][5] = 0;

    for (int c = NUM_COLS - 1; c >= 0; --c) {
        forlines(l) {
            const int d1 = (l + 1 < NUM_LINES) ? (D[l + 1][c + 1] + (S[l][c] == -1 ? 0 : 1)) : BIG_FAT_VALUE;
            const int d2 = D[l][c + 1] + (S[l][c] == 0 ? 0 : 1);
            const int d3 = (l > 0) ? (D[l - 1][c + 1] + (S[l][c] == 1 ? 0 : 1)) : BIG_FAT_VALUE;
            D[l][c]      = ___min(d1, d2, d3);
        }
    }
}

void buildDWithRedSwitches(int8_t D[5][6], const int8_t S[5][5], const int8_t SwithReds[5][5], const int8_t endx) {
    forlines(l) {
        D[l][5] = BIG_FAT_VALUE;
    }

    D[endx][5] = 0;

    for (int c = NUM_COLS - 1; c >= 0; --c) {
        forlines(l) {
            if (redSwitchesIndices.find({l, c}) != redSwitchesIndices.end()) {
                const int d1 = (l + 1 < NUM_LINES) ? (D[l + 1][c + 1] + (SwithReds[l][c] == -1 ? 0 : BIG_FAT_VALUE)) : BIG_FAT_VALUE;
                const int d2 = D[l][c + 1] + (SwithReds[l][c] == 0 ? 0 : BIG_FAT_VALUE);
                const int d3 = (l > 0) ? (D[l - 1][c + 1] + (SwithReds[l][c] == 1 ? 0 : BIG_FAT_VALUE)) : BIG_FAT_VALUE;
                D[l][c]      = ___min(d1, d2, d3) + (SwithReds[l][c] != S[l][c]);
            }
            else {
                const int d1 = (l + 1 < NUM_LINES) ? (D[l + 1][c + 1] + (SwithReds[l][c] == -1 ? 0 : 1)) : BIG_FAT_VALUE;
                const int d2 = D[l][c + 1] + (SwithReds[l][c] == 0 ? 0 : 1);
                const int d3 = (l > 0) ? (D[l - 1][c + 1] + (SwithReds[l][c] == 1 ? 0 : 1)) : BIG_FAT_VALUE;
                D[l][c]      = ___min(d1, d2, d3) + (SwithReds[l][c] != S[l][c]);
            }

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
    for (int i = 0; i < NUM_RED_SWITCHES; ++i) {
        int ri = red_switches_indices[i].line;
        int rj = red_switches_indices[i].column;
        auto[ri_controlled, rj_controlled] = redSwitchesConnectionsInverted[{ri,rj}];
        if (S[ri][rj] != S[ri_controlled][rj_controlled]) {
            return false;
        }
    }
    return true;
}

bool areRedSwitchesValid(const int8_t S[NUM_LINES][NUM_COLS], const int8_t redSwitches[NUM_LINES][NUM_COLS]) {
    for (int i = 0; i < NUM_COLS; ++i) {
        if (redSwitches[0][i] == 1 || redSwitches[NUM_LINES - 1][i] == -1) {
            return false;
        }
    }
    for (int l = 0; l < NUM_LINES - 1; ++l) {
        for (int c = 0; c < NUM_COLS; ++c) {
            if (redSwitches[l][c] == -1 && redSwitches[l + 1][c] == 1) {
                return false;
            }
            if (S[l][c] == -1 && redSwitches[l + 1][c] == 1) {
                return false;
            }
            if (redSwitches[l][c] == -1 && S[l + 1][c] == 1) {
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
        if (redSwitchControlled.find({i, j}) != redSwitchControlled.end()) {
            auto redSwitchControllerIndices = redSwitchesConnectionsInverted.at({i, j});
            S[i][j] = S[redSwitchControllerIndices.first][redSwitchControllerIndices.second];
            if (!forStateFunction(S, indexNum + 1, bodyLogic)) {
                return false;
            }
        }
        else {
            for (S[i][j] = -1; S[i][j] <= 1; ++S[i][j]) {
                if (!forStateFunction(S, indexNum + 1, bodyLogic)) {
                    return false;
                }
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
        std::ofstream(fileName + "3.in", std::ios::binary), std::ofstream(fileName + "4.in", std::ios::binary)};
    int8_t S[5][5]        = {0};
    int index             = 0;
    uint64_t result;
    constexpr int64_t statesLimit = std::pow(2, 20);
    auto getSolutionsPerState     = [&]() {
        for (int endx = 0; endx < NUM_LINES; ++endx) {
            int8_t t_startx, t_endx;
            int firstColumn[NUM_LINES] = {BIG_FAT_VALUE};
            for (int redSwitchStateIndex = 0; redSwitchStateIndex <  std::pow(3,NUM_RED_SWITCHES); ++redSwitchStateIndex) {
                int8_t SwithReds[5][5] = {0};
                int8_t D[5][6] = {0};
                forall(i, j, SwithReds[i][j] = S[i][j];);
                for(int i = 0; i < NUM_RED_SWITCHES; ++i) {
                    int ri = red_switches_indices[i].line;
                    int rj = red_switches_indices[i].column;
                    int redSwitchVal = (redSwitchStateIndex / (int)std::pow(3, i)) % 3 - 1; // 0/0 problem
                    SwithReds[ri][rj] = redSwitchVal;
                    auto[ri_controlled, rj_controlled] = redSwitchesConnections[{ri,rj}];
                    SwithReds[ri_controlled][rj_controlled] = redSwitchVal;
                }
                if (!isMapValid(SwithReds)) {
                    continue;
                }
                buildDWithRedSwitches(D, S, SwithReds, endx);
                forlines(l) {
                    if (D[l][0] < firstColumn[l]) {
                        firstColumn[l] = D[l][0];
                    }
                }
            }
            for (int k = 0; k < NUM_LINES; ++k) {
                compressMap(S, k, endx, result);
                decompressMap(result, S, t_startx, t_endx);
                moves[firstColumn[k]].write(reinterpret_cast<char*>(&result), sizeof result);
                numSolutionsPerMove[firstColumn[k]]++;
            }
        }
        index++;
        std::cout << '\r' << index << '/' << statesLimit << "      ";
        return index < statesLimit;
    };
    forStateFunction(S, 0, getSolutionsPerState);
    std::ofstream numMovementsFile("numMovements.txt");
    for (int i = 0; i < 5; ++i) {
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
    };
    std::ofstream movesOut[] = {
        std::ofstream(fileName + "1_final.in", std::ios::binary),
        std::ofstream(fileName + "2_final.in", std::ios::binary),
        std::ofstream(fileName + "3_final.in", std::ios::binary),
        std::ofstream(fileName + "4_final.in", std::ios::binary),
    };

    const int finalNumberOfMaps        = *std::min_element(numSolutionsPerMove, numSolutionsPerMove + 5);
    const int numberOfMapsPerMovements = std::min(1 << 16, finalNumberOfMaps);
    int* nums                          = new int[numberOfMapsPerMovements];
    std::random_device rd;
    std::mt19937 mte(rd());
    std::uniform_int_distribution<int> dist(0, finalNumberOfMaps);
    std::generate(nums, nums + numberOfMapsPerMovements, [&]() { return dist(mte); });
    std::sort(nums, nums + numberOfMapsPerMovements);
    int8_t t_startx = 0, t_endx = 0;
    int8_t S[5][5] = {0};
    for (int i = 0; i < numberOfMapsPerMovements; ++i) {
        char buffer[4][sizeof(uint64_t)];
        const std::streampos n = ((uint64_t) (nums[i])) * sizeof(uint64_t);
        movesIn[0].seekg(n);
        movesIn[0].read(buffer[0], sizeof(uint64_t));
        uint64_t* compressedMap = reinterpret_cast<uint64_t*>(buffer[0]);
        if (!decompressMap(*compressedMap, S, t_startx, t_endx)) {
            break;
        }
        // std::cout << std::bitset<64>(*compressedMap) << "\n";
        movesOut[0].write(buffer[0], sizeof(uint64_t));
        movesIn[1].seekg(n);
        movesIn[1].read(buffer[1], sizeof(uint64_t));
        movesOut[1].write(buffer[1], sizeof(uint64_t));
        movesIn[2].seekg(n);
        movesIn[2].read(buffer[2], sizeof(uint64_t));
        movesOut[2].write(buffer[2], sizeof(uint64_t));
        movesIn[3].seekg(n);
        movesIn[3].read(buffer[3], sizeof(uint64_t));
        movesOut[3].write(buffer[3], sizeof(uint64_t));
    }
    delete nums;
}

void buildRedSwitchMaps() {
    for (int i = 0; i < NUM_RED_SWITCHES; i++) {
        redSwitchesIndices.insert({red_switches_indices[i].line, red_switches_indices[i].column});
    }
    for (int i = 0; i < NUM_RED_SWITCHES; i++) {
        redSwitchControlled.insert({red_switch_controlled[i].line, red_switch_controlled[i].column});
    }
    for (int i = 0; i < NUM_RED_SWITCHES; ++i) {
        redSwitchesConnections[{red_switches_connections[i].line_a, red_switches_connections[i].col_a}] = {red_switches_connections[i].line_b, red_switches_connections[i].col_b};
    }
    for (int i = 0; i < NUM_RED_SWITCHES; ++i) {
        redSwitchesConnectionsInverted[{red_switches_connections[i].line_b, red_switches_connections[i].col_b}] = {red_switches_connections[i].line_a, red_switches_connections[i].col_a};
    }
}


int main() {
    int numSolutionsPerMove[5] = {0};
    buildRedSwitchMaps();
    outputMaps("moves", numSolutionsPerMove);
    std::cout << "Done making maps" << std::endl;
    pickMaps("moves", numSolutionsPerMove);
    std::cout << "Done picking maps" << std::endl;
    return 0;
}
