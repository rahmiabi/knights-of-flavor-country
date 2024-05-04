#include "../lib/read.h"
#include <thread>
#include <iostream>
#include <vector>
#include <unordered_map>
using namespace std;

// TODO: Thread manager class

class ThreadManager {
public:
	unordered_map<string, jthread> map;

	void new_thread(string s, auto function) {
		map.insert({s, function});
	}

	void make_detatched(string s) {
		map.find(s)->second.detach();
	}

	void make_joined(string s) {
		map.find(s)->second.join();
	}

};
