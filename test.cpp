#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

// Enum pour les types de tokens
enum TokenType {
    IDENTIFIER,NUMBER, ASSIGN, PLUS, MINUS, ASTERISK,   
    SLASH,PERCENT,LPAREN,RPAREN,LBRACE,RBRACE,LBRACKET,      
    RBRACKET,SEMICOLON,COMMA,AMPERSAND,LESS,GREATER,       
    LESSEQUAL,GREATEREQUAL,EQUAL,NOTEQUAL,IF,ELSE,      
    WHILE,DO,FOR,BREAK,CONTINUE,RETURN,INT, NOT,           
    AND,OR,END_OF_FILE,UNKNOWN        
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
    std::vector<Node*> enfants;  
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
    nd_or
};

struct Operateur {
    TokenType tok_type;
    int priorite;
    int associ;
    NodeType node_type;
};

vector <Operateur> Operateurs = {
    {ASTERISK, 7, 1, nd_mul},
    {SLASH, 7, 1, nd_slash},
    {PERCENT, 7, 1, nd_percent},
    {PLUS, 6, 1, nd_add},
    {MINUS, 6, 1, nd_moinun},
    {GREATEREQUAL, 5, 0, nd_greaterequal},
    {LESSEQUAL, 5, 0, nd_lessequal},
    {GREATER, 5, 0, nd_greater},
    {LESS, 5, 0, nd_less},
    {EQUAL, 4, 0, nd_equal},
    {NOTEQUAL, 4, 0, nd_notequal},
    {AND, 3, 0, nd_and},
    {OR, 2, 0, nd_or},
    {ASSIGN, 1, 0, nd_affect }
};

// Variables globales pour stocker les tokens actuels et précédents
Token T, L;  
const char* src;  // Pointeur vers le code source
int current_line = 1;  // Compteur de ligne

void next() {
    L = T; 

    while (*src != '\0') {
        char c = *src;

        if (isspace(c)) {
            if (c == '\n') {
                current_line++;
            }
            src++;
            continue;
        }

        if (isalpha(c)) {
            T.texte.clear();
            while (isalnum(c) || c == '_') {
                T.texte += c;
                c = *(++src);
            }
            if (T.texte == "if") T.type = IF;
            else if (T.texte == "else") T.type = ELSE;
            else if (T.texte == "while") T.type = WHILE;
            else if (T.texte == "do") T.type = DO;
            else if (T.texte == "for") T.type = FOR;
            else if (T.texte == "break") T.type = BREAK;
            else if (T.texte == "continue") T.type = CONTINUE;
            else if (T.texte == "return") T.type = RETURN;
            else if (T.texte == "int") T.type = INT;
            else T.type = IDENTIFIER;
            T.ligne = current_line;
            return;
        }

        if (isdigit(c)) {
            T.valeur = 0;
            while (isdigit(c)) {
                T.valeur = T.valeur * 10 + (c - '0');
                c = *(++src);
            }
            T.type = NUMBER;
            T.ligne = current_line;
            return;
        }

        switch (c) {
            case '+': T.type = PLUS; T.texte= "+"; src++; break;
            case '-': T.type = MINUS; T.texte= "-"; src++; break;
            case '*': T.type = ASTERISK; T.texte= "*"; src++; break;
            case '/': T.type = SLASH; T.texte= "/"; src++; break;
            case '%': T.type = PERCENT; T.texte= "%"; src++; break;
            case '=': 
                if (*(src + 1) == '=') {
                    T.type = EQUAL;
                    T.texte = "==";
                    src += 2;
                } else {
                    T.type = ASSIGN;
                    T.texte = "=";
                    src++;
                }
                break;
            case '<': 
                if (*(src + 1) == '=') {
                    T.type = LESSEQUAL;
                    T.texte = "<=";
                    src += 2;
                } else {
                    T.type = LESS;
                    T.texte = "<";
                    src++;
                }
                break;
            case '>': 
                if (*(src + 1) == '=') {
                    T.type = GREATEREQUAL;
                    T.texte = ">=";
                    src += 2;
                } else {
                    T.type = GREATER;
                    T.texte = ">";
                    src++;
                }
                break;
            case '!': 
                if (*(src + 1) == '=') {
                    T.type = NOTEQUAL;
                    T.texte = "!=";
                    src += 2;
                } else {
                    T.type = NOT;
                    T.texte = "!";
                    src++;
                }
                break;
            case '&':
                if (*(src + 1) == '&') {
                    T.type = AND;
                    T.texte = "&&";
                    src += 2;
                } else {
                    T.type = AMPERSAND;
                    T.texte = "&";
                    src++;
                }
                break;
            case '|':
                if (*(src + 1) == '|') {
                    T.type = OR;
                    T.texte = "||";
                    src += 2;
                }
                break;
            case '(': T.type = LPAREN; T.texte = "("; src++; break;
            case ')': T.type = RPAREN; T.texte = ")"; src++; break;
            case '{': T.type = LBRACE; T.texte = "{"; src++; break;
            case '}': T.type = RBRACE; T.texte = "}"; src++; break;
            case '[': T.type = LBRACKET; T.texte = "["; src++; break;
            case ']': T.type = RBRACKET; T.texte = "]"; src++; break;
            case ';': T.type = SEMICOLON; T.texte = ";"; src++; break;
            case ',': T.type = COMMA; T.texte = ","; src++; break;
            default: T.type = UNKNOWN; T.texte = c; src++; break;
        }
        T.ligne = current_line;
        return;
    }

    T.type = END_OF_FILE;
    T.ligne = current_line;
}

int check(int type) {
    if (T.type != type) return false;
    next();
    return true;
}

void erreurfatal(const std::string& message) {
    std::cerr << "Erreur fatale à la ligne " << T.ligne << ": " << message << std::endl;
    exit(EXIT_FAILURE);
}

void accept(int type) {
    if (T.type != type) erreurfatal("Token inattendu");
    next();
}

bool analex(const std::string& code) {
    src = code.c_str();
    current_line = 1;
    next();
    return true;
}

void AjouteEnfant(Node *N, Node *E) {
    N->enfants.push_back(E);
}

Node* creerNode(int type, int valeur) {
    Node* n = new Node;
    n->type = type;
    n->valeur = valeur;
    return n;
}

Node* creerNode(int type) {
    Node* n = new Node;
    n->type = type;
    n->valeur = 0;
    return n;
}

Node* creerNode(int type, Node* enfant) {
    Node* n = new Node;
    n->type = type;
    n->valeur = 0;
    AjouteEnfant(n, enfant);
    return n;
}

Node* creerNode(int type, Node* enfant1, Node* enfant2) {
    Node* n = new Node;
    n->type = type;
    n->valeur = 0;
    AjouteEnfant(n, enfant1);
    AjouteEnfant(n, enfant2);
    return n;
}

void erreurSyntaxique(const std::string& message) {
    std::cerr << "Erreur syntaxique à la ligne " << T.ligne << ": " << message << std::endl;
    exit(EXIT_FAILURE);
}

// Déclaration des fonctions pour chaque non-terminal
Node *A();
Node *S();
Node *P();
Node *E(int);
Node *I();
Node *F();

Node *A(){
    if (check(NUMBER)){
        Node *A = creerNode(nd_const, L.valeur);
        return A;
    }
    else if (check(LPAREN)){
        Node *A = E(0); 
        accept(RPAREN);
        return A;
    }
    erreurSyntaxique("erreur");
    return nullptr;
}

Node* S() {
    return A();
}

Node* P() {
    if (check(PLUS)) {
        Node* A = P();
        return A;
    } else if (check(MINUS)) {
        Node* A = P();
        Node* n = creerNode(nd_moinun);
        AjouteEnfant(n, A);
        return n;
    } else if (check(NOT)) {
        Node* A = P();
        Node* n = creerNode(nd_not);
        AjouteEnfant(n, A);
        return n;
    } else {
        return S();
    }
}

Node* E(int pmin) {
    Node *A1 = P();
    while (T.type != END_OF_FILE) {
        bool operateur_trouve = false;
        Operateur operateur_choisi;

        // Parcourir les opérateurs pour trouver celui qui correspond au token actuel
        for (const auto& op : Operateurs) {
            if (op.tok_type == T.type) {
                operateur_choisi = op;
                operateur_trouve = true;
                break;
            }
        }

        // Si aucun opérateur correspondant n'est trouvé ou si la priorité est inférieure à pmin
        if (!operateur_trouve || operateur_choisi.priorite < pmin) {
            return A1;
        }

        // Passe au token suivant
        next();

        // Appliquer la règle de récursion avec une priorité plus élevée
        Node *A2 = E(operateur_choisi.priorite + 1);

        // Créer un nouveau nœud avec l'opérateur choisi et les sous-arbres
        A1 = creerNode(operateur_choisi.node_type, A1, A2);
    }

    return A1;
}

Node* I() {
    return E(0);
}

Node* F() {
    return I();
}

Node* anaSynt(){
    return F();
}

void gencode(Node& N) {  // Prendre un nœud par référence

    switch (N.type) {
        case nd_const:
            std::cout << "push " << N.valeur << std::endl;
            break;
        case nd_not:
            gencode(*N.enfants[0]);
            std::cout << "not" << std::endl;
            break;
        case nd_moinun:
            std::cout << "push 0" << std::endl;
            gencode(*N.enfants[0]);
            std::cout << "sub" << std::endl;
            break;
    }
}

std::string lireFichier(const char* nomFichier) {
    std::ifstream fichier(nomFichier);
    if (!fichier) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << nomFichier << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream buffer;
    buffer << fichier.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[]) {
    std::cout << ".start" << std::endl;
    for (int i = 1; i < argc; i++) {
        analex(lireFichier(argv[i]));
        while (T.type != END_OF_FILE) {
            Node* N = anaSynt();
            gencode(*N);
        }
    }
    std::cout << "dbg\nhalt" << std::endl;
}
