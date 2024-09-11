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
    TokenType type;
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

// Variables globales pour stocker les tokens actuels et précédents
Token T, L; 
int nvar = 0; 
const char* src;  // Pointeur vers le code source
int current_line = 1;  // Compteur de ligne

void next() {
    L = T; 

    while (*src != '\0') {
        char c = *src;

        std::cout << "Caractère analysé : " << c << " (" << (int)c << ")" << std::endl;

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
            else if (T.texte == "debug") T.type = DEBUG;

            else T.type = IDENTIFIER;
            T.ligne = current_line;
            std::cout << "Identifiant détecté : " << T.texte << std::endl;
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
            std::cout << "Nombre détecté : " << T.valeur << std::endl;
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
            default: 
            std::cerr << "Caractère inconnu détecté : " << c << " (" << (int)c << ")" << std::endl;
            T.type = UNKNOWN;
            T.texte = c; src++; break;
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

// Node *A(){
//     if (check(NUMBER)){        
//         Node *A = creerNode(nd_const, L.valeur);
//         return A;
//     }
//     else if (check(LPAREN)){
//         Node *A = E(0); 
//         accept(RPAREN);
//         return A;
//     }
//     else if (check(IDENTIFIER)){
//         return creerNode(nd_ref, L.valeur);
//     }
//     erreurSyntaxique("erreur");
//     return nullptr;
// }
Node *A() {
    std::cout << "Début de A() : Token actuel = " << T.texte << " (type: " << T.type << "), ligne: " << T.ligne << std::endl;
    
    if (check(NUMBER)) {
        std::cout << "Nombre détecté : " << L.valeur << std::endl;
        Node *A = creerNode(nd_const, L.valeur);
        return A;
    }
    else if (check(LPAREN)) {
        std::cout << "Parenthèse gauche détectée." << std::endl;
        Node *A = E(0); 
        accept(RPAREN);
        std::cout << "Parenthèse droite acceptée." << std::endl;
        return A;
    }
    else if (check(IDENTIFIER)) {
        std::cout << "Identifiant détecté : " << L.texte << std::endl;
        return creerNode(nd_ref, L.valeur);
    }

    std::cerr << "Erreur syntaxique à la ligne " << T.ligne << ": attendu un nombre, une parenthèse ou un identifiant, mais reçu : " 
              << T.texte << " (type: " << T.type << ")" << std::endl;
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

// Node* I() {
//     return E(0);
// }

// Node *I(){
//     if(check(DEBUG)){
//         Node *R = E(0);
//         accept(SEMICOLON);
//         return creerNode(nd_debug, R);
//     }
//     else if(check(LBRACE)){
//         Node *R = creerNode(nd_bloc);
//         while(!check(RBRACE)){
//             AjouteEnfant(R,I());
//         }
//         return R;
//     }
//     else if (check(INT)){
//         accept(IDENTIFIER);
//         Node * R = creerNode(nd_decl, L.valeur);
//         accept(SEMICOLON);
//         return R;
//     }
//     else {
//         Node *R = E(0);
//         accept(SEMICOLON);
//         return creerNode(nd_drop, R);
//     }
// }
Node *I() {
    std::cout << "Début de l'analyse d'une instruction." << std::endl;

    if (check(DEBUG)) {
        std::cout << "Analyse d'une instruction de débogage." << std::endl;
        Node *R = E(0);
        accept(SEMICOLON);
        return creerNode(nd_debug, R);
    }
    else if (check(LBRACE)) {
        std::cout << "Début d'un bloc." << std::endl;
        Node *R = creerNode(nd_bloc);
        while (!check(RBRACE)) {
            std::cout << "Analyse d'une instruction dans un bloc." << std::endl;
            AjouteEnfant(R, I());
        }
        std::cout << "Fin du bloc." << std::endl;
        return R;
    }
    else if (check(INT)) {
        std::cout << "Déclaration d'une variable." << std::endl;
        accept(IDENTIFIER);
        Node *R = creerNode(nd_decl, L.valeur);
        accept(SEMICOLON);  // Attente du point-virgule
        return R;
    }
    else {
        std::cout << "Analyse d'une expression." << std::endl;
        Node *R = E(0);
        accept(SEMICOLON);  // Attente du point-virgule
        return creerNode(nd_drop, R);
    }
}



Node* F() {
    return I();
}

Node* anaSynt(){
    return F();
}


void gencode(Node& N) {  // Prendre un nœud par référence
    bool test = true;
    for (const auto& op : OperatorGenCode) {
        // cout<<op.type<<" "<<N.type<<endl;
            if (op.type == N.type) {
                test = false;
                for (int i = 0; i<N.enfants.size(); i++){
                    gencode(*N.enfants[i]);
                }
                cout<<op.code<<endl;
                
            }
    }
    if (test) {
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
        case nd_decl :
            return;
        case nd_ref : 
            cout << "get"<<N.position<<endl;
            return ;
        case nd_affect :
            gencode(*N.enfants[1]);
            cout << "dup"<<endl;
            cout << "set"<<N.enfants[0]->position<<endl;
            return ;
        case nd_bloc :
            for (int i = 0; i<N.enfants.size(); i++){
                gencode(*N.enfants[i]);

            }
            return ;
        case nd_drop:
            gencode(*N.enfants[0]);
            cout<< "drop "<<N.position<<endl;
            return;
        
        default:
            cerr << "Erreur : Nœud de type inconnu (" << N.type << ")" << endl;
            exit(1);

    }
}
}

std::string lireFichier(const char* nomFichier) {
    std::ifstream fichier(nomFichier);
    if (!fichier) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << nomFichier << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream buffer;
    buffer << fichier.rdbuf();  // Lire tout le fichier dans le buffer
    return buffer.str();  // Renvoyer le contenu sous forme de chaîne de caractères
}

// Une map pour stocker les variables dans chaque scope


// Une pile de scopes
//pile de map de Symbole
stack<map<string, Symb>> pileSymboles;

//declaration des fonctions
Symb *declare(int var);
Symb *find(int var);



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

Symb *declare(int vari) {
    std::string var = std::to_string(vari);
    std::cout << "Déclaration de la variable : " << var << std::endl;

    if (pileSymboles.top().find(var) == pileSymboles.top().end()) {
        Symb s;
        pileSymboles.top()[var] = s;
        return &pileSymboles.top()[var];
    } else {
        std::cerr << "Erreur : variable " << var << " déjà déclarée." << std::endl;
        exit(1);
    }
}

Symb *find(int vari) {
    std::string var = std::to_string(vari);
    std::cout << "Recherche de la variable : " << var << std::endl;

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



// void anaSem(Node *N) {
//     switch (N->type) {
//         default:
//             for (int i = 0; i < N->enfants.size(); i++) {
//                 anaSem(N->enfants[i]);
//             }
//             return;

//         case nd_affect: {
//             if (N->enfants[0]->type != nd_ref) {
//                 erreurfatal("erreur");
//             }
//             for (int i = 0; i < N->enfants.size(); i++) {
//                 anaSem(N->enfants[i]);
//             }
//             return;
//         }

//         case nd_decl: {
//             Symb *S = declare(N->valeur);
//             S->type = type_int;
//             S->position = nvar;
//             nvar++;
//             return;
//         }

//         case nd_ref: {
//             Symb *S = find(N->valeur);
//             if (S->type != type_int) {
//                 erreurfatal("erreur");
//             }
//             N->position = S->position;
//             return;
//         }

//         case nd_bloc: {
//             begin();
//             for (int i = 0; i < N->enfants.size(); i++) {
//                 anaSem(N->enfants[i]);
//             }
//             end();
//             return;
//         }
//     }
// }

void anaSem(Node *N) {
    std::cout << "Analyse sémantique : type de nœud = " << N->type << std::endl;

    switch (N->type) {
        default:
            std::cout << "Analyse sémantique du sous-arbre." << std::endl;
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            return;

        case nd_affect: {
            std::cout << "Analyse sémantique : affectation." << std::endl;
            if (N->enfants[0]->type != nd_ref) {
                erreurfatal("Erreur sémantique : L'affectation doit se faire sur une référence.");
            }
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            return;
        }

        case nd_decl: {
            std::cout << "Analyse sémantique : déclaration." << std::endl;
            Symb *S = declare(N->valeur);
            S->type = type_int;
            S->position = nvar;
            nvar++;
            return;
        }

        case nd_ref: {
            std::cout << "Analyse sémantique : référence." << std::endl;
            Symb *S = find(N->valeur);
            if (S->type != type_int) {
                erreurfatal("Erreur sémantique : Type incorrect pour la référence.");
            }
            N->position = S->position;
            return;
        }

        case nd_bloc: {
            std::cout << "Analyse sémantique : début de bloc." << std::endl;
            begin();
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            end();
            std::cout << "Analyse sémantique : fin de bloc." << std::endl;
            return;
        }
    }
}


void optim(Node *N){
    
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
            cout<<"resn "<<nvar<<endl;
            gencode(*N);
            cout<<"drop "<<nvar<<endl;
            
        }
    
    std::cout << "dbg\nhalt" << std::endl;
}

// int main() {
//     std::string code = "{int x;}";  // Utilisation d'un code source statique

//     std::cout << "Code source : " << code << std::endl;

//     analex(code);

//     try {
//         Node* N = anaSynt();
//         std::cout << "Analyse syntaxique réussie !" << std::endl;

//         anaSem(N);  // Ajout de l'analyse sémantique après l'analyse syntaxique
//         std::cout << "Analyse sémantique réussie !" << std::endl;
//         gencode(*N);

//     } catch (const std::exception& e) {
//         std::cerr << e.what() << std::endl;
//     }

//     return 0;
// }
