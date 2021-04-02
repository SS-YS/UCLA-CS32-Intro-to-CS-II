#ifndef ACTOR_H_
#define ACTOR_H_

#include "GameConstants.h"
#include "GraphObject.h"

class StudentWorld; 

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* studentWorld, int imageID, int startX, int startY, int startDirection, double size, int depth, int verSpeed, int horSpeed)
    :m_studentWorld(studentWorld), GraphObject(imageID, startX, startY, startDirection, size, depth), m_verSpeed(verSpeed), m_horSpeed(horSpeed), m_alive(true) {}
    StudentWorld* getStudentWorld() { return m_studentWorld; }
    int getVerSpeed() const { return m_verSpeed; }
    void setVerSpeed(int verSpeed) { m_verSpeed = verSpeed; }
    int getHorSpeed() const { return m_horSpeed; }
    bool isAlive() const { return m_alive; }
    void setToDead() { m_alive = false; }
    virtual void doSomething() = 0;
    
private:
    StudentWorld* m_studentWorld;
    int m_verSpeed;
    int m_horSpeed;
    bool m_alive;
};

class BorderLine: public Actor
{
public:
    BorderLine(StudentWorld* studentWorld, int imageID, int startX, int startY)
    :Actor(studentWorld, imageID, startX, startY, 0, 2, 2, -4, 0) {}
    virtual void doSomething();
};

class GhostRacer: public Actor
{
public:
    GhostRacer(StudentWorld* studentWorld)
    :Actor(studentWorld, IID_GHOST_RACER, 128, 32, 90, 4, 0, 0, 0), m_holyWater(10), m_health(100)  {}
    virtual void doSomething();
    
private:
    int m_holyWater;
    int m_health;
};

#endif // ACTOR_H_
