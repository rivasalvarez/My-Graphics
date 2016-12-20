#include "MusicPlayer.h"
#include "GameEngine.h"

MusicPlayer::MusicPlayer(){
      previous = 0;
      beforeP = 0;
      musicDelay = 0;
      t1 = std::chrono::high_resolution_clock::now();

      pikachuBuff.loadFromFile("pikachu.wav");
      pikachu.setBuffer(pikachuBuff);

      charmanderBuff.loadFromFile("jiggly.wav");
      charmander.setBuffer(charmanderBuff);

      hitBuff.loadFromFile("hit.wav");
      hit.setBuffer(hitBuff);

      goalBuff.loadFromFile("goal.wav");
      goal.setBuffer(goalBuff);

      music.openFromFile("Mulan.wav");
}

MusicPlayer::~MusicPlayer(){

}

void MusicPlayer::update(){
     int status = music.getStatus();

     t2 = std::chrono::high_resolution_clock::now();
     float dt = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
     t1 = std::chrono::high_resolution_clock::now();

     if (musicDelay >= 0)
        musicDelay -= dt;

     if (status == 0){
      musicDelay = .3;
      playNext();
     }
}

void MusicPlayer::play(){

   music.play();
}


void MusicPlayer::pause(){

   music.pause();
}



bool MusicPlayer::initialize(GameEngine *ptr) {
   gameEngine = ptr;
   
   // Play background music on loop
   music.play();

   return true;
}

void MusicPlayer::playNext(){
     bool flag = true;
     int next;

     while(flag){
        srand(time(NULL));
        next = rand() % NUM_SONGS;
        if(next != previous && next != beforeP)
           flag = false;
     }
     if(gameEngine->pokemon)
       music.openFromFile(themeSongs[next].c_str());
     else
       music.openFromFile(normalThemes[next].c_str());

     music.play();
     beforeP = previous;
     previous = next;
}
