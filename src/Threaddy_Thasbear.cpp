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

};
