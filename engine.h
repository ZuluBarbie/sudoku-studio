#pragma once
#include <algorithm>
#include <array>
#include <numeric>
#include <random>
#include <vector>

namespace sudoku {
using Board = std::array<int,81>;
inline bool allowed(const Board& b,int p,int n) {
    for(int i=0;i<81;++i) if(i!=p && b[i]==n &&
        (i/9==p/9 || i%9==p%9 || (i/27==p/27 && i%9/3==p%9/3))) return false;
    return true;
}
inline int countSolutions(Board& b,int limit=2) {
    int p=-1,best=10; std::vector<int> choices;
    for(int i=0;i<81;++i) if(!b[i]) {
        std::vector<int> c;
        for(int n=1;n<=9;++n) if(allowed(b,i,n)) c.push_back(n);
        if(c.empty()) return 0;
        if(int(c.size())<best) {p=i;best=int(c.size());choices=c;}
        if(best==1) break;
    }
    if(p<0) return 1;
    int total=0;
    for(int n:choices) {b[p]=n;total+=countSolutions(b,limit-total);if(total>=limit) break;}
    b[p]=0;return total;
}
struct Puzzle {Board clues{},solution{};};
inline Puzzle generate(std::mt19937& rng,int target) {
    std::array<int,9> digits{1,2,3,4,5,6,7,8,9};
    std::shuffle(digits.begin(),digits.end(),rng);
    auto order=[&] {std::array<int,3> groups{0,1,2};std::shuffle(groups.begin(),groups.end(),rng);
        std::array<int,9> result{};int k=0;
        for(int g:groups){std::array<int,3> v{0,1,2};std::shuffle(v.begin(),v.end(),rng);for(int x:v) result[k++]=g*3+x;}return result;};
    auto rows=order(),cols=order();Puzzle p;
    for(int r=0;r<9;++r) for(int c=0;c<9;++c) p.solution[r*9+c]=digits[(rows[r]*3+rows[r]/3+cols[c])%9];
    p.clues=p.solution;std::array<int,81> cells{};std::iota(cells.begin(),cells.end(),0);std::shuffle(cells.begin(),cells.end(),rng);
    int remaining=81;
    for(int i:cells){if(remaining<=target) break;int old=p.clues[i];p.clues[i]=0;Board copy=p.clues;
        if(countSolutions(copy)==1)--remaining;else p.clues[i]=old;}
    return p;
}
}
