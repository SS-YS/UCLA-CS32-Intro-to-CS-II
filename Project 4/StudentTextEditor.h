#ifndef STUDENTTEXTEDITOR_H_
#define STUDENTTEXTEDITOR_H_

#include <list>
#include <string>
#include "TextEditor.h"

class Undo;

class StudentTextEditor : public TextEditor
{
public:
	StudentTextEditor(Undo* undo);
	~StudentTextEditor();
	bool load(std::string file);
	bool save(std::string file);
	void reset();
	void move(Dir dir);
	void del();
	void backspace();
	void insert(char ch);
	void enter();
	void getPos(int& row, int& col) const;
	int getLines(int startRow, int numRows, std::vector<std::string>& lines) const;
	void undo();

private:
    int m_row, m_col; // current editing position
    std::list<std::string> m_text; // stores lines of text
    std::list<std::string>::iterator m_curLine; // an iterator that points to current line of text
    
    void initialize();
    void getIteratorToRow(std::list<std::string>::iterator& it, int curRow, int targetRow) const;
};

#endif // STUDENTTEXTEDITOR_H_
