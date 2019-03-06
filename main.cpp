#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <cstdint>
#include <cmath>


// Buffer, Sound & Samplee
sf::SoundBuffer buffer;
sf::Sound sound;
const sf::Int16* samples;

// Range & Mapping
int len;
sf::Int16 min;
sf::Int16 max;

// Timing
sf::Clock dTime;
float timeElapsed;

sf::RenderWindow win;

bool active(){
    return buffer.getDuration().asMilliseconds() > dTime.getElapsedTime().asMilliseconds();
}

int millis(){
    return dTime.getElapsedTime().asMilliseconds();
}

float sec(){
    return dTime.getElapsedTime().asMilliseconds()/1000.0f;
}

int currentSampleIndex(){ 
    return millis()*(11025/1000);
}

sf::Int16 sampleAmplitude(int n){
    return samples[n];
}

sf::Int16 currentSampleAmplitude(){ 
    return samples[millis()*(buffer.getSampleRate()/1000)];
}

float mapBetween(sf::Int16 inMin, sf::Int16 inMax, float outMin, float outMax, sf::Int16 x){
    
    //*************************************************************************
    // So this needs fucking fixing!!! 
    // Probably errors with casting sf::Int16 to float
    //*************************************************************************7

    // Y = (X-A)/(B-A) * (D-C) + C
    // return (x - inMin) / (inMax - inMin) * (outMax - outMin) + outMin;
    
    return (static_cast<float>(x) + 16384.0f) / (32512.0f);
}

void windowInit(){
    win.create({1200, 800}, "Win");
}

void windowUpdate(){

}

void windowRender(){
    int index = currentSampleIndex()
    for(int i = )
}

void windowTerminate(){

}

void soundPreProcess(){
    sound.setBuffer(buffer);
    samples = buffer.getSamples();
    len = buffer.getSampleCount();
    min = 0;
    max = 0;

    for(int i = 0; i < len; i++){
        if (samples[i] < min)
            min = samples[i];
        if (samples[i] > max)
            max = samples[i];
    }
}

void soundStart(){
    sound.play();
}

void soundProcess(){
    float s = mapBetween(min, max, 0, 1, currentSampleAmplitude());
    // Prints amp
    for(float f = 0.0125; f <= s; f = f + 0.0125f)
        printf("*");
    printf("\n");
}

void stats(){
    // Reference print of max amp
    for(int i = 0; i < 80; i++){
        printf("*");
    }
    // Print stats
    printf("\n%d   :   %d    :    %d    :    %d    :    %d", min, max, len, dTime.getElapsedTime().asMilliseconds(), buffer.getSampleRate());
}

int main(){
    if(buffer.loadFromFile("canary.wav")){
        windowInit();
        soundPreProcess();
        soundStart();
        dTime.restart();
        while(active()){
            soundProcess();
            windowUpdate();
            windowRender();   
        }
        windowTerminate();
        stats();
    } else { return -1; }
    return 0;
}