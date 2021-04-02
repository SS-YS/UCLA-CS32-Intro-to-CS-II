#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <vector>

class GhostRacer;
class Actor;

class StudentWorld: public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    virtual ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    GhostRacer* getGhostRacer() { return m_ghostRacer; }

private:
    GhostRacer* m_ghostRacer = nullptr;
    std::vector<Actor*> m_actors;
    int m_yOfLastWhite; 
};

#endif // STUDENTWORLD_H_
