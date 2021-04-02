#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include <stack>
#include <string>
#include "Undo.h"

class StudentUndo : public Undo
{
public:
    virtual ~StudentUndo();
	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
    struct UndoInfo
    {
        Undo::Action action;
        int row, col, count;
        std::string text;
    };
    std::stack<UndoInfo*> m_undoSystem;
};

#endif // STUDENTUNDO_H_
