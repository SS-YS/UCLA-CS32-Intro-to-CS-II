#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include <string>
#include <vector>
#include "SpellCheck.h"

char END_OF_WORD = '.'; 

class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() {}
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
    struct Node
    {
        char ch;
        std::vector<Node*> children;
    };
    Node* m_root = new Node;
    
    void deleteNode(Node* node);
    void addToTrie(std::string word);
    bool searchInTrie(std::string word) const;
};

#endif  // STUDENTSPELLCHECK_H_
