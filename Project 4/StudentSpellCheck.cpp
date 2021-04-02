#include "StudentSpellCheck.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

void StudentSpellCheck::deleteNode(Node* node)
{
    for (int i = 0; i < node->children.size(); i++)
        deleteNode(node->children[i]);
    delete node;
}

StudentSpellCheck::~StudentSpellCheck()
{
    deleteNode(m_root);
}

void StudentSpellCheck::addToTrie(string word)
{
    word += END_OF_WORD; // add a special character to indicate the end of word
    Node* curNode = m_root;
    for (int i = 0; i < word.size(); i++)
    {
        char ch = word[i];
        
        // Find ch in curNode's children
        int j = 0;
        for (; j < curNode->children.size(); j++)
        {
            if (curNode->children[j]->ch == ch)
                break;
        }
        
        if (j == curNode->children.size()) // ch is not found in curNode's children
        {
            Node* newNode = new Node;
            newNode->ch = ch;
            curNode->children.push_back(newNode);
        }
        curNode = curNode->children[j];
    }
}

bool StudentSpellCheck::load(std::string dictionaryFile)
{
    ifstream infile(dictionaryFile);
    if (!infile)
        return false;

    // Discard the old dictionary
    deleteNode(m_root);
    m_root = new Node;
    
    string word;
    while (getline(infile, word))
    {
        // Strip non-letter & non-apostrophe characters and convert every character to lowercase
        string cleanWord = "";
        for (int i = 0; i < word.size(); i++)
        {
            char ch = tolower(word[i]);
            if (isalpha(ch) || ch == '\'')
                cleanWord += ch;
        }

        if (cleanWord != "")
            addToTrie(cleanWord);
    }    
    return true;
}

bool StudentSpellCheck::searchInTrie(std::string word) const
{
    word += END_OF_WORD; // add a special character to indicate the end of word
    Node* curNode = m_root;
    for (int i = 0; i < word.size(); i++)
    {
        char ch = word[i];
        
        // Find ch in curNode's children
        int j = 0;
        for (; j < curNode->children.size(); j++)
        {
            if (curNode->children[j]->ch == ch)
                break;
        }
        
        if (j == curNode->children.size()) // ch is not found in curNode's children
            return false;
        
        curNode = curNode->children[j];
    }
    return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions)
{
    // Convert every character to lowercase
    for (int i = 0; i < word.size(); i++)
        word[i] = tolower(word[i]);
    
    if (searchInTrie(word))
        return true;
    
    // Clear suggestions
    while (!suggestions.empty())
        suggestions.pop_back();
    
    // Constitute and search every possible word that has the same length as word but differs by one character
    string allChars = "abcdefghijklmnopqrstuvwxyz\'";
    for (int i = 0; i < word.size(); i++)
    {
        for (int j = 0; j < allChars.size(); j++)
        {
            if (word[i] == allChars[j])
                continue;
            
            // Replace word[i] with allChars[j] to constitute a new word
            string newWord = word;
            newWord[i] = allChars[j];
            
            // Search the new word
            if (searchInTrie(newWord)) // the new word is a proper spelling suggestion
            {
                if (suggestions.size() < max_suggestions)
                    suggestions.push_back(newWord);
                else
                    return false;
            }
        }
    }
    return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems)
{
	// Clear problems
    while (!problems.empty())
        problems.pop_back();
    
    string word = "";
    int start = NULL;
    for (int i = 0; i <= line.size(); i++)
    {
        char ch;
        if (i == line.size()) // i is just after the last character of the line
            ch = ' '; // let ch be treated as a seperator
        else
            ch = line[i];
        
        if (isalpha(ch) || ch == '\'') // ch is a valid character in a word
        {
            if (word == "") // i starts a new word
                start = i;
            word += tolower(ch); // convert ch to lowercase and add it to current word
        }
        else // ch is a separator
        {
            if (word != "") // between start and i-1 (end) is a word
            {
                if (!searchInTrie(word)) // current word is mispelled
                {
                    Position pos;
                    pos.start = start;
                    pos.end = i - 1;
                    problems.push_back(pos);
                }
                
                // Reinitialize
                word = "";
                start = NULL;
            }
        }
    }
}
