@echo off
g++ main.cpp -O3 -s -std=c++17 -march=native ^
 -fno-exceptions -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -lopenal32
