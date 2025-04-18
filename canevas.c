// canevas.c   pour projet Genealogie SDA2 2024-25�
//
// Schoenfelder Franck
///////////////////////////////////

#include "base.h"
#include <stdio.h>

// Tyes predefinis
#define LG_MAX 64

typedef struct s_date {
        Nat jour, mois, annee;
} Date;

typedef Nat Ident;
#define omega 0

typedef struct s_individu {
        Car nom[LG_MAX];
        Date naiss;
        Date deces;
        Ident id;
        Ident idpere;
        Ident idmere;
        Ident icadet; // id du premier fils de cet individu
        Ident ifaine; // id du prochain frere
} * Individu;

typedef struct s_genealogie {
        Individu *tab;      // tableau des individus tri�s par nom
        Nat nb_individus;   // commence a 1
        Ident id_cur;       // id du prochain nouvelle indiviu
        Ident *rang;        // meme taille que tab, T(i) = tab[rang[i-1])
        Nat taille_max_tab; // taille actuelle du tableau (on double a chaque
                            // fois ?)
} * Genealogie;

// DEFINIR ICI VOS CONSTANTES

// PARTIE 1: PROTOTYPES des operations imposees
Ent compDate(Date d1, Date d2);
void genealogieInit(Genealogie *g);
void genealogieFree(Genealogie *g);
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d);
void freeIndividu(Individu id);
// selecteurs
Chaine nomIndividu(Individu ind);
Date naissIndividu(Individu ind);
Nat cardinal(Genealogie g);
Individu kieme(Genealogie g, Nat k);
Individu getByIdent(Genealogie g, Ident i);
Nat getPos(Genealogie g, Chaine name);
Individu getByName(Genealogie g, Chaine name, Date naissance);
// modificateurs
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d);
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d);
void adjFils(Genealogie g, Ident idx, Ident fils, Ident pp, Ident mm);

// PARTIE 2: PROTOTYPES des operations imposees
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff);
void affiche_enfants(Genealogie g, Ident x, Chaine buff);
void affiche_cousins(Genealogie g, Ident x, Chaine buff);
void affiche_oncles(Genealogie g, Ident x, Chaine buff);

// PARTIE 2: PROTOTYPES des operations imposees
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y);
void devient_pere(Genealogie g, Ident x, Ident y);
void devient_mere(Genealogie g, Ident x, Ident y);

// PARTIE 4: PROTOTYPES des operations imposees
Bool estAncetre(Genealogie g, Ident x, Ident y);
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y);
Ident plus_ancien(Genealogie g, Ident x);
void affiche_parente(Genealogie g, Ident x, Chaine buf);
void affiche_descendance(Genealogie g, Ident x, Chaine buf);

// PROTOTYPES DE VOS FONCTIONS INTERMEDIAIRES

/// PARTIE 1: Construction de l�arbre g�n�alogique et acc�s
/// ///////////////////////////////////////////////////////
///

#define TAILLE_INIT 10

void raler(const char* msg){
        perror(msg) ;
        exit(-1) ;
}

// PRE: None
Ent compDate(Date d1, Date d2) {
        // calculer les différences
        Ent annee = (d1.annee - d2.annee);
        Ent mois = (d1.mois - d2.mois);
        Ent jour = (d1.jour - d2.jour);

        // analyser tous les cas
        if (annee < 0)
                return -1; // diff  année
        if (annee == 0 && mois < 0)
                return -1; // meme année diff mois
        if (annee + mois == 0 && jour < 0)
                return -1; // meme annee et mois diff jour
        if (annee + mois + jour == 0)
                return 0; // meme jour
        return 1;         // tous les autres cas, différence positive
}

// PRE: None
void genealogieInit(Genealogie *g) {
        *g = MALLOC(struct s_genealogie); // Init de g
        if (*g == NULL) raler("malloc genInti") ;

        // Init des tableaux dynamiques et variables
        (*g)->tab = MALLOCN(Individu, TAILLE_INIT);
        if ((*g)->tab == NULL ) raler("malloc g->tab") ;

        (*g)->nb_individus = 0;
        (*g)->id_cur = 0; // premier id possible

        (*g)->rang = MALLOCN(Ident, TAILLE_INIT);
        if ((*g)->rang == NULL ) raler("malloc g->rang") ;

        (*g)->taille_max_tab = TAILLE_INIT; // premier id possible
}

// PRE: None
void genealogieFree(Genealogie *g) {
        // libérer les elements du tableau
        for (Nat i = 0; i < (*g)->nb_individus; i++) {
                freeIndividu((*g)->tab[i]);
        }
        // liberer les tableaux
        FREE((*g)->tab);
        FREE((*g)->rang);

        FREE(*g);
        // les autres éléments sont statique
}

// PRE: None
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d) {
        if (s[0] == '\0')
                return NULL; // individue invalide

        Individu idv = MALLOC(struct s_individu);
        if (idv == NULL) raler("MALLOC nouvIdv") ;
        chaineCopie(idv->nom, s);
        idv->naiss = n;
        idv->deces = d;
        idv->id = i;
        idv->idpere = p;
        idv->idmere = m;
        idv->icadet = omega; // seront initilisés lors de adj
        idv->ifaine = omega;

        return idv;
}

// PRE: None
void freeIndividu(Individu id) { FREE(id); }

// Selecteur
// //////////////////// préconditions de non vacuité avec chacuns des arguments
// et k >= 0
Chaine nomIndividu(Individu ind) { return ind->nom; }
Date naissIndividu(Individu ind) { return ind->naiss; }
Nat cardinal(Genealogie g) { return g->nb_individus; }
Individu kieme(Genealogie g, Nat k) {
        if (k >= g->nb_individus)
                raler("Out of range") ;
        return g->tab[k]; 
}

// PRE: None
//  permet d acceder a un indivdue avec son identidiant. je pars du principe que
//  g est initialisé
Individu getByIdent(Genealogie g, Ident i) {
        if (i <= g->nb_individus && 0 < i) // ne depasse pas du tableau
                return g->tab[ g->rang[i-1]];
        return NULL ;
}

// PRE: None
Nat getPos(Genealogie g, Chaine name) {
        
        if (g->nb_individus == 0) return 0; 
        Ent mid;
        Ent min = 0;
        Ent max = g->nb_individus -1 ;
        Ent diff = 1;
        while (min <= max) {
                mid = (min + max) / 2;
                diff = chaineCompare(name, (g->tab[mid])->nom);
                // on regarde si il y a une différence et on garde celle ci
                if (diff < 0)
                        max = mid - 1;
                else if (diff > 0)
                        min = mid + 1;
                else{ // le nom correspond alors on regarde la date
                        max -- ; // on regarde la premier en dessous
                }
        }
        return min; 
}

// PRE: None
/*
 * On utilise la dichotomie pour recherceher
 */
Individu getByName(Genealogie g, Chaine name, Date naissance) {
        if (g->nb_individus == 0)
                return NULL;

        Date datecur = {0, 0, 0};
        Individu idvcur = NULL;

        Nat pos = getPos(g, name); // recupere position du premier element de la
                                   // liste avec le nom donné

        if (compDate(datecur, naissance) == 0 && chaineCompare(name, g->tab[pos]->nom) == 0){ // si on veut le plus jeune si le nom existe deja
                datecur = g->tab[pos]->naiss ;
                while (pos < g->nb_individus && chaineCompare(name, g->tab[pos]->nom) == 0){ 
                        if (compDate(g->tab[pos]->naiss, datecur ) > 0){ // la nouvelle position a une date plus petite que l ancienne
                                datecur = g->tab[pos]->naiss ;
                                idvcur = g->tab[pos] ;
                        }
                        pos ++ ;
                }
        }
        
        // sinon on veut celui qui correspond et le retourne s il existe
        else {
                while (pos < g->nb_individus && chaineCompare(name, g->tab[pos]->nom) == 0){
                        if (compDate(naissance, g->tab[pos]->naiss) == 0) return g->tab[pos] ;
                        pos ++ ;
                }
        }

        return idvcur ;
}

/*
 * Cette fonction crée un nouvel individu, qui a pour identifiant la valeur de
id_cur, et le place dans le tableau tab en position pos. Si nécessaire, elle
réalloue des tableaux plus grands. Elle met aussi à jour le tableau rang[ ].
 */
// PRE: (pos>=1 => chaineCompare(g[pos-1]->nom,s)<=0)
//      && (pos<g->nb_individus-1 => chaineCompare(g[pos+1]->nom,s)>=0)
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d) {
        Individu idv = nouvIndividu(g->id_cur, s, p, m, n, d);

        // deix situationspossible :
        // 1 - espace suffisant : on décalle les elementsapres position dans une
        // boucle
        //
        // 2 - espace insufisant
        //      realloc (fait une copie) puis decallage, 2eme copie
        // OU
        //      mallo_c, copie manuelle de toutes les donneés meme si pas d
        //      optimisation necessaire
        //          dans les deux cas on finis par decaller

        // on realloue les deux tableaux de g
        Individu*  r_idv ;
        Ident* r_rang ;

        g->nb_individus ++ ;
        if (g->nb_individus >= g->taille_max_tab) {
                g->taille_max_tab *= 2 ;
                r_idv = REALLOC(g->tab, Individu,g->taille_max_tab);
                if (r_idv == NULL) raler("REALLOC idv") ;
                r_rang = REALLOC(g->rang, Ident, g->taille_max_tab);
                if (r_rang== NULL) raler("REALLOC rang") ;
                g->tab = r_idv ;
                g->rang = r_rang ;
        }

        // opération de décallage de position pour liste d individus
        for (Nat i = g->nb_individus; i > pos; i--)
                g->tab[i] = g->tab[i - 1];

        g->tab[pos] = idv;        // placement de idv dans sa place

        // pour le rang, puisqu il change a chaque nouvel eleemnts on doit incrémenté toutes les positions superieurs a pos en le parcourant et i
        for (Nat i = 0 ; i < g->nb_individus-1 ; i ++){
                if(g->rang[i] >= pos) g->rang[i]++ ;
        }
        g->rang[g->id_cur-1] = pos; // on ajoute a la fin de la liste, cela
                                  // correspond à l'id de idv+1
}

// PRE: getByIdent(g,x)!=NULL) && getByIdent(g,filsa)!=NULL &&  (pp!=omega |
// mm!=omega)
void adjFils(Genealogie g, Ident idx, Ident fils, Ident pp, Ident mm) {

        Individu idv = getByIdent(g, idx) ;

        if (fils == omega || compDate( idv->naiss, getByIdent(g, fils)->naiss )<0){
                if (pp != omega)
                        getByIdent(g, pp)->ifaine = idx ; // on chaine l'ainée des parents
                if (mm != omega)
                        getByIdent(g, mm)->ifaine = idx ;
                idv->icadet = fils ;
                return ; // ainée remplacé, on a fini
        }

        // changement d ainée : deux possibilités
        // 1 - les parents n avaient pas d'ainées
        // 2 - les parents le nouveau fils est plus agé que l ainée
        // Dans les deux cas, l'ainée precédent, fils, devient le cadet du niuvel individu

        // cas 2 - insertion en milieu ou fin
	Ident fratrie = fils ;

        // je parcours les cadets a partir de omega jusqu'a  ce qu'un soit omega ou sa naiss est inferieur à naiss de idv
        while (getByIdent(g, fratrie)->icadet != omega && compDate(idv->naiss, getByIdent(g, getByIdent(g, fratrie)->icadet )->naiss) > 0 ){
                fratrie =getByIdent(g, fratrie)->icadet  ;
        }
	// On insert idv entre la fratrie qu on (frocement existant) a et son cadet(peut etre omega)
	idv->icadet = getByIdent(g, fratrie)->icadet ;
	getByIdent(g, fratrie)->icadet = idx ;	
}

// PRE:  (p==omega || getByIdent(g,p)!=NULL) && (m==omega ||
// getByIdent(g,m)!=NULL) &&
//       !freres_soeurs(g,p,m) && getByIdent(g,p)->ifaine ==
//       getByIdent(g,m)->ifaine && compDate(getByIdent(g,p)->naiss,n)<0 &&
//       compDate(getByIdent(g,m)->naiss,n)<0
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d) {
        // valeurs interdime
        if (s == NULL || s[0] == 0 || n.jour == 0 || n.mois == 0 ||
            n.annee == 0)
                return omega;


        // verifier que element inexistant
        if (getByName(g, s, n)) {
                return omega ; // existe deja, rien de plus a faire
        }
        // On doit -> ajouter l individu aux listes contigues
        // l ajouter aux liste frere/soeur

        g->id_cur++;

        Nat pos = getPos(g, s) ;

        insert(g, pos, s, p, m, n, d) ;
        // id affecté

        // trouver le fils entre pere et mere
        Ident fils ;
        if (p != omega || m != omega){
                if (p == omega) fils = getByIdent(g, m)->ifaine ;
                else fils = getByIdent(g, p)->ifaine ;
                adjFils(g, g->id_cur , fils, p, m) ;
        }
        return g->id_cur;
}

//
/// PARTIE 2: Affichages
/// ///////////////////////////////////////////////////////
///

// ajoute au buffer a partir de n
// n est le dernier endroit modifé
void ajouteBufferN(Chaine buff, Nat* n, Chaine str){
        chaineCopie(buff+*n, str) ;
        *n += chaineLongueur(str) ;
}

// PRE: None
void affiche_freres_soeurs_n(Genealogie g, Ident x, Chaine buff,Nat *n) {
        // pas de securité sur la taille du buffer, mais il aurait fallue un type [] pour que ce soit simple

        // pour recuperer tous les freres et soeur, on accède son parent. et on les ajoute au buf
        if (x == omega) return ; // pas de frere et soeur

        Ident ifils ;
        Individu idv = getByIdent(g, x) ;
        if (idv->idpere == omega && idv->idmere == omega) return ; // pas de parents
        if (idv->idpere == omega) ifils = getByIdent(g, idv->idmere)->ifaine ; // on utimise ainée de la mere
        else ifils = getByIdent(g, idv->idpere)->ifaine ; // on utilise ainée de la mere
        
        // on a l ainée, maientant on boucle et a chque fois on ajoute le contenue de n à lngchaine en gardant n.
        Individu a ;
        while ( ifils != omega){
                a = getByIdent(g, ifils) ;
                if (idv->id != a->id){
                        ajouteBufferN(buff, n, a->nom) ;
                        ajouteBufferN(buff, n, " ") ;
                }
                ifils = a->icadet ;
        }
}

void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff){
        Nat n = 0 ;
        affiche_freres_soeurs_n(g, x, buff, &n) ;
}

void affiche_enfants_n(Genealogie g, Ident x, Chaine buff, Nat *n){
        if (x == omega) return ;
        Ident f = getByIdent(g, x)->ifaine;
        Individu a ;
        while (f != omega){
                // ajoute a la chaine
                a = getByIdent(g, f) ;
                ajouteBufferN(buff, n, a->nom) ;
                ajouteBufferN(buff, n, " ") ;
                f = a->icadet ;
        }
}

// PRE: None
void affiche_enfants(Genealogie g, Ident x, Chaine buff) {
        Nat n = 0 ;
        affiche_enfants_n(g, x, buff, &n) ;
}

// affiche les neveux et nieces de x
void affiche_neveux_n(Genealogie g, Ident x, Chaine buff, Nat *n){
        if (x == omega) return ;
        Ident frSr ;
        // recuperer l ainée de x a partir d un des ses parents
        Individu idv = getByIdent(g, x) ;
        if (idv->idpere == omega && idv->idmere == omega) return ; // pas de parents
        if (idv->idpere == omega) frSr = getByIdent(g, idv->idmere)->ifaine ; // on utimise ainée de la mere
        else frSr = getByIdent(g, idv->idpere)->ifaine ; // on utilise ainée de la mere
        
        // on a l'ainée mainetnant qu on parcour et ajoute la liste dnenfant au buffer en esperant aucune consanguinité

        while(frSr != omega){
                if (frSr != x){ // on affiche pas lui meme
                        affiche_enfants_n(g, frSr, buff,n) ;
                }
                frSr = getByIdent(g, frSr)->icadet ;
        }

}

// PRE: None
void affiche_cousins(Genealogie g, Ident x, Chaine buff) {
        // ideal : créer une liste de buffers et les concatener : appeler affiche enfant
        // double boucle pour parcourir freres des parents puis leurs enfant
        if (x ==omega) return ;
        // parcour oncles/tantes
        Nat n = 0 ;
        affiche_neveux_n(g, getByIdent(g, x)->idpere, buff, &n) ;
        affiche_neveux_n(g, getByIdent(g, x)->idmere, buff, &n) ;
}

// PRE: None
void affiche_oncles(Genealogie g, Ident x, Chaine buff) {
        // recup parents
        if (x == omega) return ;
        Nat n = 0 ;
        affiche_freres_soeurs_n(g, getByIdent(g, x)->idpere, buff,&n) ;
        affiche_freres_soeurs_n(g, getByIdent(g, x)->idmere, buff,&n) ;
}

//
/// PARTIE 3: Cr�er Liens de parente
/// ///////////////////////////////////////////////////////
///
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y) {}
void devient_pere(Genealogie g, Ident x, Ident y) {}
void devient_mere(Genealogie g, Ident x, Ident y) {}

//
/// PARTIE 4: Parcours de la genealogie
/// ///////////////////////////////////////////////////////
///

// PRE: None
Bool estAncetre(Genealogie g, Ident x, Ident y) {
        // on suivra des algo recursifs
        Individu idv = getByIdent(g, y);
        if (idv == NULL) return false; // out of range

        Ident p = idv->idpere;
        Ident m = idv->idmere;        

        if (p == x) return true ;   
        if (m == x) return true ;

        if (p != omega && estAncetre(g, x, p)) return true;
        if (m != omega && estAncetre(g, x, m)) return true;

        return estAncetre(g, x, p) || estAncetre(g, x, m) ;
}

void remplirTabAncetreX(Genealogie g, Bool* tab, Ident x ){
        Individu idv = getByIdent(g, x) ;
        if (idv == NULL) return ;

        tab[ x ] = true ; // il existe dans les ancetres

        Ident p = idv->idpere ;
        Ident m = idv->idmere ;

        remplirTabAncetreX(g, tab, p) ;
        remplirTabAncetreX(g, tab, m) ;
}

Bool parcourirTabAncetre(Genealogie g,Bool* tab, Ident y){
        Individu idv = getByIdent(g, y) ;
        if (idv == NULL) return false;

        if (tab[y]) return true; // on s arrete si trouvé

        Ident p = idv->idpere ;
        Ident m = idv->idmere ;

        return parcourirTabAncetre(g, tab, p) || parcourirTabAncetre(g, tab, m) ;
}

// PRE: None
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y) {
       // fonction en plusieurs etapes :
        // creer un tableau vontigue de Booleen composé du nombre totale de g+1 et tout mettre a 0 ou false
        // parcourir x  et mettre true a chaque identifiant de parent recusivmement -> fonction a coté
        // ensuite on parcour y et des qu on trouve un ancetre a case qui donné on stop

        if (getByIdent(g,x) == NULL || getByIdent(g, y) == NULL) return false ;

        Bool* tab_ancetre = CALLOCN( Bool, g->nb_individus + 1 ) ; // +1 pour eviter futurs operations ;
        //calloc met a 0 (false) 

        remplirTabAncetreX(g, tab_ancetre, x) ;

        Bool res = parcourirTabAncetre(g, tab_ancetre, y) ;

        FREE(tab_ancetre) ;

        return res ;
}

// PRE: None
Ident plus_ancien(Genealogie g, Ident x) {
        Individu idv = getByIdent(g, x);
        if (idv == NULL) return omega; // Vérifie que x existe

        Ident p = idv->idpere;
        Ident m = idv->idmere;

        // Si un seul parent est connu, retourne celui-ci directement
        if (p == omega && m != omega) return plus_ancien(g, m);
        if (m == omega && p != omega) return plus_ancien(g, p);

        // Si aucun parent n'est connu, x est le plus ancien
        if (p == omega && m == omega) return x;

        // Recherche récursive du plus ancien ancêtre dans les deux branches
        Ident ancetreP = plus_ancien(g, p);
        Ident ancetreM = plus_ancien(g, m);

        // Comparaison des dates pour trouver l'ancêtre le plus ancien
        Individu idvP = getByIdent(g, ancetreP);
        Individu idvM = getByIdent(g, ancetreM);

        if (idvP != NULL && idvM != NULL) {
        if (compDate(idvP->naiss, idvM->naiss) < 0) return ancetreP;
        return ancetreM;
        }

        if (idvP != NULL) return ancetreP;
        if (idvM != NULL) return ancetreM;

        return x;
}


// PRE: None
void affiche_parente(Genealogie g, Ident x, Chaine buff) { buff[0] = '\0'; }

// PRE: None
void affiche_descendance(Genealogie g, Ident x, Chaine buff) { buff[0] = '\0'; }

void affiche_tableaux(Genealogie g){
        // mes tests
        Individu tmpidv ;
        printf("i  ,nom       ,naiss     ,deces     ,id        ,idpere    ,idmere     ,icadet     ,ifaine     \n") ;
        putchar('\n') ;
        for (Nat i = 0 ; i < cardinal(g) ; i ++){
                tmpidv = g->tab[i] ;
                printf("%-3d,%-10s,%-2d:%-2d:%-4d,%-2d:%-2d:%-4d,%-10d,%-10d,%-10d,%-10d,%-10d\n", 
                        i,
                        tmpidv->nom, 
                        tmpidv->naiss.jour, tmpidv->naiss.mois, tmpidv->naiss.annee,
                        tmpidv->deces.jour, tmpidv->deces.mois, tmpidv->deces.annee,
                        tmpidv->id,
                        tmpidv->idpere,
                        tmpidv->idmere,
                        tmpidv->icadet,
                        tmpidv->ifaine
                );
        }

        putchar('\n') ;
        for (Nat i = 0 ; i < cardinal(g) ; i ++){
                printf("%2d\n", g->rang[i]) ;
        } 
}


//
/// VOS FONCTIONS AUXILIAIRES
/// ///////////////////////////////////////////////////////
///

//
/// MAIN
/// ///////////////////////////////////////////////////////
///
int main() {
        Car buf[500];
        Genealogie g;

        printf("******* Init:\n\n");
        genealogieInit(&g);
        printf("done.\n");

        printf("\n******* adj+getByIndent:\n");
        Date dnull = {0, 0, 0};

        // Famille Potter
        Date jhen = {16, 2, 1867};
        Date jhed = {21, 11, 1932};
        Ident ihep = adj(g, "Henri", 0, 0, jhen, jhed);

        Date jfn = {30, 7, 1905};
        Date jfd = {2, 3, 1979};
        Ident ijfl = adj(g, "Fleamont", ihep, 0, jfn, jfd);

        Date jeu = {12, 6, 1907};
        Date jed = {14, 1, 1979};
        Ident ijm = adj(g, "Euphemia", 0, 0, jeu, jed);

        Date jpn = {27, 3, 1960};
        Date jpd = {29, 7, 1981};
        Ident ijp = adj(g, "James", ijfl, ijm, jpn, jpd);

        Date lpn = {30, 1, 1960};
        Date lpd = {29, 7, 1981};
        Ident ilp = adj(g, "Lily", 0, 0, lpn, lpd);

        Date hn = {31, 7, 1980};
        Ident ih = adj(g, "Harry", ijp, ilp, hn, dnull);

        // Famille Weasley
        Date an = {6, 2, 1950};
        Ident iaw = adj(g, "Arthur", 0, 0, an, dnull);

        Date dpre = {8, 4, 1910};
        Date ddpre = {23, 10, 1968};
        Ident ipre = adj(g, "Prewett", 0, 0, dpre, ddpre);

        Date dfab = {12, 5, 1946};
        Date ddfab = {21, 12, 1982};
        Ident ifab = adj(g, "Fabian", ipre, 0, dfab, ddfab);

        Date mn = {30, 10, 1949};
        Ident imw = adj(g, "Molly", ipre, 0, mn, dnull);

        Date gn = {11, 8, 1981 } ;
        Ident ig = adj(g, "Ginny", iaw, imw, gn, dnull) ;     // Ginny

        Date rn = {1, 3, 1980 } ;
        Ident ir = adj(g, "Ron", iaw, imw, rn, dnull) ;    // Ron

        Date billn = {29, 11, 1970 } ;
        Ident ibill = adj(g, "Bill", iaw, imw, billn, dnull) ; // Bill

        Date hgn = {19, 9, 1980} ;
        Ident ihg = adj(g, "Hermione", omega, omega, hgn, dnull) ;   // Hermione

        Date rosen = {6, 8, 2006} ;
        Ident irose = adj(g, "Rose", ir, ihg, rosen, dnull) ;

        Date a2n = {15, 8, 2006} ;
        Ident ia2 = adj(g, "Albus", ih, ig, a2n, dnull) ;   // Albus

        Date jn = {1, 5, 2005} ;
        Ident ij = adj(g, "James", ih, ig, jn, dnull);    // James (son of Harry)

        // lily fred et george

        Date ln = {2, 5, 2008} ;
        Ident il = adj(g, "Lily", ih, ig, ln, dnull) ;

        Date fgn = { 1, 4, 1978 } ;
        Date geord = {5, 6, 1998} ;
        Ident ifred = adj(g, "Fred", iaw, imw, fgn, geord) ;  
        Ident igeor = adj(g, "George", iaw, imw, fgn, dnull) ;  

        for (Nat i = 0; i < cardinal(g); i++) {
                printf("%s\n", nomIndividu(kieme(g, i)));
        }
        printf("nb_individus: %d\n", cardinal(g));
        printf("Identifiant de Fabian: %u (must be 9)\n", ifab);
        printf("Identifiant de Arthur: %u (must be 7)\n", iaw);

        printf("\nAdding more people:\n");

        /*
        Date dgid = {7, 2, 1945};
        Date ddgid = {21, 12, 1982};
        Ident igid = adj(g, "Gideon", 0, 0, dgid, ddgid);
        printf("Linking Gideon as brother of Molly\n");
        deviennent_freres_soeurs(g, igid, imw);
        Date dhugo = {19, 4, 2008};
        Ident ihugo = adj(g, "Hugo", 0, 0, dhugo, dnull);
        printf("Linking Hugo as son of Hermione\n");
        devient_mere(g, ihg, ihugo);

        */        

        printf("\nTry to add a double Harry:\n");
        Date hu2n = {31, 7, 1980};
        Ident ihu2 = adj(g, "Harry", 0, 0, hu2n, dnull);
        if (ihu2 != omega)
                printf("oh, no! Added double Harry!\n");
        else
                printf("No double Harry: OK!\n");
        printf("Adding more Albus...\n");
        Date ho2n = {1, 8, 1909};
        Ident itho2 = adj(g, "Albus", 0, 0, ho2n, dnull);
        if (itho2 == omega)
                printf("what! Could not add 2nd Albus!\n");
        else
                printf("2nd Albus added: OK!\n");
        Date ho3n = {2, 2, 1832};
        Ident itho3 = adj(g, "Albus", 0, 0, ho3n, dnull);
        if (itho3 == omega)
                printf("what! Could not add 3rd Albus!\n");
        else
                printf("3rd Albus added: OK!\n");
        Individu idch = getByName(g, "Albus", dnull);
        if (idch != NULL)
                printf("Youngest Albus, date birth %d:%d:%d\n",
                       naissIndividu(idch).jour, naissIndividu(idch).mois,
                       naissIndividu(idch).annee);
        else
                printf("what? no Albus! There is a serious problem here...\n");
        printf("Now nb_individus: %d\n", cardinal(g));

        printf("\n******* fratrie:\n");
        printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, ig)));
        buf[0] = 0;
        affiche_freres_soeurs(g, ig, buf);
        printf("%s\n", buf);
        printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, imw)));
        buf[0] = 0;
        affiche_freres_soeurs(g, imw, buf);
        printf("%s\n", buf);

        printf("\n******* les enfants:\n");
        printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ipre)));
        buf[0] = 0;
        affiche_enfants(g, ipre, buf);
        printf("%s\n", buf);
        printf("Enfants de %s:\n", nomIndividu(getByIdent(g, imw)));
        buf[0] = 0;
        affiche_enfants(g, imw, buf);
        printf("%s\n", buf);
        printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ir)));
        buf[0] = 0;
        affiche_enfants(g, ir, buf);
        printf("%s\n", buf);

        printf("\n******* les cousins:\n");
        printf("Cousins de %s:\n", nomIndividu(getByIdent(g, irose)));
        buf[0] = 0;
        affiche_cousins(g, irose, buf);
        printf("%s\n", buf);

        printf("\n******* les oncles et tantes:\n");
        printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ij)));
        buf[0] = 0;
        affiche_oncles(g, ij, buf);
        printf("%s\n", buf);
        printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ir)));
        buf[0] = 0;
        affiche_oncles(g, ir, buf);
        printf("%s\n", buf);

        printf("\n******* les ancetres:\n");
        printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)),
               nomIndividu(getByIdent(g, ia2)),
               estAncetre(g, ijfl, ia2) ? "oui" : "non");

        printf("INDICE\n") ;

        printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)),
               nomIndividu(getByIdent(g, irose)),
               estAncetre(g, ijfl, irose) ? "oui" : "non");

        printf("\n******* les ancetres communs:\n");
        printf("%s et %s ont ancetre commun: %s\n",
               nomIndividu(getByIdent(g, irose)),
               nomIndividu(getByIdent(g, ij)),
               ontAncetreCommun(g, irose, ij) ? "oui" : "non");
        printf("%s et %s ont ancetre commun: %s\n",
               nomIndividu(getByIdent(g, ih)),
               nomIndividu(getByIdent(g, ibill)),
               ontAncetreCommun(g, ih, ibill) ? "oui" : "non");

        printf("\n******* ancetre plus ancien:\n");
        printf("L'ancetre le plus ancien de %s est %s\n",
               nomIndividu(getByIdent(g, ia2)),
               nomIndividu(getByIdent(g, plus_ancien(g, ia2))));

        printf("\n******* parente:\n");
        printf("parents de %s\n", nomIndividu(getByIdent(g, ia2)));
        buf[0] = 0;
        affiche_parente(g, ia2, buf);
        printf("%s\n", buf);

        printf("\n******* descendence:\n");
        printf("descendence de %s\n", nomIndividu(getByIdent(g, ijm)));
        buf[0] = 0;
        affiche_descendance(g, ijm, buf);
        printf("%s\n", buf);

        printf("\n******* free:\n");
        genealogieFree(&g);
        printf("fin.(press key)\n");
        fgets(buf, 2, stdin);
        return 0;
}
