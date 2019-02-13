#include <SFML/Audio.hpp>
#include <iostream>
#include "../Gist/src/gist.h"

int main(){
    sf::SoundBuffer buffer;
    if(!buffer.loadFromFile("canary.wav"))
        return -1;
    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();
    sound.setPitch(0.5f);
    while(1){sound.setPitch(sound.getPitch()*1.00000000001);}
    return 0;
}