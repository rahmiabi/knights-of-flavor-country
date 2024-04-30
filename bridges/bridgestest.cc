#include "Bridges.h"
#include "AudioClip.h"

using namespace std;
using namespace bridges;

int main() {
	//TODO: Make one SHORT SONG using BRIDGES (I'm only making one) to meet role reqs
	//TODO: Make SFX using BRIDGES to meet role reqs (This one is doable)
	Bridges bridges(5, "gugu", "1058853664803");
	bridges.setTitle("Task Force Smth OST");
	AudioClip song("main_theme.wav"); //In the actual game, we'll probably be changing the song every time an event occurs

	int sampleCount = song.getSampleCount();
	int channelCount = song.getNumChannels();
	int bitDepth = song.getSampleBits();
	int sampleRate = song.getSampleRate();

	cout << "Sample Count: " << sampleCount << "\n"
		 << "Channel Count: " << channelCount << "\n"
		 << "Bit Depth: " << bitDepth << "\n"
		 << "Sample Rate: " << sampleRate << "\n";


	bridges.setDataStructure(&song);
	bridges.visualize();

	return 0;
}
