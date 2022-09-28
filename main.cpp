#include <bits/stdc++.h>
// TODO remove int to object type in SyntaxToken
using namespace std;

enum SyntaxKind
{
    NumberToken,
    WhitespaceToken,
    PlusToken,
    MinusToken,
    StarToken,
    SlashToken,
    OpenParenthesisToken,
    CloseParenthesisToken,
    BadToken,
    EndOfFileToken,
    NumberExpression,
    BinaryExpression
};

class SyntaxToken
{
private:
    SyntaxKind Kind;
    int Position;
    string Text;
    int Value;

public:
    SyntaxToken(SyntaxKind kind, int position, string text, int value)
    {
        this->Kind = kind;
        this->Position = position;
        this->Text = text;
        this->Value = value;
    }

    SyntaxKind getKind()
    {
        return this->Kind;
    }

    int getPosition()
    {
        return this->Position;
    }

    string getText()
    {
        return this->Text;
    }

    int getValue()
    {
        return this->Value;
    }
};

class Lexer
{
public:
    Lexer(string text)
    {
        this->_text = text;
        this->_position = 0;
    }

    SyntaxToken *NextToken()
    {
        if (this->_position >= this->_text.length())
        {
            string text = "";
            text.push_back(*this->_text.end());
            return new SyntaxToken(EndOfFileToken, this->_position, text, NULL);
        }
        if (isdigit(this->Current()))
        {
            int start = this->_position;

            while (isdigit(this->Current()))
                this->Next();
            int length = this->_position - start;
            string text = this->_text.substr(start, length);
            int value = stoi(text);
            return new SyntaxToken(NumberToken, start, text, value);
        }

        if (isspace(this->Current()))
        {
            int start = this->_position;

            while (isspace(this->Current()))
                this->Next();
            int length = this->_position - start;
            string text = this->_text.substr(start, length);
            return new SyntaxToken(WhitespaceToken, start, text, NULL);
        }

        if (this->Current() == '+')
        {
            return new SyntaxToken(PlusToken, this->_position++, "+", NULL);
        }
        else if (this->Current() == '-')
        {
            return new SyntaxToken(MinusToken, this->_position++, "-", NULL);
        }
        else if (this->Current() == '*')
        {
            return new SyntaxToken(StarToken, this->_position++, "*", NULL);
        }
        else if (this->Current() == '/')
        {
            return new SyntaxToken(SlashToken, this->_position++, "/", NULL);
        }
        else if (this->Current() == '(')
        {
            return new SyntaxToken(OpenParenthesisToken, this->_position++, "(", NULL);
        }
        else if (this->Current() == ')')
        {
            return new SyntaxToken(CloseParenthesisToken, this->_position++, ")", NULL);
        }
        return new SyntaxToken(BadToken, this->_position++, this->_text.substr(this->_position - 1, 1), NULL);
    }

private:
    string _text;
    int _position;

    char Current()
    {
        if (this->_position >= this->_text.length())
        {
            return *this->_text.end();
        }
        return this->_text[this->_position];
    }

    void Next()
    {
        this->_position++;
    }
};

// Abstract class implementation
class SyntaxNode
{
public:
    virtual SyntaxKind Kind();
    virtual vector<SyntaxNode> GetAllChildren();
};

class ExpressionSyntax : public SyntaxNode
{
};
class NumberExpressionSyntax final : public ExpressionSyntax
{
private:
    SyntaxToken *NumberToken;

    SyntaxToken *getNumberToken()
    {
        return this->NumberToken;
    }

public:
    NumberExpressionSyntax(SyntaxToken *numberToken)
    {
        this->NumberToken = numberToken;
    }

    SyntaxKind Kind()
    {
        return NumberExpression;
    }
};

class BinaryExpressionSyntax final : public ExpressionSyntax
{
private:
    ExpressionSyntax *Left;
    SyntaxToken *OperatorToken;
    ExpressionSyntax *Right;

public:
    BinaryExpressionSyntax(ExpressionSyntax *left, SyntaxToken *operatorToken, ExpressionSyntax *right)
    {
        this->Left = left;
        this->Right = right;
        this->OperatorToken = operatorToken;
    }

    ExpressionSyntax *getLeft()
    {
        return this->Left;
    }

    ExpressionSyntax *getRight()
    {
        return this->Right;
    }

    SyntaxToken *getOperatorToken()
    {
        return this->OperatorToken;
    }

    SyntaxKind Kind()
    {
        return BinaryExpression;
    }
};

class Parser
{

public:
    vector<SyntaxToken> tokens;
    Parser(string text)
    {
        this->_position = 0;
        Lexer lexer = Lexer(text);
        SyntaxToken *token;
        do
        {
            token = lexer.NextToken();
            if (token->getKind() != WhitespaceToken && token->getKind() != BadToken)
                this->tokens.push_back(*token);
        } while (token->getKind() != EndOfFileToken);
    }

    ExpressionSyntax *Parse()
    {
        ExpressionSyntax *left = this->ParsePrimaryExpression();
        while (this->Current()->getKind() == PlusToken || this->Current()->getKind() == MinusToken)
        {
            SyntaxToken *operatorToken = this->NextToken();
            ExpressionSyntax *right = this->ParsePrimaryExpression();
            left = new BinaryExpressionSyntax(left, operatorToken, right);
        }
    }

private:
    int _position = 0;

    SyntaxToken *Peek(int offset)
    {
        int index = this->_position + offset;
        if (index >= this->tokens.size())
            return &this->tokens[this->tokens.size() - 1];
        return &this->tokens[index];
    }

    SyntaxToken *Current()
    {
        return this->Peek(0);
    }

    SyntaxToken *NextToken()
    {
        this->_position++;
        return Current();
    }

    SyntaxToken *Match(SyntaxKind kind)
    {
        if (this->Current()->getKind() == kind)
        {
            return NextToken();
        }
        return new SyntaxToken(kind, this->Current()->getPosition(), NULL, NULL);
    }

    ExpressionSyntax *ParsePrimaryExpression()
    {
        SyntaxToken *numberToken = Match(NumberToken);
        return new NumberExpressionSyntax(numberToken);
    }
};

int main()
{
    while (true)
    {
        cout << "> ";
        string line;
        getline(cin, line);
        if (!line.size())
            return 0;

        Lexer lexer = Lexer(line);
        while (true)
        {
            SyntaxToken *token = lexer.NextToken();
            if (token->getKind() == EndOfFileToken)
            {
                break;
            }
            cout << "Token Type " << token->getKind() << " "
                 << "Token Text"
                 << " " << token->getText();
            if (token->getValue())
            {
                cout << "Text Value " << token->getValue();
            }
            cout << "\n";
        }
    }
    return 0;
}