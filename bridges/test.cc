#include "Bridges.h"
#include "AudioClip.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace bridges;

//Using Bridges' tutorial code to see if that can get a visualization
//NOT WORKING
int main() {
	Bridges bridges = Bridges(5, "gugu", "741219807521");
	bridges.setTitle("Testing Audio");
	bridges.setDescription("Blah blah blah");

	ifstream ins("BETTER!1stTUNE.wav"); //Main menu music, IF IT WORKED
	if (!ins) { throw "UH OH"; }
	else { ins.close(); }

	AudioClip ooga = AudioClip("BETTER!1stTUNE.wav");

	AudioClip mod = AudioClip(ooga.getSampleCount(), 1, ooga.getSampleBits(), ooga.getSampleRate());

	for (int i = 0; i < mod.getSampleCount(); i++) {
		int sampy = ooga.getSample(0, i);
		int newSampy = sampy / 2;
		mod.setSample(0, i, newSampy);
	}

	bridges.setDataStructure(ooga);
	bridges.visualize();

	bridges.setDataStructure(mod);
	bridges.visualize();

	return 0;
}
