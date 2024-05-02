/**
 * @purpose Enemy class definitions and AI functionality
 * @author flakey5 (Aaron)
 */

#include "enemy.h"
#include <iostream>
#include <chrono>
std::deque<glm::vec2> Enemy::aStar(const std::vector<std::shared_ptr<PhysicsBody>>& space, const glm::vec2& pos, const glm::vec2& finalPos, const float& step, const glm::vec2& size){
    std::deque<glm::vec2> path;
    if (World::checkCollisions(space, finalPos, size)) {path.push_back(finalPos); return path;}
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
        //std::cout << tmp->position.x << " " << tmp->position.y << std::endl;
        path.push_front(glm::vec2(tmp->position.x, tmp->position.y));
    }
    //std::cout << std::endl;
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
    static glm::vec2 direction;
    static glm::vec2 pos1;
    static glm::vec2 pos2; 
    glm::vec2 charaPos = world->players[0]->pos(); 
    if (init) pathTimer = pathRefresh;
    pathTimer += delta;
    if (pathTimer >= pathRefresh){
        std::cout << pathTimer << std::endl;
        std::cout << pathRefresh << std::endl;
        path = aStar(world->staticBodies, this->getPos(), charaPos, this->body->size().x, this->body->size());
        // update point
        glm::vec2 pos1 = path.front();
        path.pop_front();
        glm::vec2 pos2 = path.front();
        path.pop_front();
        glm::vec2 dir = pos2 - pos1;
        if (dir.x || dir.y)
            direction = glm::normalize(dir);
        else
            direction = glm::vec2(0,0);
        pathTimer = 0;
    }
        std::cout << path.size() << std::endl;
        std::cout << this->getPos().x << " " << this->getPos().y << std::endl;
    
    // if next point is closer to the going towards, then go to the next point
    if (path.size() == 0){
        direction = glm::vec2(0,0);
    } else if (glm::length(pos2 - this->getPos()) < 10) {
        glm::vec2 pos1 = pos2;
        glm::vec2 pos2 = path.front();
        path.pop_front();
        glm::vec2 dir = pos2 - pos1;
        if (dir.x || dir.y)
            direction = glm::normalize(dir);
        else
            direction = glm::vec2(0,0);
    }
    //std::cout << direction.x << " " << direction.y << std::endl;
    init = false;


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
