#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include <string>
#include <vector>

class Actor;
class GhostRacer;
class ZombieCab;

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    virtual ~StudentWorld();
    
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    
    // Return a pointer to the world's GhostRacer.
    GhostRacer* getGhostRacer() const { return m_ghostRacer; }
    
    // Add an actor to the world.
    void addActor(Actor* a) { m_actors.push_back(a); }
    
    // Record that a soul was saved.
    void recordSoulSaved() { m_soulsToSave -= 1; }
    
    // Return true if actor a1 overlaps actor a2, otherwise false.
    bool overlaps(const Actor* a1, const Actor* a2) const;
    
    // If actor a overlaps a live actor that is affected by a holy water projectile, inflict a holy water spray on that actor and return true;
    // otherwise, return false. (See Actor::beSprayedIfAppropriate.)
    bool sprayFirstAppropriateActor(Actor* a) const;
    
    // Find the cloest "collision avoidance-worthy actor" to the zombie cab
    void findCloestAgentToZombieCab(ZombieCab* cab, double& disCloestFront, double& disCloestBehind, bool& cloestBehindIsGhostRacer) const;
    
private:
    // Add new actors to the game
    void addBorderLines();
    void addZombieCabs();
    void addOilSlicks();
    void addZombiePeds();
    void addHumanPeds();
    void addHolyWaterGoodies();
    void addSoulGoodies();
    // Generate game status text
    std::string generateGameStatText();
    
    GhostRacer* m_ghostRacer;
    std::vector<Actor*> m_actors;
    double m_yOfLastWhite;
    int m_soulsToSave;
    int m_bonus;
};

#endif // STUDENTWORLD_H_
