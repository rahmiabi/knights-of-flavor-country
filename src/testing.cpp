//BENSON MADE THESE TESTS 🎉🥳

#include <bits/chrono.h>
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>
//#include "body.h"
#include "csv_file.h"
#include "inventory.h"
#include "location.h"
//#include "npc.h"
//#include "player.h"
#include "weapon.h"
#include "attack_traits.h"
#include "client.h"
//#include "enemy.h"
#include "item.h"
//#include "projectile.h"
#include "item_registry.h"
//#include "vecmath.h"
using namespace std;

TEST(attack_traits, AttackTests) {
	AttackTraits t1;
	t1.setSpeed(10);
	t1.setDefense(20);
	t1.setHp(90);
	EXPECT_EQ(t1.getSpeed(), 10);
	EXPECT_EQ(t1.getDefense(), 20);
	EXPECT_EQ(t1.getHp(), 90);
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	string s;
	getline(cin,s);
	if (s == "test") {
		auto start = chrono::high_resolution_clock::now();
		return RUN_ALL_TESTS();
		auto stop = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
		cout << duration.count() << endl;
	}
}

//NEED TO:
//include glm?
