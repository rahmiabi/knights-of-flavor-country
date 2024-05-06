//BENSON MADE THESE TESTS 🎉🥳
//I also made the logos and did the textures for map 1
//NOTE: some files have glm error or other #include errors since aaron and julian were on vs code
#include <bits/chrono.h>
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>
//#include "body.h"
#include "csv_file.h"
#include "actor.h"
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

TEST(actor, ActorTests) {
	std::shared_ptr<PhysicsBody> body = std::make_shared<PhysicsBody>();
	Actor a("bobby lee", body);
	glm::vec2 expected(10.0f, 20.0f); //random coordinates to check
	const PhysicsBody &bodyRef = a.getRect(); //to test if the body get works
	EXPECT_EQ(a.getName(), "bobby lee");
	EXPECT_EQ(a.getBody(), body);
	EXPECT_EQ(a.size(), expected);
	EXPECT_EQ(&bodyRef, body.get());
	EXPECT_EQ(a.getPos(), expected);
}

TEST(csv, CSVTests) {
	CsvIndexMap indexMap = {{"Column1", 0}, {"Column2", 1}, {"Column3", 2}}; //makes a csv kinda
	vector<string> tokens = {"v1", "v2", "v3"}; //value for each spot
	CsvEntry entry(std::move(tokens), indexMap); //sets each value
	EXPECT_TRUE(entry.hasColumn("Column1")); //checks if the column exists
	EXPECT_FALSE(entry.hasColumn("Column2345")); //checks if the column doesnt exist
	EXPECT_EQ(entry.getValueAtColumn("Column1"), "v1"); //checks column 1's value
}

class Bo : public Item {
	public:
	Bo(const string &name, const string &description) : Item(name, description) {}
};

TEST(item, ItemTests) {
	Bo cool("bo staff", "big stick");
	EXPECT_EQ(cool.getName(), "bo staff");
	EXPECT_EQ(cool.getDescription(), "big stick");
	EXPECT_EQ(cool.getMaxAmount(), 256);
}

TEST(inventory, InventoryTests) {
	Inventory inven;
	inven.addAbility("keyboard slam");
	inven.addAbility("hey kerney");
	inven.removeAbility("hey kerney");
	inven.addWeapon("fart gun");
	Weapon sword;
	inven.weapons["sword"] = sword;
	Weapon &temp = inven.getWeapon("sword");
	Ability &abil = inven.getAbility("keyboard slam");
	EXPECT_TRUE(inven.hasAbility("keyboard slam"));
	EXPECT_FALSE(inven.hasAbility("knaw"));
	EXPECT_EQ(abil.getName(), "keyboard slam");
	EXPECT_FALSE(inven.hasAbility("hey kerney"));
	EXPECT_TRUE(inven.hasWeapon("fart gun"));
	EXPECT_EQ(temp.getName(), "sword");
}

TEST(npc, NPCTests) {
	std::shared_ptr<PhysicsBody> body = make_shared<PhysicsBody>();
	Npc menc("irl npc", body);
	EXPECT_EQ(npc.getName(), "irl npc");
	EXPECT_EQ(t1.getBody(), body);
	EXPECT_FALSE(npc.isSpeaking());	
}

TEST(player, PlayerTests) {
	glm::vec2 pos(0,0);
	glm::vec2 size(1, 1);
	auto texture = static_cast<uint>(123);
	Player finn(pos, size, texture);
	finn.weaponNames["jake"] = 0;
	EXPECT_EQ(finn.getScale(), 0.5);
	EXPECT_EQ(finn.getTexture(), texture);	
	EXPECT_EQ(finn.getWeaponInd(0), "jake");
	EXPECT_EQ(finn.getWeaponInd(1), "");
	//since king julian asked: JSON tests
	rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> aloe;
	rapidjson::Value obj = finn.toJSONObject(aloe);
	rapidjson::Document doc;
	doc.SetObject();
	rapidjson::Document::AllocatorType &aloe = doc.GetAllocator();
	doc.AddMember("scale", 0.75, aloe);
	doc.AddMember("texture", 456, aloe);
	finn.fromJSON(doc);
	EXPECT_EQ(finn.getScale(), 0.75);
	EXPECT_EQ(finn.getTexture(), 456);
	EXPECT_TRUE(obj.HasMember("scale"));
	EXPECT_TRUE(obj.HasMember("texture"));

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
