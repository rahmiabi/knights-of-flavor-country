#include "Bridges.h"
#include "AudioClip.h"
#include <iostream>

using namespace bridges;
using namespace std;

//Input some of the mixClips that Kerney has

int main() {
	//THIS IS THE ONLY WORKING ONE
	Bridges bridges = Bridges(5, "gugu", "741219807521");
	bridges.setTitle("Music Visualization");

	//double freq[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
	const int sampyRate = 44100; //Hz
	const int numSampy = sampyRate * 10; //35 Seconds
	AudioClip ac = AudioClip(numSampy, 1, 32, sampyRate);

	//Intro tune:
	//TODO:Raising in frequency like it's powering up
	double startFreq = 65.4;
	double endFreq = 164.814;

	for (int i = 0; i < ac.getSampleCount(); i++) {
		double freq = startFreq + (endFreq - startFreq) * ((double)i/ac.getSampleCount());
		double phase = 0;
		double amp = INT_MAX / 4.0;
		double time = (double)i / sampyRate;
		double val = sin((2 * M_PI) * freq * time + phase);
		int sampyVal = (int)(val * amp);
		ac.setSample(0, i, sampyVal);
	}

	bridges.setDataStructure(ac);
	bridges.visualize();

	/*
	//Main menu music:
	vector<double> efVec; //Holds all the different frequencies for the song in a vector (written in E Minor)
	const int numSampy1 = sampyRate * 50;
	AudioClip ec = AudioClip(numSampy1, 1, 32, sampyRate);

	//TODO:Figure out how to put the exact notes in
	for (int i = 0; i < efVec.size(); i++) {
		double freq = efVec.at(i);
		double phase = 0;
		double amp = INT_MAX / 4.0;
		double time = (double)i / sampyRate;
		double val = sin((2 * M_PI) * freq * time + phase);
		int sampyVal = (int)(val * amp);
		ac.setSample(0, i, sampyVal);
	}

	bridges.setDataStructure(ec);
	bridges.visualize();
	*/


	//Settings music (STARTS WITH A SFX):
	//SFX for choosing singleplayer (one note, good positive sounding one)
	//SFX for choosing multiplayer (two notes, good positive sounding one)
	//Exit SFX (very sad note D:)

	return 0;
}


