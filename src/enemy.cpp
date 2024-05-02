/**
 * @purpose Enemy class definitions and AI functionality
 * @author flakey5 (Aaron)
 */

#include "enemy.h"
#include <iostream>
#include <chrono>
std::vector<glm::vec2> Enemy::aStar(const std::vector<std::shared_ptr<PhysicsBody>>& space, const glm::vec2& pos, const glm::vec2& finalPos, const float& step, const glm::vec2& size){
  std::vector<glm::vec2> path;
  if (World::checkCollisions(space, finalPos, size)) {return {finalPos};}
  struct Node {
    glm::vec2 position;
    float gCost, hCost, fCost;
    std::shared_ptr<Node> parent = nullptr;
    std::string toString() const{
      return std::to_string(std::round(position.x * 10)) + " " + std::to_string(std::round(position.y * 10));
    }
  };
  std::shared_ptr<Node> start = std::shared_ptr<Node>(new Node(pos, 0, 0, 0));
  auto cmp = [](std::shared_ptr<Node> left, std::shared_ptr<Node> right){
    return std::tie(left->fCost, left->hCost, left->gCost) > std::tie(right->fCost, right->hCost, right->gCost);
  };
  std::priority_queue<std::shared_ptr<Node>, std::vector<std::shared_ptr<Node>>, decltype(cmp)> open;
  std::unordered_set<std::string> openSet;
  std::unordered_set<std::string> closed;

  open.push(start);
  openSet.insert(start->toString());
  std::shared_ptr<Node> lastNode;
  while (open.size()){
    std::shared_ptr<Node> current = open.top();
    open.pop();
    openSet.erase(current->toString());
    closed.insert(current->toString());

    if (glm::length(current->position - finalPos) <= step * 1.1) {lastNode = current;break;}

    static auto checkPos = [&start, &current, &open, &closed, &openSet, &step, &space, &finalPos, &size](std::shared_ptr<Node>& newNode)
    {
      if (World::checkCollisions(space, newNode->position, size) || closed.count(newNode->toString())) return;

      // erm what da sigma

      float cost = current->gCost + glm::length(newNode->position - current->position);
      if (!openSet.count(newNode->toString())){
        newNode->gCost = cost;
        newNode->hCost = glm::length(newNode->position - finalPos);
        newNode->fCost = newNode->gCost + newNode->hCost;
        newNode->parent = current;
        openSet.insert(newNode->toString());
        open.push(newNode);
      }
    };


    for (int i = -1; i <= 1; i++){
      for (int j = -1; j <= 1; j++){
        if (!i && !j) continue;
        std::shared_ptr<Node> temp = std::shared_ptr<Node>(new Node(current->position + glm::vec2(i * step, j * step), 0, 0, 0));
        checkPos(temp);
      }
    }
  }

  for (std::shared_ptr<Node> tmp = lastNode; tmp; tmp = tmp->parent){
    path.push_back(glm::vec2(tmp->position.x * -1, tmp->position.y * -1));
  }

  return path;
}

void Enemy::update(const std::shared_ptr<World> world){
  pathRefresh = 5000;
  static auto start = std::chrono::system_clock::now(); 
  static auto end = start;
  while(true){
    start = std::chrono::system_clock::now(); 
    delta = std::chrono::duration<float, std::milli>(start - end).count();
    end = start;

    static bool init = true;
    static float pathTimer;
    // point it is goind to
    glm::vec2 charaPos = world->players[0]->pos(); 
    glm::vec2 charaStart = world->players[0]->getRect().start();
    glm::vec2 charaEnd = world->players[0]->getRect().end();

    //float xDisp = charaPos.x - this->getPos().x;
    //if (xDisp != 0)
    //  xDisp /= abs(xDisp);

    //float yDisp = charaPos.y - this->getPos().y;
    //if (yDisp != 0)
    //  yDisp /= abs(yDisp);

    if (init) {
      pathTimer = pathRefresh;
    }

    pathTimer += delta;
    std::vector<glm::vec2> pathThing;
    if (pathTimer >= pathRefresh){
      {
      pathThing = aStar(world->staticBodies, this->getPos(), glm::vec2(charaStart.x, charaStart.y), 50, this->body->size());
      }
      if (pathThing.size() <= 1){
      pathThing = aStar(world->staticBodies, this->getPos(), glm::vec2(charaStart.x, charaEnd.y), 50, this->body->size());
      }
      if (pathThing.size() <= 1){
      pathThing = aStar(world->staticBodies, this->getPos(), glm::vec2(charaEnd.x, charaStart.y), 50, this->body->size());
      }
      if (pathThing.size() <= 1){
      pathThing = aStar(world->staticBodies, this->getPos(), glm::vec2(charaEnd.x, charaEnd.y), 50, this->body->size());
      }
      path = pathThing;
    }

    init = false;

    // TODO MOVE ON PATH
    // this code moves the goon
    if (pathThing.size() > 1){
      glm::vec2 bruh = pathThing[pathThing.size() - 1] - pathThing[pathThing.size() - 2]; 
      direction = glm::normalize(bruh);
      if (glm::length(this->getPos() - pathThing[pathThing.size() - 2]) < 1.00) path.pop_back();
    }


    *this += glm::vec2(direction.x / 3* delta, 0);
    if (glm::length(direction) && world->checkCollisions(world->staticBodies, *body)){
      *this += glm::vec2(-1 * direction.x / 3* delta, 0);
    }

    *this += glm::vec2(0, direction.y / 3* delta);
    if (glm::length(direction) && world->checkCollisions(world->staticBodies, *body)){
      *this += glm::vec2(0, -1 * direction.y / 3* delta);
    }
  }
}
///////////////////////////////////////////////////////
//                    M1ChipEnemy
///////////////////////////////////////////////////////


void M1ChipEnemy::update(float delta) {
  // TODO
}

///////////////////////////////////////////////////////
//                    M2ChipEnemy
///////////////////////////////////////////////////////


void M2ChipEnemy::update(float delta) {
  // TODO
}

///////////////////////////////////////////////////////
//                    M3ChipEnemy
///////////////////////////////////////////////////////


void M3ChipEnemy::update(float delta) {
  // TODO
}

///////////////////////////////////////////////////////
//                    M4ChipEnemy
///////////////////////////////////////////////////////


void M4ChipEnemy::update(float delta) {
  // TODO
}

///////////////////////////////////////////////////////
//                  EvilSurinEnemy
///////////////////////////////////////////////////////


void EvilSurinEnemy::update(float delta) {
  // TODO
}

///////////////////////////////////////////////////////
//                 EvilKerneyEnemy
///////////////////////////////////////////////////////


void EvilKerneyEnemy::update(float delta) {
  // TODO
}
