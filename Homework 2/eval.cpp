#include <iostream>
#include <string>
#include <stack>
#include <cassert>
using namespace std;

bool isValidInfix(string infix);
void infixToPostfix(string infix, string& postfix);
void evaluatePostfix(const bool values[], const string& postfix, bool& result);

int evaluate(string infix, const bool values[], string& postfix, bool& result)
// Evaluates a boolean expression
//   If infix is a syntactically valid infix boolean expression,
//   then set postfix to the postfix form of that expression, set
//   result to the value of the expression (where in that expression,
//   each digit k represents element k of the values array), and
//   return zero.  If infix is not a syntactically valid expression,
//   return 1.  (In that case, postfix may or may not be changed, but
//   result must be unchanged.)
{
    if (isValidInfix(infix))
    {
        infixToPostfix(infix, postfix);
        evaluatePostfix(values, postfix, result);
        return 0;
    }
    else
        return 1;
}

bool isValidInfix(string infix)
// Check if infix is a syntactically valid infix boolean expression
{
    if (infix == "")
        return false;
    
    // Check if parentheses are balanced, clear blanks, and check not-allowed characters
    string cleanInfix = "";
    int unclosedParenthesis = 0;
    for (int i = 0; i < infix.size(); i++)
    {
        if (isdigit(infix[i]) || infix[i] == '&' || infix[i] == '|' || infix[i] == '!')
            cleanInfix += infix[i];
        else if (infix[i] == '(')
        {
            unclosedParenthesis += 1;
            cleanInfix += '(';
        }
        else if (infix[i] == ')')
        {
            if (unclosedParenthesis == 0)
                return false;
            unclosedParenthesis -= 1;
            cleanInfix += ')';
        }
        else if (infix[i] == ' ') {}
        else
            return false;
    }
    if (unclosedParenthesis != 0)
        return false;
    
    // Check contents inside parentheses, and replace them for simplicity
    infix = cleanInfix;
    cleanInfix = "";
    for (int i = 0; i < infix.size(); i++)
    {
        if (infix[i] == '(')
        {
            if (infix[i+1] == ')')
                return false;
            // Append into newInfix the string between this '(' and its corresponding ')'
            string newInfix = "";
            int j = i + 1;
            while (infix[j] == '(')
                j += 1;
            while (true)
            {
                if (infix[j] == ')')
                {
                    if (unclosedParenthesis == 0)
                        break;
                    unclosedParenthesis -= 1;
                }
                if (infix[j] == '(')
                    unclosedParenthesis += 1;
                newInfix += infix[j];
                j++;
            }
            // If newInfix is valid, replace it to '0' for simplicity
            if (!isValidInfix(newInfix))
                return false;
            cleanInfix += '0';
            i = j;
        }
        else if (infix[i] == ')') {}
        else
            cleanInfix += infix[i];
    }
        
    // Check '&', '|', '!', and digits
    infix = cleanInfix;
    for (int i = 0; i < infix.size(); i++)
    {
        switch (infix[i]) {
            case '&':
            case '|':
                if (i == 0 || i == infix.size()-1 || infix[i+1] == '&' || infix[i+1] == '|')
                    return false;
                break;
            case '!':
                if (i == infix.size()-1 || infix[i+1] == '&' || infix[i+1] == '|')
                    return false;
                break;
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                if ((i != 0 && isdigit(infix[i-1]))
                    || (i != infix.size()-1 && infix[i+1] != '&' && infix[i+1] != '|'))
                    return false;
                break;
        }
    }
    return true;
}

void infixToPostfix(string infix, string& postfix)
// Set postfix to the postfix form of infix.
{
    postfix = "";
    stack<char> operatorStack;
    for (int i = 0; i < infix.size(); i++)
    {
        switch (infix[i]) {
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                postfix += infix[i];
                break;
            case '(':
                operatorStack.push('(');
                break;
            case ')':
                while (!operatorStack.empty() && operatorStack.top() != '(') {
                    postfix += operatorStack.top();
                    operatorStack.pop();
                }
                operatorStack.pop();
                break;
            case '!':
            case '&':
            case '|':
                while (!operatorStack.empty() && operatorStack.top() != '('
                       && (infix[i] == '|'
                           || (infix[i] == '&' && operatorStack.top() != '|')))
                {
                    postfix += operatorStack.top();
                    operatorStack.pop();
                }
                operatorStack.push(infix[i]);
                break;
            case ' ':
                break;
        }
    }
    while (!operatorStack.empty())
    {
        postfix += operatorStack.top();
        operatorStack.pop();
    }
}

void evaluatePostfix(const bool values[], const string& postfix, bool& result)
// Set result to the value of postfix.
{
    stack<bool> operandStack;
    for (int i = 0; i < postfix.size(); i++)
    {
        if (isdigit(postfix[i]))
            operandStack.push(values[postfix[i] - '0']);
        else
        {
            bool operand2 = operandStack.top();
            operandStack.pop();
            
            if (postfix[i] == '!')
            {
                operandStack.push(!operand2);
                continue;
            }
            
            bool operand1 = operandStack.top();
            operandStack.pop();
            
            if (postfix[i] == '&')
            {
                if (operand2 && operand1)
                    operandStack.push(true);
                else
                    operandStack.push(false);
            }
            
            else if (postfix[i] == '|')
            {
                if (operand2 || operand1)
                    operandStack.push(true);
                else
                    operandStack.push(false);
            }
        }
    }
    result = operandStack.top();
}
