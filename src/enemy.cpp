/**
 * @purpose Enemy class definitions and AI functionality
 * @author flakey5 (Aaron)
 */

#include "enemy.h"

std::deque<glm::vec2> aStar(const std::vector<std::shared_ptr<PhysicsBody>>& space, const glm::vec2& pos, const glm::vec2& finalPos, const float& step, const float& maxSize){
    std::deque<glm::vec2> path;
    if (World::checkCollisions(space, finalPos, glm::vec2(step / 2, step / 2))) return{};
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

        static auto checkPos = [&start, &current, &open, &closed, &openSet, &step, &space, &finalPos](std::shared_ptr<Node>& newNode)
        {
            if (World::checkCollisions((space), newNode->position, glm::vec2{0,0}) || closed.count(newNode->toString())) return;

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
        path.push_front(tmp->position);
    }
    return path;
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
