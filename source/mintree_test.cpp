#include "mintree.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;
using namespace treens;

int main ()
{
    MinTree <int> tree;
    vector <int> res;

    while (!cin.eof ())
    {
        char c;
        int v;
        cin >> c;

        if (c == 'q' || cin.eof ())
            break;

        cin >> v;
        if (c == 'k')
            tree.insert (v);
        else if (c == 'm')
            res.push_back (tree[v - 1]);
        else if (c == 'n')
        {
            auto it = tree.lower_bound (v);
            if (it == tree.end ())
                res.push_back (tree.size ());
            else
                res.push_back (it.position ());
        }
    }

    for (auto i : res)
        cout << i << endl;

    return 0;
}