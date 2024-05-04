#include "Bridges.h"
#include "AudioClip.h"
#include <iostream>
#include <map>

using namespace bridges;
using namespace std;

map<string, double> eMin = {
	{"A#3", 233.08}, {"A3", 220.00}, {"G3", 196.00}, {"D#3", 155.56},
	{"F3", 174.61}, {"D3", 146.83}, {"C3", 130.81}, {"G4", 392.00},
	{"A#4", 466.16}, {"A4", 440.00}, {"D#4", 311.13}, {"F4", 349.23},
	{"D4", 293.66}, {"C4", 261.63}
};

int main() {
	//THIS IS THE ONLY WORKING ONE
	Bridges bridges = Bridges(5, "gugu", "741219807521");
	bridges.setTitle("Music Visualization");

	//double freq[] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
	const int sampyRate = 44100; //Hz
	const int numFreqRaise = sampyRate * 5;
	const int numSampy = sampyRate * 35; //35 Seconds
	AudioClip ac = AudioClip(numFreqRaise, 1, 32, sampyRate);

	//Intro tune:
	//TODO:Raising in frequency like it's powering up
	double startFreq = 256;
	double endFreq = 329.6;

	int sampIndex = 0;

	for (int i = 0; i < numFreqRaise; i++) { //ac.getSampleCount / 2 == Half note; / 4 == Quarter note
		double freq = startFreq + (endFreq - startFreq) * ((double)i/numFreqRaise); //Raise pitch, kind of like shifting positions on a violin
		//double freq = startFreq;
		double phase = 0;
		double amp = INT_MAX / 4.0;
		double time = (double)i / sampyRate;
		double val = sin((2 * M_PI) * freq * time + phase);
		int sampyVal = (int)(val * amp);
		ac.setSample(0, i, sampyVal);
	}

	/*
	for (int i = ac.getSampleCount() / 2; i < ac.getSampleCount(); i++) {
		double freq = endFreq;
		double phase = 0;
		double amp = INT_MAX / 4.0;
		double time = (double)i / sampyRate;
		double val = sin((2 * M_PI) * freq * time + phase);
		int sampyVal = (int)(val * amp);
		ac.setSample(0, i, sampyVal);
		
	}
	*/

	vector<string> notes = {"A#4", "A3", "G3", "D#3", "A3", "G3", "F3", "D3", "G3", "F3", "D#3", "C3", "G3", "A3", "D3", "A#4", "A4", "G4", "D#4", "A4", "D4", "G4", "F4", "D#4", "C4", "G4", "A4", "D4"};

	AudioClip ac1 = AudioClip(numSampy, 1, 32, sampyRate);
	int noteDuration = sampyRate;
	for (size_t i = 0; i < notes.size(); i++) {
		for (int j = 0; j < sampyRate; j++) {
		int sampleIndex = numFreqRaise + i * noteDuration + j;
		if (sampleIndex < ac1.getSampleCount()) {
			double frequency = eMin[notes[i]];
			double phase = 0;
			double amp = INT_MAX / 4.0;
			double time = (double)sampleIndex / sampyRate;
			double val = sin((2 * M_PI) * frequency * time + phase);
			int sampyVal = (int)(val * amp);
			ac1.setSample(0, sampleIndex, sampyVal);
			}
		}
	}

	

	bridges.setDataStructure(ac);
	bridges.visualize();

	
	bridges.setDataStructure(ac1);
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


