#include "Sequence.h"

Sequence::Sequence()
{
    // Initialize a sequence
    m_size = 0;
    dummy = new Node;
    dummy->next = nullptr;
}

Sequence::~Sequence()
{
    // Delete every node in the sequence
    Node* p = dummy->next;
    delete dummy;
    for (int i = 0; i < size(); i++)
    {
        Node* temp = p;
        p = p->next;
        delete temp;
    }
}

Sequence::Sequence(const Sequence& src)
{
    // Create a sequence by copying everything from src
    m_size = 0;
    dummy = new Node;
    dummy->next = nullptr;
    for (int i = 0; i < src.size(); i++)
    {
        ItemType value;
        src.get(i, value);
        insert(i, value);
    }
}

Sequence& Sequence::operator=(const Sequence& src)
{
    if (&src == this)
        return *this;
    
    Sequence temp(src); // create a sequence from src
    swap(temp); // swap values
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
    if (pos < 0 || pos > size()) // pos is not in bound
        return -1;
    
    Node* p = dummy; // points to the dummy node
    for (int i = 0; i < pos; i++) // traverse until p points to the node before the insert position
        p = p->next;
    
    // Insert a new node after p
    Node* temp = new Node;
    temp->value = value;
    temp->next = p->next;
    temp->prev = p;
    p->next = temp;
    if (pos != size()) // temp->next is not nullptr
        temp->next->prev = temp;
    m_size += 1; // increment size
    
    return pos;
}

int Sequence::insert(const ItemType& value)
{
    Node* p = dummy->next; // points to the first node (after dummy)
    int i;
    for (i = 0; i < size(); i++)
    {
        if (value <= p->value) // meets the condition
            break;
        p = p->next; // proceed to the next node
    }
    return insert(i, value); // insert at position i
}

bool Sequence::erase(int pos)
{
    if (pos < 0 || pos >= size()) // pos is not in bound
        return false;
    
    Node* p = dummy->next; // points to the first node (after dummy)
    for (int i = 0; i < pos; i++) // traverse until p points to the node to be erased
        p = p->next;
    
    // Erase p
    p->prev->next = p->next;
    if (pos != size() - 1) // p->next is not nullptr
        p->next->prev = p->prev;
    delete p;
    m_size -= 1; // decrement size
    
    return true;
}

int Sequence::remove(const ItemType& value)
{
    Node* p = dummy->next; // points to the first node (after dummy)
    int numRemoved = 0;
    for (int i = 0; i < size(); i++)
    {
        if (p->value == value) // meets the condition
        {
            // Erase p
            p = p->next;
            erase(i);
            numRemoved += 1;
            i--;
        }
        else
            p = p->next; // proceed to the next node
    }
    return numRemoved;
}

bool Sequence::get(int pos, ItemType& value) const
{
    if (pos < 0 || pos >= size()) // pos is not in bound
        return false;
    
    Node* p = dummy->next; // points to the first node (after dummy)
    for (int i = 0; i < pos; i++) // traverse until p points to the target node
        p = p->next;
    value = p->value;
    return true;
}

bool Sequence::set(int pos, const ItemType& value)
{
    if (pos < 0 || pos >= size()) // pos is not in bound
        return false;
    
    Node* p = dummy->next; // points to the first node (after dummy)
    for (int i = 0; i < pos; i++) // traverse until p points to the target node
        p = p->next;
    p->value = value;
    return true;
}

int Sequence::find(const ItemType& value) const
{
    Node* p = dummy->next; // points to the first node (after dummy)
    for (int i = 0; i < size(); i++)
    {
        if (p->value == value) // meets the condition
            return i;
        p = p->next; // proceed to the next node
    }
    return -1; // no such value exists
}

void Sequence::swap(Sequence& other)
{
    // Swap m_size
    int tempSize = size();
    m_size = other.size();
    other.m_size = tempSize;
    
    // Swap the dummy node
    Node* tempDummy = dummy;
    dummy = other.dummy;
    other.dummy = tempDummy;
}

int subsequence(const Sequence& seq1, const Sequence& seq2)
{
    if (seq1.size() < seq2.size() || seq2.empty()) // special cases
        return -1;
    
    for (int i = 0; i <= seq1.size() - seq2.size(); i++) // traverse every possible beginning position
    {
        // For each possible beginning position, traverse every item in seq2
        for (int j = 0; j < seq2.size(); j++)
        {
            ItemType seq1Value, seq2Value;
            seq1.get(i+j, seq1Value);
            seq2.get(j, seq2Value);
            if (seq1Value != seq2Value) // the corresponding items do not match
                break;
            if (j == seq2.size() - 1) // all items have been examined, i begins a subsequence
                return i;
        }
    }
    return -1; // no such subsequence exists
}

void interleave(const Sequence& seq1, const Sequence& seq2, Sequence& result)
{
    Sequence tempResult; // deals with aliasing
    
    // Insert items to tempResult
    int largerSize;
    if (seq1.size() >= seq2.size())
        largerSize = seq1.size();
    else
        largerSize = seq2.size();
    for (int i = 0; i < largerSize; i++)
    {
        ItemType seqValue;
        if (seq1.get(i, seqValue)) // seq1 has an item at position i
            tempResult.insert(tempResult.size(), seqValue); // insert the item at the end of tempResult
        if (seq2.get(i, seqValue)) // seq2 has an item at position i
            tempResult.insert(tempResult.size(), seqValue); // insert the item at the end of tempResult
    }
    
    result = tempResult; // copy tempResult to result
}
