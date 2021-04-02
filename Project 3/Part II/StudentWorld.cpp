#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), m_ghostRacer(nullptr)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    m_soulsToSave = getLevel() * 2 + 5;
    m_bonus = 5000;
    
    // Create a Ghost Racer
    m_ghostRacer = new GhostRacer(this, 128, 32);
    addActor(m_ghostRacer);
    
    // Create yellow border lines
    for (int j = 0; j < VIEW_HEIGHT / SPRITE_HEIGHT; j++)
    {
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, j * SPRITE_HEIGHT));
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, j * SPRITE_HEIGHT));
    }
    
    // Create white border lines
    for (int j = 0; j < VIEW_HEIGHT / (4*SPRITE_HEIGHT); j++)
    {
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_MIDDLE, j * 4 * SPRITE_HEIGHT));
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, MIDDLE_RIGHT, j * 4 * SPRITE_HEIGHT));
        m_yOfLastWhite = j * 4 * SPRITE_HEIGHT;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Ask each actor to do something
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i] == nullptr)
            continue;
        if (!m_actors[i]->isDead())
        {
            m_actors[i]->doSomething();
            if (m_ghostRacer->isDead())
            {
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            if (m_soulsToSave <= 0)
            {
                increaseScore(m_bonus);
                return GWSTATUS_FINISHED_LEVEL;
            }
        }
    }
    
    // Delete newly-dead actors
    for (int i = 0; i < m_actors.size(); i++)
    {
        if (m_actors[i] == nullptr)
            continue;
        if (m_actors[i]->isDead())
        {
            delete m_actors[i];
            m_actors[i] = nullptr;
        }
    }
    
    // Add new actors
    addBorderLines();
    addZombieCabs();
    addOilSlicks();
    addZombiePeds();
    addHumanPeds();
    addHolyWaterGoodies();
    addSoulGoodies();
    
    // Update the status text
    m_bonus -= 1;
    if (m_bonus < 0)
        m_bonus = 0; 
    setGameStatText(generateGameStatText());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    while (!m_actors.empty())
    {
        delete m_actors[m_actors.size()-1];
        m_actors.pop_back();
    }
}

bool StudentWorld::overlaps(const Actor *a1, const Actor *a2) const
{
    double delta_x = a1->getX() - a2->getX();
    if (delta_x < 0)
        delta_x = -delta_x;

    double delta_y = a1->getY() - a2->getY();
    if (delta_y < 0)
        delta_y = -delta_y;
    
    double radius_sum = a1->getRadius() + a2->getRadius();
    
    return delta_x < radius_sum * 0.25 && delta_y < radius_sum * 0.6;
}

bool StudentWorld::sprayFirstAppropriateActor(Actor *a) const
{
    for (int i = 0; i < m_actors.size(); i++)
    {
        Actor* curActor = m_actors[i];
        if (curActor == nullptr || curActor->isDead())
            continue;
        
        if (overlaps(a, curActor) && curActor->beSprayedIfAppropriate())
            return true;
    }
    return false;
}

void StudentWorld::findCloestAgentToZombieCab(ZombieCab *cab, double &disCloestFront, double &disCloestBehind, bool& cloestBehindIsGhostRacer) const
{
    int leftBound, rightBound;
    if (cab->getX() == ROAD_CENTER - ROAD_WIDTH/3) // the left lane
    {
        leftBound = LEFT_EDGE;
        rightBound = LEFT_MIDDLE;
    }
    else if (cab->getX() == ROAD_CENTER) // the middle lane
    {
        leftBound = LEFT_MIDDLE;
        rightBound = MIDDLE_RIGHT;
    }
    else // the right lane
    {
        leftBound = MIDDLE_RIGHT;
        rightBound = RIGHT_EDGE;
    }

    for (int i = 0; i < m_actors.size(); i++)
    {
        Actor* curActor = m_actors[i];
        if (curActor == nullptr || curActor->isDead() || !curActor->isCollisionAvoidanceWorthy() || curActor == cab)
            continue;
        
        double curX = curActor->getX();
        double curY = curActor->getY();
        if (leftBound <= curX && curX < rightBound)
        {
            double dis = curY - cab->getY();
            if (0 < dis && dis < disCloestFront)
                disCloestFront = dis;
            if (dis < 0 && -dis < disCloestBehind)
            {
                disCloestBehind = -dis;
                if (curActor == m_ghostRacer)
                    cloestBehindIsGhostRacer = true;
                else
                    cloestBehindIsGhostRacer = false;
            }
        }
    }
}

void StudentWorld::addBorderLines()
{
    double new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    m_yOfLastWhite += -4 - m_ghostRacer->getVerticalSpeed();
    double delta_y = new_border_y - m_yOfLastWhite;
    if (delta_y >= SPRITE_HEIGHT)
    {
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, new_border_y));
        addActor(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, new_border_y));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT)
    {
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_MIDDLE, new_border_y));
        addActor(new BorderLine(this, IID_WHITE_BORDER_LINE, MIDDLE_RIGHT, new_border_y));
        m_yOfLastWhite = new_border_y;
    }
}

void StudentWorld::addZombieCabs()
{
    int chanceVehicle = 100 - getLevel()*10;
    if (chanceVehicle < 20)
        chanceVehicle = 20;
    if (randInt(0, chanceVehicle-1) == 0)
    {
        // Evaluate the three lanes in a random order
        int LEFT_LANE = randInt(0, 2);
        int MIDDLE_LANE;
        while (true)
        {
            MIDDLE_LANE = randInt(0, 2);
            if (MIDDLE_LANE != LEFT_LANE)
                break;
        }
        
        for (int i = 0; i < 3; i++)
        {
            int leftBound, rightBound;
            if (i == LEFT_LANE)
            {
                leftBound = LEFT_EDGE;
                rightBound = LEFT_MIDDLE;
            }
            else if (i == MIDDLE_LANE)
            {
                leftBound = LEFT_MIDDLE;
                rightBound = MIDDLE_RIGHT;
            }
            else // the right lane
            {
                leftBound = MIDDLE_RIGHT;
                rightBound = RIGHT_EDGE;
            }
            
            // Find the closet "collision avoidance-worthy actor" to the bottom/top of the screen in the candidate lane
            double yOfCloestBottom = 999;
            double yOfCloestTop = -999;
            for (int j = 0; j < m_actors.size(); j++)
            {
                Actor* curActor = m_actors[j];
                
                if (curActor == nullptr || !curActor->isCollisionAvoidanceWorthy())
                    continue;
                
                double curX = curActor->getX();
                double curY = curActor->getY();
                if (leftBound <= curX && curX < rightBound)
                {
                    if (curY < yOfCloestBottom)
                        yOfCloestBottom = curY;
                    if (curY > yOfCloestTop)
                        yOfCloestTop = curY;
                }
            }
            
            // Check if a new zombie cab should be added to the current lane
            bool shouldBeAdded = false;
            double startY = NULL;
            double vertiSpeed = NULL;
            if (yOfCloestBottom == 999 || yOfCloestBottom > VIEW_HEIGHT/3)
            {
                startY = SPRITE_HEIGHT / 2;
                vertiSpeed = m_ghostRacer->getVerticalSpeed() + randInt(2, 4);
                shouldBeAdded = true;
            }
            else if (yOfCloestTop == -999 || yOfCloestTop < VIEW_HEIGHT * 2 / 3)
            {
                startY = VIEW_HEIGHT - SPRITE_HEIGHT/2;
                vertiSpeed = m_ghostRacer->getVerticalSpeed() - randInt(2, 4);
                shouldBeAdded = true;
            }
            
            // If so, add a new zombie cab and break out of the loop
            if (shouldBeAdded)
            {
                double startX;
                if (i == LEFT_LANE)
                    startX = ROAD_CENTER - ROAD_WIDTH/3;
                else if (i == MIDDLE_LANE)
                    startX = ROAD_CENTER;
                else
                    startX = ROAD_CENTER + ROAD_WIDTH/3;
                Actor* newZombieCab = new ZombieCab(this, startX, startY);
                newZombieCab->setVerticalSpeed(vertiSpeed);
                addActor(newZombieCab);
                break;
            }
        }
    }
}

void StudentWorld::addOilSlicks()
{
    int chanceOilSlick = 150 - getLevel()*10;
    if (chanceOilSlick < 40)
        chanceOilSlick = 40;
    if (randInt(0, chanceOilSlick-1) == 0)
        addActor(new OilSlick(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

void StudentWorld::addZombiePeds()
{
    int chanceZombiePed = 100 - getLevel()*10;
    if (chanceZombiePed < 20)
        chanceZombiePed = 20;
    if (randInt(0, chanceZombiePed-1) == 0)
        addActor(new ZombiePedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
}

void StudentWorld::addHumanPeds()
{
    int chanceHumanPed = 200 - getLevel()*10;
    if (chanceHumanPed < 30)
        chanceHumanPed = 30;
    if (randInt(0, chanceHumanPed-1) == 0)
        addActor(new HumanPedestrian(this, randInt(0, VIEW_WIDTH), VIEW_HEIGHT));
}

void StudentWorld::addHolyWaterGoodies()
{
    int chanceOfHolyWater = 100 + 10 * getLevel();
    if (randInt(0, chanceOfHolyWater) == 0)
        addActor(new HolyWaterGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

void StudentWorld::addSoulGoodies()
{
    int chanceOfSoul = 100;
    if (randInt(0, chanceOfSoul-1) == 0)
        addActor(new SoulGoodie(this, randInt(LEFT_EDGE, RIGHT_EDGE), VIEW_HEIGHT));
}

string StudentWorld::generateGameStatText()
{
    ostringstream oss;
    oss.setf(ios::fixed);
    oss.precision(2);
    oss << "Score: " << getScore() << "  ";
    oss << "Lvl: " << getLevel() << "  ";
    oss << "Souls2Save: " << m_soulsToSave << " ";
    oss << "Lives: " << getLives() << "  ";
    oss << "Health: " << m_ghostRacer->getHP() << "  ";
    oss << "Sprays: " << m_ghostRacer->getNumSprays() << "  ";
    oss << "Bonus: " << m_bonus;
    return oss.str();
}
