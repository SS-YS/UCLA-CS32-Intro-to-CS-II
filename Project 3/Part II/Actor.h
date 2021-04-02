#ifndef ACTOR_H_
#define ACTOR_H_

#include "GameConstants.h"
#include "GraphObject.h"

const int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
const int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
const int LEFT_MIDDLE = LEFT_EDGE + ROAD_WIDTH/3;
const int MIDDLE_RIGHT = RIGHT_EDGE - ROAD_WIDTH/3; 

class StudentWorld; 

/*
 GraphObject
     Actor
         BorderLine
         Agent
             GhostRacer
             Pedestrian
                 HumanPedestrian
                 ZombiePedestrian
             ZombieCab
         Spray
         GhostRacerActivatedObject
             OilSlick
             HealingGoodie
             HolyWaterGoodie
             SoulGoodie
*/

class Actor : public GraphObject
{
public:
    Actor(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int depth);

    // Action to perform for each tick.
    virtual void doSomething() = 0;
    
    // Is this actor dead?
    bool isDead() const { return !m_isAlive; }
      
    // Mark this actor as dead.
    void setDead() { m_isAlive = false; }
      
    // Get this actor's world
    StudentWorld* world() const { return m_sw; }
      
    // Get this actor's vertical speed.
    double getVerticalSpeed() const { return m_vertiSpeed; }
      
    // Set this actor's vertical speed.
    void setVerticalSpeed(double speed) { m_vertiSpeed = speed; }
    
    // Adjust the x coordinate by dx to move to a position with a y coordinate
    // determined by this actor's vertical speed relative to GhostRacer's
    // vertical speed.  Return true if the new position is within the view;
    // otherwise, return false, with the actor dead.
    bool moveRelativeToGhostRacerVerticalSpeed(double dx);
    
    // If this actor is affected by holy water projectiles, then inflict that
    // effect on it and return true; otherwise, return false.
    virtual bool beSprayedIfAppropriate() { return false; }
    
    // Does this object affect zombie cab placement and speed?
    virtual bool isCollisionAvoidanceWorthy() const { return false; }

private:
    StudentWorld* m_sw;
    bool m_isAlive;
    double m_vertiSpeed;
};

class BorderLine : public Actor
{
public:
    BorderLine(StudentWorld* sw, int imageID, double x, double y);
    virtual void doSomething();
};

class Agent : public Actor
{
public:
    Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp);
    
    virtual bool isCollisionAvoidanceWorthy() const { return true; }
      
    // Get hit points.
    int getHP() const { return m_hp; }
        
    // Increase hit points by hp.
    void increaseHP(int hp)
    {
        m_hp += hp;
        if (m_hp > 100)
            m_hp = 100;
    }
    
    // Get the agent's horizontal speed
    int getHorizSpeed() const { return m_horizSpeed; }
    
    // Set the agent's horizontal speed
    void setHorizSpeed(int s) { m_horizSpeed = s; }
    
    // Do what the spec says happens when hp units of damage is inflicted.
    virtual void takeDamageAndPossiblyDie(int hp);
    
    // What sound should play when this agent is damaged but does not die?
    virtual int soundWhenHurt() const { return SOUND_NONE; }
    
    // What sound should play when this agent is damaged and dies?
    virtual int soundWhenDie() const { return SOUND_NONE; }
    
private:
    int m_hp;
    int m_horizSpeed;
};

class GhostRacer : public Agent
{
public:
    GhostRacer(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    
    virtual int soundWhenDie() const { return SOUND_PLAYER_DIE; }
    
    // How many holy water projectiles does the object have?
    int getNumSprays() const { return m_numSprays; }
    
    // Increase the number of holy water projectiles the object has.
    void increaseSprays(int amt) { m_numSprays += amt; }
    
    // Spin as a result of hitting an oil slick.
    void spin();
    
private:
    int m_numSprays;
};

class Pedestrian : public Agent
{
public:
    Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size);
    
    virtual int soundWhenHurt() const { return SOUND_PED_HURT; }
          
    // Move the pedestrian. If the pedestrian doesn't go off screen and
    // should pick a new movement plan, pick a new plan.
    void moveAndPossiblyPickPlan();
    
private:
    int m_movePlanDis;
};

class HumanPedestrian : public Pedestrian
{
public:
    HumanPedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
};

class ZombiePedestrian : public Pedestrian
{
public:
    ZombiePedestrian(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void takeDamageAndPossiblyDie(int hp);
    virtual bool beSprayedIfAppropriate();
    
    virtual int soundWhenDie() const { return SOUND_PED_DIE; }
    
private:
    int m_ticksUntilGrunt;
};

class ZombieCab : public Agent
{
public:
    ZombieCab(StudentWorld* sw, double x, double y);
    virtual void doSomething();
    virtual void takeDamageAndPossiblyDie(int hp);
    virtual bool beSprayedIfAppropriate();
    
    virtual int soundWhenHurt() const { return SOUND_VEHICLE_HURT; }
    
    virtual int soundWhenDie() const { return SOUND_VEHICLE_DIE; }
    
private:
    int m_movePlanDis; 
    bool m_damagedGR;
};

class Spray : public Actor
{
public:
    Spray(StudentWorld* sw, double x, double y, int dir);
    virtual void doSomething();
    
private:
    int m_travelDis;
};

class GhostRacerActivatedObject : public Actor
{
public:
    GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir);
    virtual void doSomething();
    virtual bool beSprayedIfAppropriate();
      
    // Do the object's special activity (increase health, spin Ghostracer, etc.)
    virtual void doActivity() const = 0;
    
    // Return the object's increase to the score when activated.
    virtual int getScoreIncrease() const = 0;
    
    // Return the sound to be played when the object is activated.
    virtual int getSound() const { return SOUND_GOT_GOODIE; }
    
    // Return whether the object dies after activation.
    virtual bool selfDestructs() const { return true; }
    
    // Return whether the object is affected by a holy water projectile.
    virtual bool isSprayable() const { return true; };
};

class OilSlick : public GhostRacerActivatedObject
{
public:
    OilSlick(StudentWorld* sw, double x, double y);
    virtual void doActivity() const;
    
    virtual int getScoreIncrease() const { return 0; }
    
    virtual int getSound() const { return SOUND_OIL_SLICK; }
    
    virtual bool selfDestructs() const { return false; }
    
    virtual bool isSprayable() const { return false; }
};

class HealingGoodie : public GhostRacerActivatedObject
{
public:
    HealingGoodie(StudentWorld* sw, double x, double y);
    virtual void doActivity() const;
        
    virtual int getScoreIncrease() const { return 250; }
};

class HolyWaterGoodie : public GhostRacerActivatedObject
{
public:
    HolyWaterGoodie(StudentWorld* sw, double x, double y);
    virtual void doActivity() const;
    
    virtual int getScoreIncrease() const { return 50; }
};

class SoulGoodie : public GhostRacerActivatedObject
{
public:
    SoulGoodie(StudentWorld* sw, double x, double y);
    virtual void doSomething(); 
    virtual void doActivity() const;

    virtual int getScoreIncrease() const { return 100; }

    virtual int getSound() const { return SOUND_GOT_SOUL; }

    virtual bool isSprayable() const { return false; }
};

#endif // ACTOR_H_
