#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>
using namespace std;

string fizzbuzz(int x) {
	return "";
}

TEST(Fizzbuzz, GoodTests) {
	EXPECT_EQ(fizzbuzz(11), "11");
	EXPECT_EQ(fizzbuzz((1+rand() % 100)/15*15), "Fizzbuzz");
	EXPECT_EQ(fizzbuzz(3), "Fizz");
}

TEST(Fizzbuzz, BadTests) {
	EXPECT_EQ(fizzbuzz(-50), "Error");
}

TEST(Fizzbuzz, EdgeTests) {
	EXPECT_EQ(fizzbuzz(1),"1");
}

string fizzbuzz2(int x) {
	return "";
}

int main(int argc, char** argv) {
	testing::InitGoogleTest(&argc, argv);
	/srand(time(0));
#ifndef FOO
	//Stop people from compiling this code using 'compile'
	static_assert(false,"Type 'make' to build this, not 'compile'");
#endif
	/*
	string s;
	getline(cin,s);
	if (s == "test") return RUN_ALL_TESTS();
	*/
	}
}
