#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

//Should look something like this
//I'm ngl I tried to do this in VSCode but idk how to use it so I wrote it here first :skull:
int main() {
	if (SDL_Init(SDL_INIT_AUDIO) < 0) return 1;

	if (Mix_OpenAudio(44100, MIX_DEFALT_FORMAT, 2, 2048) < 0) {
		cout << "SDL Mixer can't initialize; Error: " << Mix_GetError());
		return 1;
	}

	Mix_Chunk *sound = Mix_LoadWAV("path_to_sound.wav"); //Plays the song
	if (sound == NULL) {
		cout << "Failed to load sound; Error: " << Mix_GetError());
		return 1;
	}

	Mix_PlayMusic(music, -1); //Loops the song
	Mix_FreeMusic(music);
	Mix_CloseAudio();
	SDL_Quit();

	return 0;
}
