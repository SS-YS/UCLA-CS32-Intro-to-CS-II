#include "StudentTextEditor.h"
#include "Undo.h"
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
    return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
: TextEditor(undo)
{
    initialize();
}

StudentTextEditor::~StudentTextEditor()
{
}

void StudentTextEditor::initialize()
{
    m_row = 0;
    m_col = 0;
    m_text.push_back("");
    m_curLine = m_text.begin();
}

bool StudentTextEditor::load(std::string file)
{
    ifstream infile(file);
    if (!infile)
        return false;
    
    // Clear old text
    reset();
    
    // Load new text from file
    string line;
    bool first = true;
    while (getline(infile, line))
    {
        // Strip out the '\r' at the end of line
        if (line.size() > 0 && line[line.size() - 1] == '\r')
            line = line.substr(0, line.size() - 1);
        
        if (first)
        {
            (*m_curLine) = line;
            first = false;
        }
        else
            m_text.push_back(line);
    }
    return true;
}

bool StudentTextEditor::save(std::string file)
{
    ofstream outfile(file);
    if (!outfile)
        return false;
    
    // Save text to file
    list<string>::iterator it = m_text.begin();
    while (it != m_text.end())
    {
        outfile << (*it) << endl;
        it++;
    }
    return true;
}

void StudentTextEditor::reset()
{
    // Clear all text
    while (!m_text.empty())
        m_text.pop_back();
    
    // Reinitialize
    initialize();
    getUndo()->clear();
}

void StudentTextEditor::move(Dir dir)
{
    switch (dir)
    {
        case Dir::UP:
        {
            if (m_row == 0) // current editing position is on the top line
                return;
            m_row--;
            m_curLine--;
            if (m_col > int(m_curLine->size()))
                m_col = int(m_curLine->size());
            break;
        }
        case Dir::DOWN:
        {
            if (m_row == int(m_text.size()) - 1) // current editing position is on the bottom line
                return;
            m_row++;
            m_curLine++;
            if (m_col > int(m_curLine->size()))
                m_col = int(m_curLine->size());
            break;
        }
        case Dir::LEFT:
        {
            if (m_col == 0) // current editing position is on the first character of the line
            {
                if (m_row == 0)
                    return;
                m_row--;
                m_curLine--;
                m_col = int(m_curLine->size());
            }
            else
                m_col--;
            break;
        }
        case Dir::RIGHT:
        {
            if (m_col == int(m_curLine->size())) // current editing position is just after the last character of the line
            {
                if (m_row == int(m_text.size()) - 1)
                    return;
                m_row++;
                m_curLine++;
                m_col = 0;
            }
            else
                m_col++;
            break;
        }
        case Dir::HOME:
        {
            m_col = 0;
            break;
        }
        case Dir::END:
        {
            m_col = int(m_curLine->size());
            break;
        }
    }
}

void StudentTextEditor::del()
{
    if (m_col == int(m_curLine->size())) // current editing position is just after the last character of the line
    {
        if (m_row == int(m_text.size()) - 1)
            return;
        string curLineText = (*m_curLine);
        m_curLine = m_text.erase(m_curLine); // erase current line from the list and let the iterator point to the next line
        (*m_curLine) = curLineText + (*m_curLine); // merge text
        
        // Inform the Undo member variable
        getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
    }
    else
    {
        // Delete current chracter from current line
        string oldText = (*m_curLine);
        (*m_curLine) = oldText.substr(0, m_col) + oldText.substr(m_col + 1, int(oldText.size()) - m_col - 1);
        
        // Inform the Undo member variable
        getUndo()->submit(Undo::Action::DELETE, m_row, m_col, oldText[m_col]);
    }
}

void StudentTextEditor::backspace()
{
    if (m_col == 0 && m_row == 0) // current editing position is at the first character of the first line
            return;
    
    // Delete the character just before current editing position
    move(Dir::LEFT);
    del();
}

void StudentTextEditor::insert(char ch)
{
    if (ch == '\t') // ch is a tab character
    {
        for (int i = 0; i < 4; i++)
            insert(' ');
        return;
    }
    
    // Insert ch to current line of text
    string oldText = (*m_curLine);
    (*m_curLine) = oldText.substr(0, m_col) + ch + oldText.substr(m_col, int(oldText.size()) - m_col);
    
    // Move current editing position one spot to the right
    move(Dir::RIGHT);
    
    // Inform the Undo member variable
    getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
}

void StudentTextEditor::enter()
{
    // Inform the Undo member variable
    getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);
    
    // Split current line of text into two lines
    string text = (*m_curLine); // get current line of text
    string leftText = text.substr(0, m_col);
    string rightText = text.substr(m_col, int(text.size()) - m_col);
    (*m_curLine) = leftText;
    
    m_row++;
    m_curLine++;
    m_col = 0;
    m_text.insert(m_curLine, rightText);
    m_curLine--; // adjust
}

void StudentTextEditor::getPos(int& row, int& col) const
{
    row = m_row;
    col = m_col;
}

void StudentTextEditor::getIteratorToRow(std::list<std::string>::iterator& it, int curRow, int targetRow) const
{
    if (curRow < targetRow)
    {
        for (int i = 0; i < targetRow - curRow; i++)
            it++;
    }
    else if (curRow > targetRow)
    {
        for (int i = 0; i < curRow - targetRow; i++)
            it--;   
    }
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const
{
    if (startRow < 0 || numRows < 0 || startRow > int(m_text.size()))
        return -1;
    
    // Clear the lines variable
    while (!lines.empty())
        lines.pop_back();
    
    // Get to startRow
    list<string>::iterator it = m_curLine;
    getIteratorToRow(it, m_row, startRow);
    
    // Place numRows lines of text into lines
    while (numRows > 0 && it != m_text.end())
    {
        lines.push_back(*it);
        numRows--;
        it++;
    }
    return int(lines.size());
}

void StudentTextEditor::undo()
{
    // Get next undo's information
    int row, col, count;
    string text;
    Undo::Action action = getUndo()->get(row, col, count, text);
    if (action == Undo::Action::ERROR)
        return;
    
    // Undo
    getUndo()->submit(Undo::Action::JOIN, -1, -1); // prevent the following undo actions from "batching" with previous actions
    getIteratorToRow(m_curLine, m_row, row);
    m_row = row;
    m_col = col;
    switch (action) {
        case Undo::Action::INSERT:
        {
            for (int i = 0; i < text.size(); i++)
                insert(text[i]);
            m_col = col; // get back to the specified position
            break;
        }
        case Undo::Action::DELETE:
        {
            for (int i = 0; i < count; i++)
                del();
            break;
        }
        case Undo::Action::SPLIT:
        {
            enter();
            m_curLine--; // get back to the specified position
            m_row = row; // get back to the specified position
            m_col = col; // get back to the specified position
            break;
        }
        case Undo::Action::JOIN:
        {
            del();
            break;
        }
        case Undo::Action::ERROR: // fix compilation warning
        {
            break;
        }
    }
    
    // Remove the preceding undo actions and the meaningless JOIN action
    getUndo()->get(row, col, count, text);
    getUndo()->get(row, col, count, text);
}
