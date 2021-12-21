

#include <Windows.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <format>
#include <fstream>
#include <streambuf>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;

struct Part {
    int length;
    int count;
};

int get_minimum_stock(vector<Part> parts, int stock_length, int Kerf = 3)
{
    vector<int> lengs;
    for (auto [length, count] : parts)
    {
        for (int i = 0; i < count; ++i) {
            assert(length < stock_length);
            lengs.push_back(length);
        }
    }
    

    int answer = 1000'000;

    vector<vector<int>> optimal_cutmap;
    vector<int> optimal_remains;
    int optimal_longest_remained = 0;

    sort(lengs.begin(), lengs.end());
    do {
        vector<int> remains;
        int longest_remained = 0;
        vector<vector<int>> cutmap_list;
       
        vector<int> cutmap;
        int stock_needed = 1, remained = stock_length;

        for (auto len : lengs)
        {
            int len_with_kerf = len + Kerf;
            if (remained - len_with_kerf < 0)
            {
                assert (cutmap.empty() == false);
                // save remained & current cutmap
                cutmap_list.push_back(cutmap);
                remains.push_back(remained);
                longest_remained = max(longest_remained, remained);
                    
                // refill stock.
                remained = stock_length;
                cutmap.clear();
                stock_needed++;

                if (stock_needed > answer) break; // early stop.
            }

            remained -= len_with_kerf;
            cutmap.push_back(len);
        }

        if (stock_needed <= answer) 
        {
            // handle last cutmap
            if (cutmap.empty() == false)
            {
                // save remained & current cutmap
                cutmap_list.push_back(cutmap);
                remains.push_back(remained);
                longest_remained = max(longest_remained, remained);
            }

            if (answer > stock_needed || longest_remained > optimal_longest_remained) {
                optimal_cutmap = cutmap_list;
                optimal_remains = remains;
                optimal_longest_remained = longest_remained;
            }
            answer = stock_needed;
        }

    } while (next_permutation(lengs.begin(), lengs.end()));

    // dump optimal cutmap.
    printf("\nneeded stock(%d): #%d\n", stock_length, answer);


    printf("\ncutmap(including kerf.%d):\n", Kerf);
    // make cuts into 2 parts for transfortable size.
    for (size_t i = 0; i < optimal_cutmap.size(); ++i) {
        
        auto& cuts = optimal_cutmap[i];

        //sort(cuts.begin(), cuts.end()); // already sorted.
        std::vector<int> part1, part2;
        int l1 = 0, l2 = 0;
        for (int s = 0, e = cuts.size() - 1; s < (int)cuts.size();)
        {
            part1.push_back(cuts[s]); if (++s > e) break;
            part2.push_back(cuts[s]); if (++s > e) break;

            part1.push_back(cuts[e]); if (--e < s) break;
            part2.push_back(cuts[e]); if (--e < s) break;
        }
        
        int part_length = 0;
        stringstream ss;
        for (auto i : part1) {
            ss << i << ", ";
            part_length += i + Kerf;
        }
        cout << format("    {}({})", ss.str(), part_length);

        ss.str("");
        part_length = 0;
        for (auto i : part2) {
            ss << i << ", ";
            part_length += i+Kerf;
        }
        cout << format(" | {}({})", ss.str(), part_length);
        cout << format(" | remained={}", optimal_remains[i]) << endl;

        const char filler = '.';
        string line(101, ' ');
        int fill = 0;
        int ith = 0;
        for (auto c : part1) {
            int ratio = c * 100 / 3600;
            std::fill_n(&line[fill], ratio, filler);
            int n = sprintf(&line[fill], "%d", c); line[fill+n] = filler;
            fill += ratio;
        }

        line[fill++] = '|';
        for (auto c : part2) {
            int ratio = c * 100 / 3600;
            std::fill_n(&line[fill], ratio, filler);
            int n = sprintf(&line[fill], "%d", c); line[fill + n] = filler;
            fill += ratio;
        }
        cout << "    [" << line << "]" << endl << endl;
    }
    return answer;
}


/*
input format:
    [general]
    ;type = linear

    part1 = 440x4
    part2 = 680x4
    part3 = 700x4

    stock_length = 3600
    kerf = 3
*/


int main(int argc, const char*argv[])
{
    if (argc != 2 || !fs::exists(argv[1]))
    {
        std::cout << "Usage:\n"
            << "\txxx.exe datafile\n\n";
            return -1;
    }

    // length output
    //string line(100, ' ');
    //vector<int> cuts = { 440, 680 };
    //int stock_length = 3600;
    //int fill = 0;
    //for (auto c : cuts) {
    //    int ratio = c * 100 / 3600;
    //    std::fill_n(&line[fill], ratio, '-');
    //    sprintf(&line[fill], "%d", c);
    //    fill += ratio;
    //}

    //cout << '[' << line << ']' << endl;
    //return 0;

    auto data_path = argv[1];// ".\\optimal_linear_data.txt";
    auto product = "general";

    std::string inifile = std::tmpnam(nullptr);
    {
        ofstream of(inifile);
        of << format("[{}]", product) << endl;

        std::ifstream t(data_path);
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        of << str;

        data_path = inifile.c_str();
    }

    try {
        char val[255] = {};
        //GetPrivateProfileStringA(product, "type", nullptr, val, 255, data_path);
        //if (val != "linear"s) {
        //    throw "invalid type: " + string(val);
        //}
        //string type = val;


        int stock_length = GetPrivateProfileIntA(product, "stock_length", 0, data_path);
        if (stock_length < 100 || 4000 < stock_length)
            throw format("invalid stock length: {}", stock_length);

        int kerf = GetPrivateProfileIntA(product, "kerf", 3, data_path);
        if (kerf < 1 || 10 < kerf)
            throw format("invalid kerf: {}", kerf);

        vector<Part> parts;
        for (int i = 1; i < 1000; ++i)
        {
            string key = "part" + to_string(i);
            GetPrivateProfileStringA(product, key.c_str(), nullptr, val, 255, data_path);
            int len = 0, cnt = 0;
            if (strlen(val) == 0) break;

            if (sscanf(val, "%d x %d", &len, &cnt) != 2)
                throw format("invalid {}: {}", key, val);
            parts.push_back({len,cnt});
        }

        cout << format("stock: {}(mm) ", stock_length) << endl;
        cout << format("Total parts: {}, kerf: {}mm\n", parts.size(), kerf);
        for (auto [l, c] : parts) {
            cout << format("\t{} x {}\n", l, c);
        }

        get_minimum_stock(parts, stock_length, kerf);
    }
    catch (const string& errmsg) {
        cout << format("Error: \n\t{}, en={}\n", errmsg.c_str(), GetLastError());
    }


    return 0;
}


