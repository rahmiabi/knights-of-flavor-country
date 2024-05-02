#include "Bridges.h"
#include "AudioClip.h"
#include "ColorGrid.h"
#include "GraphAdjList.h"
#include <iostream>
#include <fstream>

using namespace std;
using namespace bridges;

//KERNEY WITH THE DEMO CODE
AudioClip mixClips(const AudioClip& ac1, const AudioClip& ac2, float duration) {
	if (ac1.getSampleRate() != ac2.getSampleRate()) {
		throw "can't mix audio clips of varying rates";
	}

	int SAMPLE_RATE = ac1.getSampleRate();
	int sampleCount = SAMPLE_RATE * duration;

	AudioClip acMix = AudioClip(sampleCount, 1, ac1.getSampleBits(), SAMPLE_RATE);

	for (int i = 0; i < acMix.getSampleCount(); i++) {
		int sample1 = ac1.getSample(0, i);
		int sample2 = ac2.getSample(0, i);
		acMix.setSample(0, i, (sample1 + sample2) / 2);
	}

	return acMix;
}

AudioClip mixClips(const AudioClip& ac1, const AudioClip& ac2) {
	return mixClips(ac1, ac2, min((float)ac1.getSampleCount() / ac1.getSampleRate(), (float)ac2.getSampleCount()));
}

AudioClip mixFadeClips(const AudioClip& ac1, const AudioClip ac2, int fadeDuration, int duration) {
	if (ac1.getSampleRate() != ac2.getSampleRate()) throw "can't mix audio clips of varying rates";

	int SAMPLE_RATE = ac1.getSampleRate();
	int sampleCount = SAMPLE_RATE * duration;

	AudioClip acMix = AudioClip(sampleCount, 1, ac1.getSampleBits(), SAMPLE_RATE);

	int center = acMix.getSampleCount() / 2;
	int fadeSamples = SAMPLE_RATE * fadeDuration;
	fadeSamples = min(fadeSamples, acMix.getSampleCount());

	int fadeHalf1 = fadeSamples / 2;
	int fadeHalf2 = fadeHalf1 - (fadeSamples / 2);

	int start = center - fadeHalf1;
	int end = center + fadeHalf2;

	for (int i = 0; i < acMix.getSampleCount(); i++) {
		int sample1 = (int)(ac1.getSample(0, i) * (max(0, fadeSamples - (i - start)) / (float)fadeSamples));
		int sample2 = 0;

		if (i >= start) sample2 = (int)(ac2.getSample(0, i) * (min(fadeSamples, (i - start)) / (float)fadeSamples));

		acMix.setSample(0, i, (sample1 + sample2) / 2);
	}
	return acMix;
}
AudioClip mixFadeClips(const AudioClip& ac1, const AudioClip& ac2) {
	int duration = min(ac1.getSampleCount() / ac1.getSampleRate(), ac2.getSampleCount() / ac2.getSampleRate());

	int fadeDuration = duration / 2;

	return mixFadeClips(ac1, ac2, fadeDuration, duration);
}


int main() {
	cout << "WHY ARE YOU NOT WORKING?\n";
	Bridges bridges = Bridges(5, "gugu", "1058853664803");
	bridges.setTitle("Task Force OST");
	cout << "Oo oo aa aa\n";

	Color color(255, 255, 255);
	cout << "Colors\n";
	ColorGrid cg = ColorGrid(1080, 1920, color);
	cout << "Grid\n";

	ifstream ins("testaudio.wav"); //Okay, open it this way
	if (!ins) cout << "WHO WOULDVE THOUGHT?\n";
	else ins.close();

	/*
	ifstream ins2("./testaudio2.wav");
	if (!ins2) cout << "NOOOOOO WAY\n";
	else ins.close();
	*/

	AudioClip ac = AudioClip("testaudio.wav"); //I'm convinced it just takes forever? Is there a faster way to do this?
	cout << "Clip made\n";

	long long sampleCount = ac.getSampleCount();
	vector<int> sampy(sampleCount);
	for (int i = 0; i < ac.getSampleCount(); i++) {
		sampy.at(i) = ac.getSample(0, i);
	}

	long long sum = 0;
	for (int i = 0; i < sampleCount; i++) {
		sum += sampy[i];
	}

	double avg = static_cast<double>(sum) / sampleCount;
	for (int i = 0; i < cg.getWidth(); i++) {
		int height = (int)((avg + 1) / 2.0 * cg.getHeight());
		for (int j = 0; j < height; j++) {
			cg.set(i, j, Color(0, 0, 0));
		}
	}

	bridges.setDataStructure(&cg);
	bridges.visualize();
}
