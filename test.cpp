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

    IDENTIFIER,NUMBER, ASSIGN, PLUS, MINUS, ASTERISK,//5   
    SLASH,PERCENT,LPAREN,RPAREN,LBRACE,RBRACE,LBRACKET,  //12    
    RBRACKET,SEMICOLON,COMMA,AMPERSAND,LESS,GREATER,//18     
    LESSEQUAL,GREATEREQUAL,EQUAL,NOTEQUAL,IF,ELSE,//24      
    WHILE,DO,FOR,BREAK,CONTINUE,RETURN,INT,DECLARATION,//32
    REF,BLOC,DROP,NOT,AND,OR,DEBUG,END_OF_FILE,UNKNOWN

};

// Structure pour un token
struct Token {
    TokenType type;
    int ligne;
    std::string valeur;  
    std::string texte;  
};

// Définition de la structure Node
struct Node {
    int type;
    int nvar;
    string valeur; 
    int position = 0; 
    std::vector<Node*> enfants;    

};
void afficherNode(Node* node, int profondeur = 0) {
    // Gestion de l'indentation selon la profondeur de récursion
    std::string indent(profondeur * 2, ' ');
    
    // Affichage des informations de base du node
    std::cout << indent << "Type: " << node->type << "\n";
    std::cout << indent << "Nvar: " << node->nvar << "\n";
    std::cout << indent << "Valeur: " << node->valeur << "\n";
    std::cout << indent << "Position: " << node->position << "\n";

    // Si le node a des enfants, on les affiche
    if (!node->enfants.empty()) {
        std::cout << indent << "Enfants:\n";
        for (Node* enfant : node->enfants) {
            afficherNode(enfant, profondeur + 1);
        }
    } else {
        std::cout << indent << "Aucun enfant\n";
    }
}
void afficherNode(const Node& node, int profondeur = 0) {
    // Gestion de l'indentation selon la profondeur de récursion
    std::string indent(profondeur * 2, ' ');
    
    // Affichage des informations de base du node
    std::cout << indent << "Type: " << node.type << "\n";
    std::cout << indent << "Nvar: " << node.nvar << "\n";
    std::cout << indent << "Valeur: " << node.valeur << "\n";
    std::cout << indent << "Position: " << node.position << "\n";

    // Si le node a des enfants, on les affiche
    if (!node.enfants.empty()) {
        std::cout << indent << "Enfants:\n";
        for (Node* enfant : node.enfants) {
            afficherNode(*enfant, profondeur + 1);  // Note le passage par référence ici
        }
    } else {
        std::cout << indent << "Aucun enfant\n";
    }
}


enum SymbType{
    type_int,
    type_fonc
};


struct Symb{
    SymbType type;
    int position = -1 ;
};

enum NodeType {
    nd_mul, //0
    nd_const,
    nd_moinun,
    nd_not,
    nd_add,
    nd_affect,
    nd_slash,
    nd_percent,
    nd_greaterequal,
    nd_lessequal,
    nd_greater,//10
    nd_less,
    nd_equal,
    nd_notequal,
    nd_and,
    nd_or,
    nd_debug,
    nd_bloc,
    nd_drop,
    nd_ref,
    nd_decl,//20
    nd_cond,
    nd_break,
    nd_continue,
    nd_ancre,
    nd_loop,
    nd_seq,//25
    nd_appel,
    nd_fonc,
    nd_ret,
    nd_adr,
    nd_ind,//30
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
    {GREATEREQUAL, 5, 1, nd_greaterequal},
    {LESSEQUAL, 5, 1, nd_lessequal},
    {GREATER, 5, 1, nd_greater},
    {LESS, 5, 1, nd_less},
    {EQUAL, 4, 1, nd_equal},
    {NOTEQUAL, 4, 1, nd_notequal},
    {AND, 3, 1, nd_and},
    {OR, 2, 1, nd_or},
    {ASSIGN, 1, 0, nd_affect },

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
            else if (T.texte == "return") T.type = RETURN;
            else T.type = IDENTIFIER;
            T.ligne = current_line;
            return;
        }

        if (isdigit(c)) {
            T.texte.clear();//AAAAAAAAAAAAAAAAAAAAAAA
            while (isdigit(c)) {
                T.texte += c;//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa
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

void AjouteEnfant(Node *N, Node *E) {
    N->enfants.push_back(E);
}

Node* creerNode(int type, string valeur) {
    Node* n = new Node;
    n->type = type;
    n->valeur = valeur;
    return n;
}

Node* creerNode(int type) {
    Node* n = new Node;
    n->type = type;
    return n;
}

Node* creerNode(int type, Node* enfant) {
    Node* n = new Node;
    n->type = type;
    AjouteEnfant(n, enfant);
    return n;
}

Node* creerNode(int type, Node* enfant1, Node* enfant2) {
    Node* n = new Node;
    n->type = type;
    AjouteEnfant(n, enfant1);
    AjouteEnfant(n, enfant2);
    return n;
}

Node* creerNode(int type, Node* enfant1, Node* enfant2, Node* enfant3) {
    Node* n = new Node;
    n->type = type;
    AjouteEnfant(n, enfant1);
    AjouteEnfant(n, enfant2);
    AjouteEnfant(n, enfant3);
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


Node *A() {
    // std::cout << "Début de A() : Token actuel = " << T.texte << " (type: " << T.type << "), ligne: " << T.ligne << std::endl;
    
    if (check(NUMBER)) {
        // std::cout << "Nombre détecté : " << L.valeur << std::endl;
        Node *A = creerNode(nd_const, L.texte);//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
        return A;
    }
    else if (check(LPAREN)) {
        Node *A = E(0); 
        accept(RPAREN);
        return A;
    }
    else if (check(IDENTIFIER)) {

        // std::cout << "Identifiant détecté : " << L.texte << std::endl;
        return creerNode(nd_ref, L.texte);//AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
    }
    else {
    std::cerr << "Erreur syntaxique à la ligne " << T.ligne << ": attendu un nombre, une parenthèse ou un identifiant, mais reçu : " 
              << T.texte << " (type: " << T.type << ")" << std::endl;
    erreurSyntaxique("erreur");
    }
}


Node* S() {
    Node *R = A();
    if (check(LPAREN)){
        Node *R1 = creerNode(nd_appel, R);
        
        while (!check(RPAREN)){
            AjouteEnfant(R1,E(0));
            
            check(COMMA);
        } 
        return R1;
    }
    else if (check(LBRACKET)){
        Node *add = creerNode(nd_add,R,E(0));
        accept(RBRACKET);
        check(SEMICOLON);
        R = creerNode(nd_ind,add);

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
    } else if (check(AMPERSAND)){ 
        Node* A = P();
        Node* n = creerNode(nd_adr);
        AjouteEnfant(n,A);
        return n;
    }else if (check(ASTERISK)){
        Node* A = P();
        Node* n = creerNode(nd_ind);
        AjouteEnfant(n,A);
        return n;
    }

    else {
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



Node *I() {

    if (check(DEBUG)) {
        Node *R = E(0);
        accept(SEMICOLON);
        return creerNode(nd_debug, R);
    }
    else if (check(RETURN)){
        Node *R = E(0);
        accept(SEMICOLON);
        return creerNode(nd_ret, R);
    }
    
    
    else if (check(LBRACE)) {
        Node *R = creerNode(nd_bloc);
        while (!check(RBRACE)) {
            AjouteEnfant(R, I());
        }
        return R;
    }
    else if (check(INT)) {
        while(check(ASTERISK)){}
        accept(IDENTIFIER);
        Node *R = creerNode(nd_decl, L.texte);
        accept(SEMICOLON);  // Attente du point-virgule
        return R;
    }
    else if (check(IF)){
        accept (LPAREN);
        Node *E1 = E(0);
        accept(RPAREN);
        Node *I1 = I();
        Node *cond = creerNode(nd_cond,E1,I1);
        if (check(ELSE)){
            Node *I2 = I();
            AjouteEnfant(cond,I2);
        }
        return cond;

    }
    else if (check(WHILE)){
        accept (LPAREN);
        Node *E1 = E(0);
        accept(RPAREN);
        Node *I1 = I();
        Node *brek = creerNode(nd_break);
        Node *cond = creerNode(nd_cond,E1,I1,brek);
        Node *ancre = creerNode(nd_ancre);
        Node *loop = creerNode(nd_loop,ancre,cond);
        return loop;
    }
    else if (check(DO)){
        Node *I1 = I();
        accept(WHILE);
        accept(LPAREN);
        Node *E1 = E(0);
        accept(RPAREN);
        accept(SEMICOLON);
        Node *notx = creerNode(nd_not,E1);
        Node *brek = creerNode(nd_break);
        Node *cond = creerNode(nd_cond,notx,cond);
        Node *ancre = creerNode(nd_ancre);
        Node *loop = creerNode(nd_loop,ancre,I1,cond);
        return loop;
    }
    else if (check(FOR)){
        accept (LPAREN);
        Node *E1 = E(0);
        accept (SEMICOLON);
        Node *E2 = E(0);
        accept (SEMICOLON);
        Node *E3 = E(0);
        accept (RPAREN);

        Node *I1 = I();
        Node *brek = creerNode(nd_break);
        Node *cond = creerNode(nd_cond,E2,I1,brek);
        Node *ancre = creerNode(nd_ancre);
        Node *loop = creerNode(nd_loop,cond,ancre,E3);
        Node *seq = creerNode(nd_seq,E1,loop);
        
        return seq;

    }
    else {
        Node *R = E(0);
        accept(SEMICOLON);  // Attente du point-virgule
        return creerNode(nd_drop, R);
        
    }
}



// Node* F() {

    
//     if(check(INT)){
//         while(check(ASTERISK)){}
//         accept(IDENTIFIER);
//         Node *R = creerNode(nd_fonc, L.texte);
//         accept (LPAREN);
//         if  (check(RPAREN)){
//             AjouteEnfant(R,I());
//         }
//         else {
//             accept(INT);
//             while(check(ASTERISK)){}
//             accept(IDENTIFIER);
//             Node *C = creerNode(nd_decl,L.texte);
//             AjouteEnfant(R,C);
//             while (!check(RPAREN)){
//                 accept(COMMA);
//                 accept(INT);
//                  while(check(ASTERISK)){}

//                 accept(IDENTIFIER);
//                 Node *C = creerNode(nd_decl,L.texte);
//                 AjouteEnfant(R,C);

//             }
//         AjouteEnfant(R,I());

//         } 
//     // afficherNode(R);     
//     return R;
//     }
//     else {
//         return I();
//     }
    
// }

Node* F() {
    accept(INT);
    while(check(ASTERISK)){}
    accept(IDENTIFIER);
    Node *R = creerNode(nd_fonc, L.texte);
    accept(LPAREN);
    while (!check(RPAREN)){
        accept(INT);
        while(check(ASTERISK)){}
        accept(IDENTIFIER);
        Node *R2 = creerNode(nd_decl,L.texte);
        AjouteEnfant(R,R2);
        check(COMMA);
    }
    Node *I1 = I();
    AjouteEnfant(R,I1);
    return R;
    
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
            cout << "get "<<N.position<<endl;
            return ;
        case nd_affect :
            if (N.enfants[0]->type == nd_ref){
            gencode(*N.enfants[1]);
            cout << "dup"<<endl;
            cout << "set "<<N.enfants[0]->position<<endl;
            }
            else if (N.enfants[0]->type == nd_ind){
                gencode (*N.enfants[1]);
                gencode (*N.enfants[0]->enfants[0]);
                cout<<"write"<<endl;
            }
            return ;
        case nd_bloc :
            for (int i = 0; i<N.enfants.size(); i++){
                gencode(*N.enfants[i]);

            }
            return ;
        case nd_drop:
            gencode(*N.enfants[0]);
            cout<< "drop 1"<<endl;
            return;
        case nd_ancre:
            cout<<".l3_"<<label_boucle<<endl;
            return;
        case nd_continue:
            cout<<"jump l3_"<<label_boucle<<endl;

        case nd_break:
            cout<<"jump l2_"<<label_boucle<<endl;
            return;

        case nd_loop : {
            int l = nblabel++;
            int tmp = label_boucle;
            label_boucle = l;
            cout<<".loop_l1_"<<l<<endl;
            for ( Node* E : N.enfants){
                gencode(*E);
            }
            cout<<"jump loop_l1_"<<l<<endl;
            cout<<".l2_"<<l<<endl;
            label_boucle = tmp;
            break; }

        case nd_cond :{

            int temp_if = nblabel;
            nblabel++;
            gencode(*N.enfants[0]);
            cout<<"jumpf if_l1_"<<temp_if<<endl;
            gencode(*N.enfants[1]);
            cout<<"jump if_l2_"<<temp_if<<endl;
            if (N.enfants.size()>2){
                cout<<".if_l1_"<<temp_if<<endl;
                gencode(*N.enfants[2]);
                cout<<".if_l2_"<<temp_if<<endl;

            }
            else {
                cout<<".if_l1_"<<temp_if<<endl;
                cout<<".if_l2_"<<temp_if<<endl;

            }
            break;}

        case nd_seq:
            for (int i = 0; i<N.enfants.size(); i++){
                gencode(*N.enfants[i]);
            }
            break;

        case nd_ret :
            gencode(*N.enfants[0]);
            cout<<"ret"<<endl;
            break;
        case nd_debug:
            gencode(*N.enfants[0]);
            cout<<"dbg"<<endl;
            break;
        

        
        case nd_ind : 

            gencode(*N.enfants[0]);
            cout<<"read"<<endl;
            
            return;
        
        case nd_appel:
            
            cout<<"prep "<<N.enfants[0]->valeur<<endl;
            for(int i = 1; i<N.enfants.size();i++){
                gencode(*N.enfants[i]);
            }
            cout<<"call "<<N.enfants.size()-1<<endl;
            return;
        case nd_fonc:
            cout<<"."<<N.valeur<<endl;
            cout<<"resn "<< N.nvar<<endl;
            gencode(*N.enfants[N.enfants.size()-1]);
            cout<<"push 0"<<endl;
            cout<<"ret"<<endl;

            return;
        case nd_adr : 

            cout<< "prep start"<<endl;
            cout<< "swap"<<endl;
            cout<< "drop 1"<<endl;
            cout<<"push "<<N.enfants[0]->position + 1<<endl;
            cout<<"sub"<<endl;
            
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
Symb *declare(string var);
Symb *find(string var);



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


Symb *declare(string var) {
    // std::cout << "Déclaration de la variable : " << var << std::endl;  // Affiche le nom de la variable

    // Vérification de l'existence dans le scope actuel (scope au sommet de la pile)
    if (pileSymboles.top().find(var) != pileSymboles.top().end()) {

        std::cerr << "Erreur : variable " << var << " déjà déclarée dans ce scope." << std::endl;
        exit(1);  // Lève une erreur en cas de redéclaration
    }

    // Si elle n'est pas déjà déclarée, on la crée dans le scope courant
    Symb s;
    pileSymboles.top()[var] = s;
    return &pileSymboles.top()[var];
}




Symb *find(string var) {
    // std::cout << "Recherche de la variable : " << var << std::endl;

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


void anaSem(Node *N) {
    // std::cout << "Analyse sémantique : type de nœud = " << N->type << std::endl;

    switch (N->type) {
        default: {
            // std::cout << "Analyse sémantique du sous-arbre." << std::endl;
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            return;
        }

        case nd_affect: {
            // std::cout << "Analyse sémantique : affectation." << std::endl;
            if (N->enfants[0]->type != nd_ref && N->enfants[0]->type != nd_ind) {
                erreurfatal("Erreur sémantique : L'affectation doit se faire sur une référence.");
            }
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            return;
        }

        case nd_decl: {
            // std::cout << "AHHHHHHHHH" <<N->valeur<< std::endl;
            Symb *S = declare(N->valeur);
            S->type = type_int;
            S->position = nvar++;
            
            return;
        }

        case nd_ref: {
            // std::cout << "Analyse sémantique : référence."<< std::endl;

            Symb *S = find(N->valeur);
            if (S->type != type_int) {
                erreurfatal("Erreur sémantique : Type incorrect pour la référence.");
            }
            N->position = S->position;
            return;
        }

        case nd_bloc: {
            // std::cout << "Analyse sémantique : début de bloc." << std::endl;
            begin();
            for (int i = 0; i < N->enfants.size(); i++) {
                anaSem(N->enfants[i]);
            }
            end();
            // std::cout << "Analyse sémantique : fin de bloc." << std::endl;
            return;
        }
        case nd_adr : 
            if (N->enfants[0]->type != nd_ref){
                erreurfatal("erreur");
            }
            anaSem(N->enfants[0]);
            return;

        case nd_fonc :{
            
            Symb *S = declare (N->valeur);
            S->type = type_fonc;
            begin();
            nvar = 0;
            
            for (const auto& E : N->enfants) {
            
                anaSem(E);
            }
            
            end();
            N->nvar = nvar - (N->enfants.size()-1);
            return;}

        case nd_appel :

            if(N->enfants[0]->type != nd_ref){
                erreurfatal("erreur...");
            }

            Symb *S = find(N->enfants[0]->valeur);

            if(S->type != type_fonc){
                erreurfatal("erreur...");
                
            }

            for( int i = 1; i<N->enfants.size(); i++){
                anaSem(N->enfants[i]);
            }
            return ;

}

}

Node *optim(Node *A){
    for(int i = 0; i<A->enfants.size();i++){
        A->enfants[i] = optim(A->enfants[i]);
    }
    switch(A->type){
        case nd_add:
            if (A->enfants[0]->type == nd_const && A->enfants[1]->type == nd_const){
                return creerNode(nd_const,A->enfants[0]->valeur + A->enfants[1]->valeur);
            }
            return A;
    }
    
}

// int malloc (int n ){
//     int r;
//     r = *0;
//     *0 = *0 +n;
//     return r;
// }
// int malloc(int n) {
//     int r;
//     r = *(int*)0;       // Cast 0 to an int pointer, then dereference it
//     *(int*)0 = *(int*)0 + n;  // Update the value at address 0
//     return r;
// }

// int free(int p){
// }
int main(int argc, char *argv[]) {
    std::cout << ".start" << std::endl;
    std::cout << "prep main" << std::endl;
    std::cout << "call 0" << std::endl;
    std::cout << "halt" << std::endl;


    begin();
    
    std::string code = lireFichier(argv[1]);
    // std::cout << "Code source : " << code << std::endl;
        analex(code);
        
        while (T.type != END_OF_FILE) {
                    
            Node* N = anaSynt();
            nvar = 0;
            anaSem(N);
            // optim(N);
            //    cout<<"resn "<<nvar<<endl;
            gencode(*N);
        //    cout<<"drop "<<nvar<<endl;
            
        }
    
    // std::cout << "dbg\nhalt" << std::endl;
}
