#include "engine.h"
#include <iostream>
#include <stdexcept>
void require(bool ok){if(!ok)throw std::runtime_error("Test failed");}
int main(){std::mt19937 rng(12345);for(int target:{42,34,28})for(int run=0;run<12;++run){auto p=sudoku::generate(rng,target);int clues=0;for(int i=0;i<81;++i){require(p.solution[i]>=1&&p.solution[i]<=9);require(sudoku::allowed(p.solution,i,p.solution[i]));if(p.clues[i]){++clues;require(p.clues[i]==p.solution[i]);}}require(clues>=target&&clues<50);auto copy=p.clues;require(sudoku::countSolutions(copy)==1);require(copy==p.clues);}
    sudoku::Board b{};b[0]=5;require(!sudoku::allowed(b,8,5));require(!sudoku::allowed(b,72,5));require(!sudoku::allowed(b,10,5));require(sudoku::allowed(b,40,5));std::cout<<"PASS: 36 unique puzzles, complete solutions, clue preservation and move validation\n";}
