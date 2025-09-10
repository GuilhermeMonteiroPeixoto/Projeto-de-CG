#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "audio.h"

static Mix_Music* bgm = NULL;

void initAudio(void){
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0) {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == 0) {
            bgm = Mix_LoadMUS("torcida.ogg");
            if (bgm) {
                Mix_PlayMusic(bgm, -1);
                Mix_VolumeMusic(96);
            }
        }
    }
}

void stopAudio(void) {
    if (bgm) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgm);
        bgm = NULL;
    }

    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

