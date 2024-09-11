#include <unordered_map>
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <stack>


using namespace std;

// Enum pour les types de tokens
enum TokenType {
    IDENTIFIER,NUMBER, ASSIGN, PLUS, MINUS, ASTERISK,   
    SLASH,PERCENT,LPAREN,RPAREN,LBRACE,RBRACE,LBRACKET,      
    RBRACKET,SEMICOLON,COMMA,AMPERSAND,LESS,GREATER,       
    LESSEQUAL,GREATEREQUAL,EQUAL,NOTEQUAL,IF,ELSE,      
    WHILE,DO,FOR,BREAK,CONTINUE,RETURN,INT,DECLARATION,REF,BLOC,DROP,NOT,           
    AND,OR,DEBUG,END_OF_FILE,UNKNOWN        
};

// Structure pour un token
struct Token {
    int type;
    int ligne;
    int valeur;  
    std::string texte;  
};

// Définition de la structure Node
struct Node {
    int type;
    int valeur; 
    int position; 
    std::vector<Node*> enfants;
};


enum SymbType{
    type_int,
};

struct Symb{
    SymbType type;
    int position = -1 ;
};
enum NodeType {
    nd_mul,
    nd_const,
    nd_moinun,
    nd_not,
    nd_add,
    nd_affect,
    nd_slash,
    nd_percent,
    nd_greaterequal,
    nd_lessequal,
    nd_greater,
    nd_less,
    nd_equal,
    nd_notequal,
    nd_and,
    nd_or,
    nd_debug,
    nd_bloc,
    nd_drop,
    nd_ref,
    nd_decl,
};

struct Operateur {
    TokenType tok_type;
    int priorite;
    int associ;
    NodeType node_type;
};

vector <Operateur> Operateurs = {
    {ASTERISK, 70, 1, nd_mul},
    {SLASH, 70, 1, nd_slash},
    {PERCENT, 70, 1, nd_percent},
    {PLUS, 60, 1, nd_add},
    {MINUS, 60, 1, nd_moinun},
    {GREATEREQUAL, 50, 1, nd_greaterequal},
    {LESSEQUAL, 50, 1, nd_lessequal},
    {GREATER, 50, 1, nd_greater},
    {LESS, 50, 1, nd_less},
    {EQUAL, 40, 1, nd_equal},
    {NOTEQUAL, 40, 1, nd_notequal},
    {AND, 30, 1, nd_and},
    {OR, 20, 1, nd_or},
    {ASSIGN, 10, 0, nd_affect },
    // {DEBUG,,,nd_debug},
    // {DECLARATION,,,nd_decl},
    // {BLOC,,,nd_bloc},
    // {REF,,,nd_ref},
    // {DROP,,,nd_drop},

};
struct OperatorToCode{
    NodeType type;
    string code;
};

std::vector<OperatorToCode> OperatorGenCode = {
    {nd_mul, "mul"},
    {nd_add, "add"},
    {nd_slash, "div"},
    {nd_percent, "mod"},
    {nd_greaterequal,"cmpge"},
    {nd_lessequal, "cmple"},
    {nd_greater, "cmpgt"},
    {nd_less, "cmplt"},
    {nd_equal, "cmpeq"},
    {nd_notequal, "cmpne"},
    {nd_and, "and"},
    {nd_or, "or"},
};

void gencode(Node& N) {
for (const auto& op : OperatorGenCode) {
            if (op.type == N.type) {
                for (int i = 0; i<N.enfants.size(); i++){
                    gencode(*N.enfants[i]);
                }
                cout<<op.code<<endl;
                
            }
    }
}

int main() {
    Node add;
    add.type = nd_bloc;
    gencode(add);
}