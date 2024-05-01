#include "../lib/read.h"
#include <thread>
#include <iostream>
#include <vector>
using namespace std;

/*
Need functions for:
Enemies, thier AI will update on it's own thread
Players, they will have their own thread
Physics of projectiles, this one will probably have 2 threads, whenever there are more than 30 projectiles it'll put the rest on the second thread
Input Thread
Output Thread
*/

void enemy_threading() {
}

void player_threading() { 
}

void projectile_threading() {
}

void input_thread() {
}

void output_thread() {
}

int main() {
	vector<jthread> vec;
	// Some code doing actual threading

	// Joining all threads
	for (int i = 0; i < vec.size(); i++) {vec.at(i).join();}


}
