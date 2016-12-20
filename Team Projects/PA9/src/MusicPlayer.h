#ifndef MusicPlayer_H
#define MusicPlayer_H

#include <iostream>
#include <SFML/Audio.hpp>
#include <chrono>
#include <ctime>
#include <cstdlib>

#define NUM_SONGS 4

class GameEngine;
class PhysicsEngine;
class Graphics;

class MusicPlayer {
   public:
      // Constructor/Destructor
      MusicPlayer();
      ~MusicPlayer();
      void update();
      bool initialize(GameEngine*);
      void playNext();
      void pause();
      void play();
      friend GameEngine;
      friend PhysicsEngine;
      friend Graphics;

   private:
      GameEngine *gameEngine;
      std::string themeSongs[NUM_SONGS]   = {"Original.wav","wildPokemon.wav","Melee.wav","Johto.wav"};
      std::string normalThemes[NUM_SONGS] = {"Mulan.wav", "Sea.wav", "Snowman.wav", "Tarzan.wav"};
      int previous;
      int beforeP;
      float musicDelay;
      sf::Music music;

      sf::SoundBuffer pikachuBuff;
      sf::Sound pikachu;

      sf::SoundBuffer charmanderBuff;
      sf::Sound charmander;

      sf::SoundBuffer goalBuff;
      sf::Sound goal;
   
      sf::SoundBuffer hitBuff;
      sf::Sound hit;
      std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
};

#endif
