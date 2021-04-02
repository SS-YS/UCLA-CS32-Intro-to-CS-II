#include "StudentUndo.h"

Undo* createUndo()
{
    return new StudentUndo;
}

StudentUndo::~StudentUndo()
{
    clear();
}

void StudentUndo::submit(const Action action, int row, int col, char ch)
{
    // Check "batching" and possibly merge with the previous undo action
    UndoInfo* preUndo = nullptr;
    if (m_undoSystem.size() != 0)
    {
        preUndo = m_undoSystem.top();
        if (action == preUndo->action)
        {
            if (action == Undo::Action::DELETE && row == preUndo->row && col == preUndo->col) // delete operations occur consecutively at the same spot
            {
                preUndo->text += ch;
                return;
            }
            if (action == Undo::Action::DELETE && row == preUndo->row && col == preUndo->col - 1) // backspace operations occur consecutively in adjacent spots on the same line
            {
                preUndo->text = ch + preUndo->text;
                preUndo->row = row;
                preUndo->col = col;
                return;
            }
            if (action == Undo::Action::INSERT && row == preUndo->row && col == preUndo->col + 1) // insert operations occur consecutively in adjacent spots on the same line
            {
                preUndo->count++;
                preUndo->row = row;
                preUndo->col = col;
                return;
            }
        }
    }
    
    // Add the new undo action to the system
    UndoInfo* newUndo = new UndoInfo;
    newUndo->action = action;
    newUndo->row = row;
    newUndo->col = col;
    newUndo->count = 1;
    newUndo->text = "";
    newUndo->text += ch;
    m_undoSystem.push(newUndo);
}

StudentUndo::Action StudentUndo::get(int& row, int& col, int& count, std::string& text)
{
    if (m_undoSystem.size() == 0)
        return Undo::Action::ERROR;
        
    // Get next undo's information
    count = 1;
    text = "";
    row = m_undoSystem.top()->row;
    col = m_undoSystem.top()->col;
    Undo::Action action;
    switch (m_undoSystem.top()->action)
    {
        case Undo::Action::INSERT:
        {
            action = Undo::Action::DELETE;
            count = m_undoSystem.top()->count;
            col -= count;
            break;
        }
        case Undo::Action::DELETE:
        {
            action = Undo::Action::INSERT;
            text = m_undoSystem.top()->text;
            break;
        }
        case Undo::Action::SPLIT:
        {
            action = Undo::Action::JOIN;
            break;
        }
        case Undo::Action::JOIN:
        {
            action = Undo::Action::SPLIT;
            break;
        }
        case Undo::Action::ERROR: // fix compilation warning
        {
            action = Undo::Action::ERROR;
            break;
        }
    }
    delete m_undoSystem.top(); 
    m_undoSystem.pop();
    
    return action;
}

void StudentUndo::clear() {
    while (!m_undoSystem.empty())
    {
        delete m_undoSystem.top();
        m_undoSystem.pop();
    }
}
