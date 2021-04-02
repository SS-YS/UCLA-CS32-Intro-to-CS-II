#include "Sequence.h"
#include "ScoreList.h"

ScoreList::ScoreList()
{
}

bool ScoreList::add(unsigned long score)
{
    if (score <= 100 && m_scoreList.size() < DEFAULT_MAX_ITEMS)
    {
        m_scoreList.insert(m_scoreList.size(), score);
        return true;
    }
    return false;
}

bool ScoreList::remove(unsigned long score)
{
    int pos = m_scoreList.find(score);
    return m_scoreList.erase(pos);
}

int ScoreList::size() const
{
    return m_scoreList.size();
}

unsigned long ScoreList::minimum() const
{
    if (m_scoreList.empty())
        return NO_SCORE;

    unsigned long min;
    m_scoreList.get(0, min);
    for (int i = 0; i < m_scoreList.size(); i++)
    {
        unsigned long cur;
        m_scoreList.get(i, cur);
        if (cur < min)
            min = cur;
    }
    return min;
}

unsigned long ScoreList::maximum() const
{
    if (m_scoreList.empty())
        return NO_SCORE;

    unsigned long max;
    m_scoreList.get(0, max);
    for (int i = 0; i < m_scoreList.size(); i++)
    {
        unsigned long cur;
        m_scoreList.get(i, cur);
        if (cur > max)
            max = cur;
    }
    return max;
}
