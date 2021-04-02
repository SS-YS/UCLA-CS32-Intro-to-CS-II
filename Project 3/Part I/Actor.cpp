#include "Actor.h"
#include "GameConstants.h"
#include "StudentWorld.h"
#include <cmath>

void BorderLine::doSomething()
{
    int vert_speed = getVerSpeed() - getStudentWorld()->getGhostRacer()->getVerSpeed();
    int horiz_speed = getHorSpeed();
    double new_x = getX() + horiz_speed;
    double new_y = getY() + vert_speed;
    moveTo(new_x, new_y);
    if (getX() < 0 || getY() < 0 || getX() > VIEW_WIDTH || getY() > VIEW_HEIGHT)
        setToDead();
}

void GhostRacer::doSomething()
{
    // Check if Ghost Racer is swerving off the road
    int LEFT_EDGE = ROAD_CENTER - ROAD_WIDTH/2;
    int RIGHT_EDGE = ROAD_CENTER + ROAD_WIDTH/2;
    if (getX() <= LEFT_EDGE)
    {
        setDirection(82);
        getStudentWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    else if (getX() >= RIGHT_EDGE)
    {
        setDirection(98);
        getStudentWorld()->playSound(SOUND_VEHICLE_CRASH);
    }
    
    // Check if user pressed a key
    else
    {
        int ch;
        if (getStudentWorld()->getKey(ch))
        {
            // User hit a key during this tick
            switch (ch) {
                case KEY_PRESS_LEFT:
                    if (getDirection() < 114)
                        setDirection(getDirection() + 8);
                    break;
                case KEY_PRESS_RIGHT:
                    if (getDirection() > 66)
                        setDirection(getDirection() - 8);
                    break;
                case KEY_PRESS_UP:
                    if (getVerSpeed() < 5)
                        setVerSpeed(getVerSpeed() + 1);
                    break;
                case KEY_PRESS_DOWN:
                    if (getVerSpeed() > -1)
                        setVerSpeed(getVerSpeed() - 1);
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
