#include "Actor.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include <cmath>

Actor::Actor(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int depth)
: m_sw(sw), GraphObject(imageID, x, y, dir, size, depth), m_isAlive(true)
{
}

BorderLine::BorderLine(StudentWorld* sw, int imageID, double x, double y)
: Actor(sw, imageID, x, y, 2, 0, 2)
{
    setVerticalSpeed(-4);
}

Agent::Agent(StudentWorld* sw, int imageID, double x, double y, double size, int dir, int hp)
: Actor(sw, imageID, x, y, size, dir, 0), m_hp(hp), m_horizSpeed(0)
{
}

GhostRacer::GhostRacer(StudentWorld* sw, double x, double y)
: Agent(sw, IID_GHOST_RACER, x, y, 4, 90, 100), m_numSprays(10)
{
    setVerticalSpeed(0);
}

Pedestrian::Pedestrian(StudentWorld* sw, int imageID, double x, double y, double size)
: Agent(sw, imageID, x, y, size, 0, 2), m_movePlanDis(0)
{
    setVerticalSpeed(-4);
}

HumanPedestrian::HumanPedestrian(StudentWorld* sw, double x, double y)
: Pedestrian(sw, IID_HUMAN_PED, x, y, 2)
{
}

ZombiePedestrian::ZombiePedestrian(StudentWorld* sw, double x, double y)
: Pedestrian(sw, IID_ZOMBIE_PED, x, y, 3), m_ticksUntilGrunt(0)
{
}

ZombieCab::ZombieCab(StudentWorld* sw, double x, double y)
: Agent(sw, IID_ZOMBIE_CAB, x, y, 4, 90, 3), m_movePlanDis(0), m_damagedGR(false)
{
}

Spray::Spray(StudentWorld* sw, double x, double y, int dir)
: Actor(sw, IID_HOLY_WATER_PROJECTILE, x, y, 1, dir, 1), m_travelDis(0)
{
}

GhostRacerActivatedObject::GhostRacerActivatedObject(StudentWorld* sw, int imageID, double x, double y, double size, int dir)
: Actor(sw, imageID, x, y, size, dir, 2)
{
    setVerticalSpeed(-4);
}

OilSlick::OilSlick(StudentWorld* sw, double x, double y)
: GhostRacerActivatedObject(sw, IID_OIL_SLICK, x, y, randInt(2, 5), 0)
{
}

HealingGoodie::HealingGoodie(StudentWorld* sw, double x, double y)
: GhostRacerActivatedObject(sw, IID_HEAL_GOODIE, x, y, 1, 0)
{
}

HolyWaterGoodie::HolyWaterGoodie(StudentWorld* sw, double x, double y)
: GhostRacerActivatedObject(sw, IID_HOLY_WATER_GOODIE, x, y, 2, 90)
{
}

SoulGoodie::SoulGoodie(StudentWorld* sw, double x, double y)
: GhostRacerActivatedObject(sw, IID_SOUL_GOODIE, x, y, 4, 0)
{
}

bool Actor::moveRelativeToGhostRacerVerticalSpeed(double dx)
{
    double vert_speed = getVerticalSpeed() - world()->getGhostRacer()->getVerticalSpeed();
    double new_x = getX() + dx;
    double new_y = getY() + vert_speed;
    moveTo(new_x, new_y);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDead();
        return false;
    }
    return true;
}

void BorderLine::doSomething()
{
    moveRelativeToGhostRacerVerticalSpeed(0);
}

void Agent::takeDamageAndPossiblyDie(int hp)
{
    increaseHP(-hp);
    if (m_hp <= 0)
    {
        setDead();
        world()->playSound(soundWhenDie());
    }
    else
        world()->playSound(soundWhenHurt());
}

void GhostRacer::doSomething()
{
    if (isDead())
        return;
    
    // Check if Ghost Racer is swerving off the road
    if (getX() <= LEFT_EDGE)
    {
        if (getDirection() > 90)
            takeDamageAndPossiblyDie(10);
        setDirection(82);
        world()->playSound(SOUND_VEHICLE_CRASH);
    }
    else if (getX() >= RIGHT_EDGE)
    {
        if (getDirection() < 90)
            takeDamageAndPossiblyDie(10);
        setDirection(98);
        world()->playSound(SOUND_VEHICLE_CRASH);
    }
    
    // Check if user pressed a key
    else
    {
        int ch;
        if (world()->getKey(ch))
        {
            // User hit a key during this tick
            switch (ch) {
                case KEY_PRESS_SPACE:
                    if (getNumSprays() > 0)
                    {
                        double newX, newY;
                        getPositionInThisDirection(getDirection(), SPRITE_HEIGHT, newX, newY);
                        world()->addActor(new Spray(world(), newX, newY, getDirection()));
                        world()->playSound(SOUND_PLAYER_SPRAY);
                        m_numSprays -= 1;
                    }
                    break; 
                case KEY_PRESS_LEFT:
                    if (getDirection() < 114)
                        setDirection(getDirection() + 8);
                    break;
                case KEY_PRESS_RIGHT:
                    if (getDirection() > 66)
                        setDirection(getDirection() - 8);
                    break;
                case KEY_PRESS_UP:
                    if (getVerticalSpeed() < 5)
                        setVerticalSpeed(getVerticalSpeed() + 1);
                    break;
                case KEY_PRESS_DOWN:
                    if (getVerticalSpeed() > -1)
                        setVerticalSpeed(getVerticalSpeed() - 1);
                    break;
            }
        }
    }
    
    // Move
    double max_shift_per_tick = 4.0;
    double dirInRad = getDirection() * M_PI / 180;
    double delta_x = cos(dirInRad) * max_shift_per_tick;
    double cur_x = getX();
    double cur_y = getY();
    moveTo(cur_x + delta_x, cur_y);
}

void GhostRacer::spin()
{
    int new_dir;
    while (true)
    {
        int delta_dir = randInt(5, 20);
        if (randInt(0, 1) == 0)
            new_dir = getDirection() + delta_dir;
        else
            new_dir = getDirection() - delta_dir;
        if (60 <= new_dir && new_dir <= 120)
            break;
    }
    setDirection(new_dir);
}

void Pedestrian::moveAndPossiblyPickPlan()
{
    m_movePlanDis -= 1;
    if (m_movePlanDis > 0)
        return;
    
    // Pick a new movement plan
    while (true)
    {
        setHorizSpeed(randInt(-3, 3));
        if (getHorizSpeed() != 0)
            break;
    }
    m_movePlanDis = randInt(4, 32);
    if (getHorizSpeed() < 0)
        setDirection(180);
    else
        setDirection(0);
}

void HumanPedestrian::doSomething()
{
    if (isDead())
        return;
    
    if (world()->overlaps(this, world()->getGhostRacer()))
    {
        world()->getGhostRacer()->setDead();
        return;
    }
    
    if (!moveRelativeToGhostRacerVerticalSpeed(getHorizSpeed()))
        return;
    
    moveAndPossiblyPickPlan();
}

bool HumanPedestrian::beSprayedIfAppropriate()
{
    setHorizSpeed(-1 * getHorizSpeed());
    setDirection(getDirection() + 180);
    world()->playSound(soundWhenHurt()); 
    return true;
}

void ZombiePedestrian::doSomething()
{
    if (isDead())
        return;
    
    if (world()->overlaps(this, world()->getGhostRacer()))
    {
        world()->getGhostRacer()->takeDamageAndPossiblyDie(5);
        takeDamageAndPossiblyDie(2);
        return;
    }
    
    double xDiff = getX() - world()->getGhostRacer()->getX();
    if (xDiff < 0)
        xDiff = -xDiff;
    double yDiff = getY() - world()->getGhostRacer()->getY();
    if (xDiff <= 30 && yDiff > 0)
    {
        setDirection(270);
        if (getX() < world()->getGhostRacer()->getX())
            setHorizSpeed(1);
        else if (getX() > world()->getGhostRacer()->getX())
            setHorizSpeed(-1);
        else
            setHorizSpeed(0);
        
        m_ticksUntilGrunt -= 1;
        if (m_ticksUntilGrunt <= 0)
        {
            world()->playSound(SOUND_ZOMBIE_ATTACK);
            m_ticksUntilGrunt = 20;
        }
    }
    
    if (!moveRelativeToGhostRacerVerticalSpeed(getHorizSpeed()))
        return;
    
    moveAndPossiblyPickPlan();
}

void ZombiePedestrian::takeDamageAndPossiblyDie(int hp)
{
    Agent::takeDamageAndPossiblyDie(hp);
    if (isDead())
    {
        if (!world()->overlaps(this, world()->getGhostRacer()))
        {
            if (randInt(1, 5) == 1)
                world()->addActor(new HealingGoodie(world(), getX(), getY()));
        }
        world()->increaseScore(150);
    }
}

bool ZombiePedestrian::beSprayedIfAppropriate()
{
    takeDamageAndPossiblyDie(1);
    return true;
}

void ZombieCab::doSomething()
{
    if (isDead())
        return;

    // Collide with Ghost Racer
    if (world()->overlaps(this, world()->getGhostRacer()) && !m_damagedGR)
    {
        world()->playSound(SOUND_VEHICLE_CRASH);
        world()->getGhostRacer()->takeDamageAndPossiblyDie(20);
        if (getX() <= world()->getGhostRacer()->getX())
        {
            setHorizSpeed(-5);
            setDirection(120 + randInt(0, 19));
        }
        else
        {
            setHorizSpeed(5);
            setDirection(60 - randInt(0, 19));
        }
        m_damagedGR = true;
    }
    
    if (!moveRelativeToGhostRacerVerticalSpeed(getHorizSpeed()))
        return;
    
    double disCloestFront = 999;
    double disCloestBehind = 999;
    bool cloestBehindIsGhostRacer = false;
    world()->findCloestAgentToZombieCab(this, disCloestFront, disCloestBehind, cloestBehindIsGhostRacer);
    if (getVerticalSpeed() > world()->getGhostRacer()->getVerticalSpeed() && disCloestFront != 999)
    {
        if (disCloestFront < 96)
        {
            setVerticalSpeed(getVerticalSpeed() - 0.5);
            return;
        }
    }
    if (getVerticalSpeed() <= world()->getGhostRacer()->getVerticalSpeed() && disCloestBehind != 999)
    {
        if (disCloestBehind < 96 && !cloestBehindIsGhostRacer)
        {
            setVerticalSpeed(getVerticalSpeed() + 5);
            return;
        }
    }
    
    m_movePlanDis -= 1;
    if (m_movePlanDis > 0)
        return;
    m_movePlanDis = randInt(4, 32);
    setVerticalSpeed(getVerticalSpeed() + randInt(-2, 2));
}

bool ZombieCab::beSprayedIfAppropriate()
{
    takeDamageAndPossiblyDie(1);
    return true;
}

void ZombieCab::takeDamageAndPossiblyDie(int hp)
{
    Agent::takeDamageAndPossiblyDie(hp);
    if (isDead())
    {
        if (randInt(1, 5) == 1)
            world()->addActor(new OilSlick(world(), getX(), getY()));
        world()->increaseScore(200);
    }
}

void Spray::doSomething()
{
    if (isDead())
        return;
    
    if (world()->sprayFirstAppropriateActor(this))
    {
        setDead();
        return; 
    }
    
    moveForward(SPRITE_HEIGHT);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
    {
        setDead();
        return;
    }
    
    m_travelDis += SPRITE_HEIGHT;
    if (m_travelDis >= 160)
        setDead();
}

void GhostRacerActivatedObject::doSomething()
{
    if (!moveRelativeToGhostRacerVerticalSpeed(0))
        return;
    
    if (world()->overlaps(this, world()->getGhostRacer()))
    {
        doActivity();
        world()->playSound(getSound());
        if (selfDestructs())
            setDead();
        world()->increaseScore(getScoreIncrease());
    }
}

bool GhostRacerActivatedObject::beSprayedIfAppropriate()
{
    if (!isSprayable())
        return false;
    setDead();
    return true;
}

void OilSlick::doActivity() const
{
    world()->getGhostRacer()->spin(); 
}

void HealingGoodie::doActivity() const
{
    world()->getGhostRacer()->increaseHP(10);
}

void HolyWaterGoodie::doActivity() const
{
    world()->getGhostRacer()->increaseSprays(10);
}

void SoulGoodie::doSomething()
{
    GhostRacerActivatedObject::doSomething();
    setDirection(getDirection() - 10);
}

void SoulGoodie::doActivity() const
{
    world()->recordSoulSaved();
}
