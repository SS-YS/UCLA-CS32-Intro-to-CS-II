#include "newSequence.h"
#include <iostream>

Sequence::Sequence()
{
    m_size = 0;
    m_maxSize = DEFAULT_MAX_ITEMS;
    m_sequence = new ItemType[m_maxSize];
}

Sequence::Sequence(int maxSize)
{
    if (maxSize < 0)
        exit(1);
    
    m_size = 0;
    m_maxSize = maxSize;
    m_sequence = new ItemType[m_maxSize];
}

Sequence::~Sequence()
{
    delete [] m_sequence;
}

Sequence::Sequence(const Sequence& src)
{
    m_size = src.m_size;
    m_maxSize = src.m_maxSize;
    m_sequence = new ItemType[m_maxSize];
    for (int i = 0; i < m_size; i++)
        m_sequence[i] = src.m_sequence[i];
}

Sequence& Sequence::operator=(const Sequence& src)
{
    if (&src == this)
        return *this;
    
    delete [] m_sequence;
    m_size = src.m_size;
    m_maxSize = src.m_maxSize;
    m_sequence = new ItemType[m_maxSize];
    for (int i = 0; i < m_size; i++)
        m_sequence[i] = src.m_sequence[i];
    return *this;
}

bool Sequence::empty() const
{
    return m_size == 0;
}

int Sequence::size() const
{
    return m_size;
}

int Sequence::insert(int pos, const ItemType& value)
{
    if (0 <= pos && pos <= size() && size() < m_maxSize)
    {
        for (int i = size(); i > pos; i--)
            m_sequence[i] = m_sequence[i-1];
        m_sequence[pos] = value;
        m_size += 1;
        return pos;
    }
    else
        return -1;
}

int Sequence::insert(const ItemType& value)
{
    int pos;
    for (pos = 0; pos < size(); pos++)
    {
        if (value <= m_sequence[pos])
            break;
    }
    
    return insert(pos, value);
}

bool Sequence::erase(int pos)
{
    if (0 <= pos && pos < size())
    {
        for (int i = pos; i < size()-1; i++)
            m_sequence[i] = m_sequence[i+1];
        m_size -= 1;
        return true;
    }
    else
        return false;
}

int Sequence::remove(const ItemType& value)
{
    int numRemoved = 0;
    for (int i = 0; i < size(); i++)
    {
        if (m_sequence[i] == value)
        {
            erase(i);
            numRemoved += 1;
            i--;
        }
    }
    return numRemoved;
}

bool Sequence::get(int pos, ItemType& value) const
{
    if (0 <= pos && pos < size())
    {
        value = m_sequence[pos];
        return true;
    }
    else
        return false;
}

bool Sequence::set(int pos, const ItemType& value)
{
    if (0 <= pos && pos < size())
    {
        m_sequence[pos] = value;
        return true;
    }
    else
        return false;
}

int Sequence::find(const ItemType& value) const
{
    for (int i = 0; i < size(); i++)
    {
        if (m_sequence[i] == value)
            return i;
    }
    return -1;
}

void Sequence::swap(Sequence& other)
{
    int tempSize = size();
    m_size = other.size();
    other.m_size = tempSize;
    
    int tempMaxSize = m_maxSize;
    m_maxSize = other.m_maxSize;
    other.m_maxSize = tempMaxSize;
    
    ItemType* tempSequence = m_sequence;
    m_sequence = other.m_sequence;
    other.m_sequence = tempSequence;
}
