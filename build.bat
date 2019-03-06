@echo off
g++ main.cpp -O3 -s -std=c++17 -march=native -DUSE_KISS_FFT^
 -fno-exceptions -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-system -lopenal32
