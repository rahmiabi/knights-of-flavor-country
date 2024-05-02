#include "Bridges.h"
#include "AudioClip.h"
#include "iostream"

using namespace std;
using namespace bridges;

int main(int argc, char **argv) {
	Bridges bridges(5, "gugu", "1058853664803");
	bridges.setTitle("Testing Audio");
	bridges.setDescription("Blah blah blah");

	AudioClip ooga = AudioClip("./testaudio.wav");

	bridges.setDataStructure(ooga);
	bridges.visualize();

	return 0;
}
