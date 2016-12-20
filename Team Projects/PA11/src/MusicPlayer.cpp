#include "MusicPlayer.h"
#include "GameEngine.h"

MusicPlayer::MusicPlayer(){
      srand(time(NULL));
      musicDelay = 0;
      t1 = std::chrono::high_resolution_clock::now();


      fallBuff.loadFromFile("fall.ogg");
      fall.setBuffer(fallBuff);


      acheiveBuff.loadFromFile("acheive.ogg");
      acheive.setBuffer(acheiveBuff);
      int previous = rand() % NUM_SONGS;
      beforeP = previous;
      music.openFromFile(themeSongs[previous].c_str());
}

MusicPlayer::~MusicPlayer(){

}

void MusicPlayer::update(){
     int status = music.getStatus();
     //time variables
     t2 = std::chrono::high_resolution_clock::now();
     float dt = std::chrono::duration_cast< std::chrono::duration<float> >(t2-t1).count();
     t1 = std::chrono::high_resolution_clock::now();
   
     if (musicDelay >= 0)
        musicDelay -= dt;
     // music stopped
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
   
   // Play background music
   music.play();

   return true;
}

void MusicPlayer::playNext(){
     bool flag = true;
     int next;
     // get a new song
     while(flag){
        next = rand() % NUM_SONGS;
        if(next != previous && next != beforeP)
           flag = false;
     }
     // open and play new song
     music.openFromFile(themeSongs[next].c_str());

     music.play();
     beforeP = previous;
     previous = next;
}
