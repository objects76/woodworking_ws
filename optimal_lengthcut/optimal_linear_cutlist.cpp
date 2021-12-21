

#include <Windows.h>

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <format>

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

    //printf("\nremains: longest=%d\n", optimal_longest_remained);
    //printf("    > ");
    //for (auto c : optimal_remains) printf("%d, ", c);
    //printf("\n");


    printf("\ncutmap(including kerf.%d):\n", Kerf);
    //for (auto& cuts : optimal_cutmap) {
    //    printf("    > ");
    //    for (auto c : cuts) printf("%d, ", c);
    //    printf("\n");
    //}
    
    // make cuts into 2 parts.
    for (size_t i = 0; i < optimal_cutmap.size(); ++i) {

        auto& cuts = optimal_cutmap[i];
        //sort(cuts.begin(), cuts.end()); // already sorted.
        stringstream s1, s2;
        int l1=0, l2 = 0;
        for (int s = 0, e = cuts.size()-1; s < (int)cuts.size();)
        {
            s1 << cuts[s] << ", "; l1 += cuts[s]+ Kerf; if (++s > e) break;
            s2 << cuts[s] << ", "; l2 += cuts[s]+ Kerf; if (++s > e) break;

            s1 << cuts[e] << ", "; l1 += cuts[e] + Kerf; if (--e < s) break;
            s2 << cuts[e] << ", "; l2 += cuts[e] + Kerf; if (--e < s) break;
        }
        printf("   >> %s(%d) || %s(%d) || remained=%d\n",
            s1.str().c_str(), l1,
            s2.str().c_str(), l2,
            optimal_remains[i]);
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
int main()
{
    auto data_path = ".\\optimal_linear_data.txt";
    auto product = "general";

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


