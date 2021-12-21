#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>

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
    printf("\nneeded stock: #%d\n", answer);

    printf("\nremains: longest=%d\n", optimal_longest_remained);
    printf("    > ");
    for (auto c : optimal_remains) printf("%d, ", c);
    printf("\n");


    printf("\ncutmap:\n");
    //for (auto& cuts : optimal_cutmap) {
    //    printf("    > ");
    //    for (auto c : cuts) printf("%d, ", c);
    //    printf("\n");
    //}
    // make cuts to 2 parts.
    for (auto& cuts : optimal_cutmap) {

        stringstream s1, s2;
        int l1=0, l2 = 0;
        for (int s = 0, e = cuts.size()-1; s < (int)cuts.size();)
        {
            s1 << cuts[s] << ", "; l1 += cuts[s]+ Kerf; if (++s > e) break;
            s2 << cuts[s] << ", "; l2 += cuts[s]+ Kerf; if (++s > e) break;

            s1 << cuts[e] << ", "; l1 += cuts[e] + Kerf; if (--e < s) break;
            s2 << cuts[e] << ", "; l2 += cuts[e] + Kerf; if (--e < s) break;
        }
        printf("   >> %s(%d) || %s(%d)\n",
            s1.str().c_str(), l1,
            s2.str().c_str(), l2);
    }

    return answer;
}

int main()
{
    vector<Part> parts = {
        {440,4}, {680,4}, {700,4},
    };
    int stock_length = 3600;
    int kerf = 3;

    int n = get_minimum_stock(parts, stock_length, kerf);


    return 0;
}


