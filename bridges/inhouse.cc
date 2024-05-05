#include "Bridges.h"
#include "AudioClip.h"
#include <iostream>
#include <map>

using namespace bridges;
using namespace std;

//Maps the note name to its frequency
map<string, double> eMin = {
	{"A#3", 233.08}, {"A3", 220.00}, {"G3", 196.00}, {"D#3", 155.56},
	{"F3", 174.61}, {"D3", 146.83}, {"C3", 130.81}, {"G4", 392.00},
	{"A#4", 466.16}, {"A4", 440.00}, {"D#4", 311.13}, {"F4", 349.23},
	{"D4", 293.66}, {"C4", 261.63}, {"F#3", 184.99}, {"G5", 783.99}
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
		//Rest of this is just based ont eh Bridges tutorial code
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

	/*
	vector<string> notes = {"A#4", "A3", "G3", "D#3", "A3", "G3", "F3", "D3", "G3", "F3", "D#3", "C3", "G3", "A3", "D3", "A#4", "A4", "G4", "D#4", "A4", "D4", "G4", "F4", "D#4", "C4", "G4", "A4", "D4"};
	*/

	AudioClip ac1 = AudioClip(numSampy, 1, 32, sampyRate);

vector<pair<string,int>> notes = { //My music composition
		{{"A#3"}, sampyRate / 5},
		{{"A3"}, sampyRate / 5 },
		{{"G3"}, sampyRate / 5},
		{{"D#3"}, sampyRate},
		{{"A3"}, sampyRate / 5},
		{{"G3"}, sampyRate / 5},
		{{"F3"}, sampyRate / 5},
		{{"D3"}, sampyRate},
		{{"G3"}, sampyRate / 5},
		{{"F3"}, sampyRate /5},
		{{"D3"}, sampyRate / 5},
		{{"C3"}, sampyRate},
		{{"G3"}, sampyRate / 2},
		{{"A3"}, sampyRate / 2},
		{{"D3"}, sampyRate / 2},
		{{"A#4"}, sampyRate / 5},
		{{"A4"}, sampyRate / 5},
		{{"G4"}, sampyRate / 5},
		{{"D#4"}, sampyRate},
		{{"A4"}, sampyRate / 5},
		{{"G4"}, sampyRate / 5},
		{{"F4"}, sampyRate / 5},
		{{"D4"}, sampyRate },
		{{"G4"}, sampyRate / 5},
		{{"F4"}, sampyRate / 5},
		{{"D#4"}, sampyRate / 5},
		{{"C4"}, sampyRate / 2},
		{{"G4"}, sampyRate},
		{{"A4"}, sampyRate},
		{{"D4"}, sampyRate},
		{{"A#3"}, sampyRate / 5},
		{{"A3"}, sampyRate / 5 },
		{{"G3"}, sampyRate / 5},
		{{"D#3"}, sampyRate},
		{{"A3"}, sampyRate / 5},
		{{"G3"}, sampyRate / 5},
		{{"F3"}, sampyRate / 5},
		{{"D3"}, sampyRate},
		{{"G3"}, sampyRate / 5},
		{{"F3"}, sampyRate /5},
		{{"D3"}, sampyRate / 5},
		{{"C3"}, sampyRate},
		{{"G3"}, sampyRate / 2},
		{{"A3"}, sampyRate / 2},
		{{"D3"}, sampyRate / 2},
		{{"A#4"}, sampyRate / 5},
		{{"A4"}, sampyRate / 5},
		{{"G4"}, sampyRate / 5},
		{{"D#4"}, sampyRate},
		{{"A4"}, sampyRate / 5},
		{{"G4"}, sampyRate / 5},
		{{"F4"}, sampyRate / 5},
		{{"D4"}, sampyRate },
		{{"G4"}, sampyRate / 5},
		{{"F4"}, sampyRate / 5},
		{{"D#4"}, sampyRate / 5},
		{{"C4"}, sampyRate / 2},
		{{"G4"}, sampyRate},
		{{"A4"}, sampyRate},
		{{"D4"}, sampyRate},
		{{"G3"}, sampyRate / 6},
		{{"G3"}, sampyRate / 6},
		{{"A#3"}, sampyRate / 6},
		{{"F3"}, sampyRate / 6},
		{{"F3"}, sampyRate / 6},
		{{"A3"}, sampyRate / 6},
		{{"D#3"}, sampyRate / 6},
		{{"D#3"}, sampyRate / 6},
		{{"G3"}, sampyRate / 6},
		{{"D3"}, sampyRate / 6},
		{{"D3"}, sampyRate / 6},
		{{"F#3"}, sampyRate / 6},
		{{"G5"}, sampyRate},
		{{"G5"}, sampyRate},
		{{"G3"}, sampyRate},
		{{"G3"}, sampyRate},
		{{"G3"}, sampyRate},
		



	};
	int totalSamples = 0; //Starts at sample zero (AKA the first note)

	for (size_t i = 0; i < notes.size(); i++) {
		string note = notes[i].first;
		int noteDuration = notes[i].second; //Determines speed of the note being played
		for (int j = 0; j < sampyRate; j++) {
		int sampleIndex = totalSamples + j;
		if (sampleIndex < ac1.getSampleCount()) { //Everything here is just the Bridges tutorial code with minor changes
			double frequency = eMin[note]; //Grabs the frequency from the map
			double phase = 0;
			double amp = INT_MAX / 4.0;
			double time = (double)sampleIndex / sampyRate;
			double val = sin((2 * M_PI) * frequency * time + phase);
			int sampyVal = (int)(val * amp);
			ac1.setSample(0, sampleIndex, sampyVal);
			}
		}
		totalSamples += noteDuration; //Determines the length of the next sample
	}

	AudioClip mixed = AudioClip(numSampy + numFreqRaise, 1, 32, sampyRate);
	for (int i = 0; i < ac.getSampleCount(); i++) {
		mixed.setSample(0, i, ac.getSample(0, i));
	}

	for (int i = 0; i < ac1.getSampleCount(); i++) {
		mixed.setSample(0, i + ac.getSampleCount(), ac1.getSample(0, i));
	}

	bridges.setDataStructure(ac);
	bridges.visualize();

	
	bridges.setDataStructure(ac1);
	bridges.visualize();

	bridges.setDataStructure(mixed);
	bridges.visualize();
	


	return 0;
}


