

#include <Windows.h>

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>
namespace fs = std::filesystem;
using namespace std;

struct Part2D {
    int long_grain, cross_grain;

    bool operator >(const Part2D& rhs) const {
        if (long_grain == rhs.long_grain && cross_grain == rhs.cross_grain)
            return false;

        return !(this->operator<(rhs));
    }
    bool operator <(const Part2D& rhs) const {

        if (this->long_grain < rhs.long_grain)
            return true;
        if (this->long_grain > rhs.long_grain)
            return false;
        return this->cross_grain < rhs.cross_grain;
    }

    int area() const { return cross_grain * long_grain; }
};

struct Sheet {
    int long_grain, cross_grain;
    vector<int> part_map;
    Sheet(int longG, int crossG) {
        part_map.resize(longG * crossG, 0);


    }
    int xy2index(int aG, int aCG) {
        return aCG* (long_grain + 1) + aG;
    }
    
    int& cell(int x, int y) { // =(long, cross)
        return part_map[y * (long_grain + 1) + x];
    }

    bool fill(int lgLen, int cgLen) {
        // find empty slot.
        int x, y;
        for (y = 0; y < cross_grain; ++y)
        {
            for (x = 0; x < long_grain; ++x)
            {
                if (cell(x, y) == 0) break;
            }
        }

        // try fill.
        return false;
    }
};
int get_minimum_stock(vector< pair<Part2D, int>> packed_parts, int stock_h, int stock_w)
{
    // unroll
    vector<Part2D> parts;
    for (auto& [p, c] : packed_parts) {
        while(c-- > 0) parts.push_back(p);
    }

    sort(parts.begin(), parts.end(), greater<Part2D>());
    for (auto& p : parts) {
        cout << format("{} x {}, area={}\n", p.long_grain, p.cross_grain, p.area());
    }
    return -1;
}



int main(int argc, const char* argv[])
{
    int main_linear(int argc, const char* argv[]);
    return main_linear(argc, argv);

    vector< pair<Part2D, int>> parts = {
        {{2101,350}, 2 },
        {{601,350},2},
        {{320,272},1},
        {{320,300},4},
        {{600,280},3},
    };

    int stock_h = 2440, stock_w = 1220; // 4*8 feet.
    get_minimum_stock(parts, stock_h, stock_w);

    return 0;
}


