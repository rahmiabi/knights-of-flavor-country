#include "Bridges.h"
#include "AudioClip.h"
#include <iostream>

using namespace bridges;
using namespace std;

int main() {
	//THIS IS THE ONLY WORKING ONE
	Bridges bridges = Bridges(5, "gugu", "741219807521");
	bridges.setTitle("Music Visualization");

	//double freq[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
	const int sampyRate = 44100; //Hz
	const int numSampy = sampyRate * 35; //35 Seconds
	AudioClip ac = AudioClip(numSampy, 1, 32, sampyRate);

	//Intro tune:

	for (int i = 0; i < ac.getSampleCount(); i++) {
		double freq = 65.4;
		double phase = 0;
		double amp = INT_MAX / 4.0;
		double time = (double)i / sampyRate;
		double val = sin((2 * M_PI) * freq * time + phase);
		int sampyVal = (int)(val * amp);
		ac.setSample(0, i, sampyVal);
	}

	bridges.setDataStructure(ac);
	bridges.visualize();

	//Main menu music:
	/*
	vector<double> efVec; //Holds all the different frequencies for the song in a vector (written in E Minor)
	const int numSampy1 = sampyRate * 50;
	AudioClip ec = AudioClip(numSampy1, 1, 32, sampyRate);

	for (int i = 0; i < fVec.size(); i++) {
		double freq = fVec.at(i);
		double phase = 0;
		double amp = INT_MAX / 4.0;
		double time - (doubel)i / sampyRate;
		double val = sin((2 * M_PI) * freq * time + phase);
		int sampyVal = (int)(val * amp);
		ac.setSample(0, i, sampyVal);
	}

	bridges.setDataSstrcture(ec);
	bridges.visualize();
	*/


	//Settings music (STARTS WITH A SFX):
	//SFX for choosing singleplayer
	//SFX for choosing multiplayer

	return 0;
}


