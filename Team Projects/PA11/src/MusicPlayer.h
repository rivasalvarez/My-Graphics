#ifndef MusicPlayer_H
#define MusicPlayer_H

#include <iostream>
#include <SFML/Audio.hpp>
#include <chrono>
#include <ctime>
#include <cstdlib>

#define NUM_SONGS 6

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
      std::string themeSongs[NUM_SONGS]   = {"Everything.ogg","cat.ogg","galaxy.ogg","sandstorm.ogg",
                                             "levels.ogg","danza.ogg"};
      int previous;
      int beforeP;
      float musicDelay;
      sf::Music music;

      sf::SoundBuffer fallBuff;
      sf::Sound fall;
      sf::SoundBuffer acheiveBuff;
      sf::Sound acheive;

      std::chrono::time_point<std::chrono::high_resolution_clock> t1,t2;
};

#endif
