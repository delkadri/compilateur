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
    IDENTIFIER, NUMBER, ASSIGN, PLUS, MINUS, ASTERISK,   
    SLASH, PERCENT, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET,      
    RBRACKET, SEMICOLON, COMMA, AMPERSAND, LESS, GREATER,       
    LESSEQUAL, GREATEREQUAL, EQUAL, NOTEQUAL, IF, ELSE,      
    WHILE, DO, FOR, BREAK, CONTINUE, RETURN, INT, DECLARATION,
    REF, BLOC, DROP, NOT, AND, OR, DEBUG, END_OF_FILE, UNKNOWN
};

// Structure pour un token
struct Token {
    TokenType type;
    int ligne;
    std::string valeur;  // Le champ "valeur" est maintenant toujours un string
    std::string texte;  
};

// Définition de la structure Node
struct Node {
    int type;
    std::string valeur; // Utilisation de string pour la valeur
    int position = 0; 
    std::vector<Node*> enfants;
};

enum SymbType {
    type_int,
    type_fonc
};

struct Symb {
    SymbType type;
    int position = -1;
};

enum NodeType {
    nd_mul, nd_const, nd_moinun, nd_not, nd_add, nd_affect,
    nd_slash, nd_percent, nd_greaterequal, nd_lessequal,
    nd_greater, nd_less, nd_equal, nd_notequal, nd_and, nd_or,
    nd_debug, nd_bloc, nd_drop, nd_ref, nd_decl,
    nd_cond, nd_break, nd_ancre, nd_loop, nd_seq, nd_appel, nd_fonc, nd_ret
};

struct Operateur {
    TokenType tok_type;
    int priorite;
    int associ;
    NodeType node_type;
};

vector<Operateur> Operateurs = {
    {ASTERISK, 70, 1, nd_mul}, {SLASH, 70, 1, nd_slash}, {PERCENT, 70, 1, nd_percent},
    {PLUS, 60, 1, nd_add}, {MINUS, 60, 1, nd_moinun},
    {GREATEREQUAL, 50, 1, nd_greaterequal}, {LESSEQUAL, 50, 1, nd_lessequal},
    {GREATER, 50, 1, nd_greater}, {LESS, 50, 1, nd_less},
    {EQUAL, 40, 1, nd_equal}, {NOTEQUAL, 40, 1, nd_notequal},
    {AND, 30, 1, nd_and}, {OR, 20, 1, nd_or}, {ASSIGN, 10, 0, nd_affect}
};

struct OperatorToCode {
    NodeType type;
    string code;
};

std::vector<OperatorToCode> OperatorGenCode = {
    {nd_mul, "mul"}, {nd_add, "add"}, {nd_slash, "div"}, {nd_percent, "mod"},
    {nd_greaterequal, "cmpge"}, {nd_lessequal, "cmple"}, {nd_greater, "cmpgt"},
    {nd_less, "cmplt"}, {nd_equal, "cmpeq"}, {nd_notequal, "cmpne"},
    {nd_and, "and"}, {nd_or, "or"}
};

// Variables globales pour stocker les tokens actuels et précédents
Token T, L;
int nvar = 0;
const char* src;
int current_line = 1;
int nblabel = 0;
int label_boucle = 0;

void next() {
    L = T;
    while (*src != '\0') {
        char c = *src;
        if (isspace(c)) {
            if (c == '\n') current_line++;
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
            else if (T.texte == "debug") T.type = DEBUG;
            else T.type = IDENTIFIER;
            T.ligne = current_line;
            return;
        }

        if (isdigit(c)) {
            T.valeur.clear();
            while (isdigit(c)) {
                T.valeur += c;
                c = *(++src);
            }
            T.type = NUMBER;
            T.ligne = current_line;
            return;
        }

        switch (c) {
            case '+': T.type = PLUS; T.texte = "+"; src++; break;
            case '-': T.type = MINUS; T.texte = "-"; src++; break;
            case '*': T.type = ASTERISK; T.texte = "*"; src++; break;
            case '/': T.type = SLASH; T.texte = "/"; src++; break;
            case '%': T.type = PERCENT; T.texte = "%"; src++; break;
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
            default:
                T.type = UNKNOWN;
                T.texte = std::string(1, c); 
                src++;
                break;
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
    if (T.type != type) {
        std::cerr << "Erreur : Attendu " << type << " mais reçu " << T.type << " à la ligne " << T.ligne << std::endl;
        erreurfatal("Token inattendu");
    }
    next();
}

bool analex(const std::string& code) {
    src = code.c_str();
    current_line = 1;
    next();
    return true;
}

// Fonction pour ajouter un enfant à un nœud
void AjouteEnfant(Node *N, Node *E) {
    N->enfants.push_back(E);
}

// Fonction pour créer un nœud avec une valeur de type string
Node* creerNode(int type, const std::string& valeur) {
    Node* n = new Node;
    n->type = type;
    n->valeur = valeur;
    return n;
}

// Autres fonctions de création de nœud
Node* creerNode(int type) {
    return new Node{type, ""};
}

Node* creerNode(int type, Node* enfant) {
    Node* n = new Node{type, ""};
    AjouteEnfant(n, enfant);
    return n;
}

Node* creerNode(int type, Node* enfant1, Node* enfant2) {
    Node* n = new Node{type, ""};
    AjouteEnfant(n, enfant1);
    AjouteEnfant(n, enfant2);
    return n;
}

void erreurSyntaxique(const std::string& message) {
    std::cerr << "Erreur syntaxique à la ligne " << T.ligne << ": " << message << std::endl;
    exit(EXIT_FAILURE);
}

// Analyseur syntaxique pour les règles non-terminales
Node *A() {
    if (check(NUMBER)) {
        std::cout << "Nombre détecté : " << L.valeur << std::endl;
        Node *A = creerNode(nd_const, L.valeur);
        return A;
    }
    else if (check(LPAREN)) {
        Node *A = E(0); 
        accept(RPAREN);
        return A;
    }
    else if (check(IDENTIFIER)) {
        return creerNode(nd_ref, L.valeur);
    }

    std::cerr << "Erreur syntaxique à la ligne " << T.ligne << ": attendu un nombre, une parenthèse ou un identifiant, mais reçu : " 
              << T.texte << std::endl;
    erreurSyntaxique("erreur");
    return nullptr;
}

Node* S() {
    Node *R = A();
    if (check(LPAREN)) {
        Node *R = creerNode(nd_appel, R);
        while (!check(RPAREN)) {
            AjouteEnfant(R, E(0));
            check(COMMA);
        } 
    }
    return R;
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

        for (const auto& op : Operateurs) {
            if (op.tok_type == T.type) {
                operateur_choisi = op;
                operateur_trouve = true;
                break;
            }
        }

        if (!operateur_trouve || operateur_choisi.priorite < pmin) {
            return A1;
        }

        next();
        Node *A2 = E(operateur_choisi.priorite + 1);
        A1 = creerNode(operateur_choisi.node_type, A1, A2);
    }

    return A1;
}

Node* I() {
    if (check(DEBUG)) {
        Node *R = E(0);
        accept(SEMICOLON);
        return creerNode(nd_debug, R);
    }
    else if (check(LBRACE)) {
        Node *R = creerNode(nd_bloc);
        while (!check(RBRACE)) {
            AjouteEnfant(R, I());
        }
        return R;
    }
    else if (check(INT)) {
        accept(IDENTIFIER);
        Node *R = creerNode(nd_decl, L.valeur);
        accept(SEMICOLON);
        return R;
    }
    else {
        Node *R = E(0);
        accept(SEMICOLON);
        return creerNode(nd_drop, R);
    }
}

Node* anaSynt() {
    return I();
}

// Fonction de génération de code
void gencode(Node& N) {
    for (const auto& op : OperatorGenCode) {
        if (op.type == N.type) {
            for (int i = 0; i < N.enfants.size(); i++) {
                gencode(*N.enfants[i]);
            }
            cout << op.code << endl;
            return;
        }
    }
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
        case nd_decl:
            return;
        case nd_ref:
            cout << "get " << N.valeur << endl;
            return;
        case nd_affect:
            gencode(*N.enfants[1]);
            cout << "dup" << endl;
            cout << "set " << N.enfants[0]->valeur << endl;
            return;
        case nd_bloc:
            for (int i = 0; i < N.enfants.size(); i++) {
                gencode(*N.enfants[i]);
            }
            return;
        default:
            cerr << "Erreur : Nœud de type inconnu (" << N.type << ")" << endl;
            exit(1);
    }
}

// Fonction pour lire un fichier et retourner son contenu sous forme de string
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

stack<map<string, Symb>> pileSymboles;

Symb* declare(string var) {
    if (pileSymboles.top().find(var) == pileSymboles.top().end()) {
        Symb s;
        pileSymboles.top()[var] = s;
        return &pileSymboles.top()[var];
    } else {
        std::cerr << "Erreur : variable " << var << " déjà déclarée." << std::endl;
        exit(1);
    }
}

Symb* find(string var) {
    stack<map<string, Symb>> pileSymbolesTemp = pileSymboles;
    while (!pileSymbolesTemp.empty()) {
        if (pileSymbolesTemp.top().find(var) != pileSymbolesTemp.top().end()) {
            return &pileSymbolesTemp.top()[var];
        }
        pileSymbolesTemp.pop();
    }
    std::cerr << "Erreur : variable " << var << " non déclarée." << std::endl;
    exit(1);
}


void begin();
void end();

void begin()
{
    //on crée une nouvelle map de Symbole et on la met sur la pile
    map<string, Symb> m;
    pileSymboles.push(m);
}

void end()
{
    //on enlève la map de Symbole du dessus de la pile
    pileSymboles.pop();
}

void anaSem(Node *N) {
    switch (N->type) {
        default:
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            return;
        case nd_affect:
            if (N->enfants[0]->type != nd_ref) {
                erreurfatal("Erreur sémantique : L'affectation doit se faire sur une référence.");
            }
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            return;
        case nd_decl:
            Symb *S = declare(N->valeur);
            S->type = type_int;
            S->position = nvar++;
            return;
        case nd_ref:
            Symb *S = find(N->valeur);
            if (S->type != type_int) {
                erreurfatal("Erreur sémantique : Type incorrect pour la référence.");
            }
            N->position = S->position;
            return;
        case nd_bloc:
            begin();
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            end();
            return;
    }
}

void optim(Node *N) {
    // Optimisation possible ici
}

int main(int argc, char *argv[]) {
    std::cout << ".start" << std::endl;
    
    std::string code = lireFichier(argv[1]);
    std::cout << "Code source : " << code << std::endl;
    analex(code);
    
    while (T.type != END_OF_FILE) {
        Node* N = anaSynt();
        nvar = 0;
        anaSem(N);
        optim(N);
        cout << "resn " << nvar << endl;
        gencode(*N);
        cout << "drop " << nvar << endl;
    }

    std::cout << "dbg\nhalt" << std::endl;
}
