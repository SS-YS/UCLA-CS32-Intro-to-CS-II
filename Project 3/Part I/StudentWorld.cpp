#include "Actor.h"
#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <vector>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    // Create a Ghost Racer
    m_ghostRacer = new GhostRacer(this);
    
    // Create yellow border line
    for (int j = 0; j < VIEW_HEIGHT / SPRITE_HEIGHT; j++)
    {
        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, LEFT_EDGE, j * SPRITE_HEIGHT));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, RIGHT_EDGE, j * SPRITE_HEIGHT));
    }
    
    // Create white border line
    for (int j = 0; j < VIEW_HEIGHT / (4*SPRITE_HEIGHT); j++)
    {
        int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
        int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, LEFT_EDGE + ROAD_WIDTH/3, j * 4 * SPRITE_HEIGHT));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, RIGHT_EDGE - ROAD_WIDTH/3, j * 4 * SPRITE_HEIGHT));
        m_yOfLastWhite = j * 4 * SPRITE_HEIGHT;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // Ask each actor to do something
    m_ghostRacer->doSomething();
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it) == nullptr)
            continue;
        Actor*& curActor = (*it);
        if (curActor->isAlive())
            curActor->doSomething();
    }
    
    // Remove newly-dead actors
    for (vector<Actor*>::iterator it = m_actors.begin(); it != m_actors.end(); it++)
    {
        if ((*it) == nullptr)
            continue;
        Actor*& curActor = *it;
        if (!curActor->isAlive())
        {
            delete curActor;
            curActor = nullptr;
        }
    }
    
    // Potentially add new border lines
    int new_border_y = VIEW_HEIGHT - SPRITE_HEIGHT;
    m_yOfLastWhite += -4 - getGhostRacer()->getVerSpeed();
    int delta_y = new_border_y - m_yOfLastWhite;
    if (delta_y >= SPRITE_HEIGHT)
    {
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH/2, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_YELLOW_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH/2, new_border_y));
    }
    if (delta_y >= 4 * SPRITE_HEIGHT)
    {
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER - ROAD_WIDTH/2 + ROAD_WIDTH/3, new_border_y));
        m_actors.push_back(new BorderLine(this, IID_WHITE_BORDER_LINE, ROAD_CENTER + ROAD_WIDTH/2 - ROAD_WIDTH/3, new_border_y));
        m_yOfLastWhite = new_border_y;
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    delete m_ghostRacer;
    vector<Actor*>::iterator it;
    for (it = m_actors.begin(); it != m_actors.end(); it++)
        delete *it;
}
