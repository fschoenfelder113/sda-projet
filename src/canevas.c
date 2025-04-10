// canevas.c   pour projet Genealogie SDA2 2024-25�
//

// Schoenfelder Franck
///////////////////////////////////

#include "base.h"

// Tyes predefinis
#define LG_MAX  64

typedef struct s_date { Nat jour, mois, annee; } Date;

typedef Nat Ident;
#define omega  0

typedef struct s_individu {
	Car nom[LG_MAX];
	Date naiss ;
	Date deces ;
	Ident id ;
	Ident idpere ;
	Ident idmere ;
} *Individu;

typedef struct s_genealogie {
	Individu* tab;				// tableau des individus tri�s par nom
	Ent nb_individus ;			// commence a 1
	Ident id_cur ;				// id du prochain nouvelle indiviu
	Ident* rang ; 				// meme taille que tab, T(i) = tab[rang[i-1])
	Ent taille_max_tab ; // taille actuelle du tableau (on double a chaque fois ?)
} *Genealogie;

// DEFINIR ICI VOS CONSTANTES


// PARTIE 1: PROTOTYPES des operations imposees
Ent compDate(Date d1, Date d2);
void genealogieInit(Genealogie* g);
void genealogieFree(Genealogie* g);
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

//PARTIE 2: PROTOTYPES des operations imposees
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff);
void affiche_enfants(Genealogie g, Ident x, Chaine buff);
void affiche_cousins(Genealogie g, Ident x, Chaine buff);
void affiche_oncles(Genealogie g, Ident x, Chaine buff);

//PARTIE 2: PROTOTYPES des operations imposees
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y);
void devient_pere(Genealogie g, Ident x, Ident y);
void devient_mere(Genealogie g, Ident x, Ident y);

//PARTIE 4: PROTOTYPES des operations imposees
Bool estAncetre(Genealogie g, Ident x, Ident y);
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y);
Ident plus_ancien(Genealogie g, Ident x);
void affiche_parente(Genealogie g, Ident x, Chaine buf);
void affiche_descendance(Genealogie g, Ident x, Chaine buf);

// PROTOTYPES DE VOS FONCTIONS INTERMEDIAIRES


/// PARTIE 1: Construction de l�arbre g�n�alogique et acc�s
/// ///////////////////////////////////////////////////////
///

#define VIVANT {0, 0, 0}
#define TAILLE_INIT 10

//PRE: None
Ent compDate(Date d1, Date d2)
{
	return 0;
}

//PRE: None
void genealogieInit(Genealogie *g)
{
	*g = MALLOC(struct s_genealogie) ; // Init de g

  	//Init des tableaux dynamiques et variables
        (*g)->tab = MALLOCN( Individu, TAILLE_INIT ) ;
 	(*g)->nb_individus = 0 ;
	(*g)->id_cur = 1 ; // premier id possible
 	(*g)->rang = MALLOCN( Ident, TAILLE_INIT ) ;
	(*g)->taille_max_tab = TAILLE_INIT ; // premier id possible
}

//PRE: None
void genealogieFree(Genealogie *g)
{
	// libérer les elements du tableau
  	for (int i = 0 ; i < (*g)->taille_max_tab ; i ++)
 	{
 		freeIndividu((*g)->tab[i]) ;
	}
   	// liberer les tableaux
   	FREE((*g)->tab) ;
   	FREE((*g)->rang) ;

  	// les autres éléments sont statique
}

//PRE: None
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d)
{
	if (s[0] == '\0') return NULL ; // individue invalide

	Individu idv = MALLOC(struct s_individu ) ;
  	chaineCopie(idv->nom, s) ;
 	idv->naiss = n ;
 	idv->deces = d ;
	idv->id = i ;
	idv->idpere = p ;
	idv->idmere = m ;

  	return idv ;
}

//PRE: None
void freeIndividu(Individu id)
{
	FREE(id) ;
}

// Selecteur
// ////////////////////
Chaine nomIndividu(Individu ind) { return ""; }
Date naissIndividu(Individu ind) { Date d = { 0,0,0 }; return d; }
Nat cardinal(Genealogie g) { return 0; }
Individu kieme(Genealogie g, Nat k) { return NULL; }

//PRE: None
Individu getByIdent(Genealogie g, Ident i)
{
	return NULL;
}

//PRE: None
Nat getPos(Genealogie g, Chaine name)
{
	return 0;
}

//PRE: None
Individu getByName(Genealogie g, Chaine name, Date naissance)
{
	return NULL;
}

//PRE: (pos>=1 => chaineCompare(g[pos-1]->nom,s)<=0)
//     && (pos<g->nb_individus-1 => chaineCompare(g[pos+1]->nom,s)>=0)
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d)
{
}

// PRE: getByIdent(g,x)!=NULL) && getByIdent(g,filsa)!=NULL &&  (pp!=omega || mm!=omega)
void adjFils(Genealogie g, Ident idx, Ident fils, Ident pp, Ident mm)
{
}

//PRE:  (p==omega || getByIdent(g,p)!=NULL) && (m==omega || getByIdent(g,m)!=NULL) &&
//      !freres_soeurs(g,p,m) && getByIdent(g,p)->ifaine == getByIdent(g,m)->ifaine &&
//      compDate(getByIdent(g,p)->naiss,n)<0 && compDate(getByIdent(g,m)->naiss,n)<0
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d)
{
	// valeurs interdites
	if (s == NULL || s[0] == 0 || n.jour == 0 || n.mois == 0 || n.annee == 0) return omega;

	return omega;
}


//
/// PARTIE 2: Affichages
/// ///////////////////////////////////////////////////////
///

//PRE: None
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff)
{
	buff[0] = '\0';
}

//PRE: None
void affiche_enfants(Genealogie g, Ident x, Chaine buff)
{
	buff[0] = '\0';
}

//PRE: None
void affiche_cousins(Genealogie g, Ident x, Chaine buff)
{
	buff[0] = '\0';
}

//PRE: None
void affiche_oncles(Genealogie g, Ident x, Chaine buff)
{
	buff[0] = '\0';
}

//
/// PARTIE 3: Cr�er Liens de parente
/// ///////////////////////////////////////////////////////
///
void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y)
{

}
void devient_pere(Genealogie g, Ident x, Ident y)
{

}
void devient_mere(Genealogie g, Ident x, Ident y)
{

}


//
/// PARTIE 4: Parcours de la genealogie
/// ///////////////////////////////////////////////////////
///

//PRE: None
Bool estAncetre(Genealogie g, Ident x, Ident y)
{
	return false;
}

//PRE: None
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y)
{
	return false;
}

//PRE: None
Ident plus_ancien(Genealogie g, Ident x)
{
	return omega;
}

//PRE: None
void affiche_parente(Genealogie g, Ident x, Chaine buff)
{
	buff[0] = '\0';
}

//PRE: None
void affiche_descendance(Genealogie g, Ident x, Chaine buff)
{
	buff[0] = '\0';
}

//
/// VOS FONCTIONS AUXILIAIRES
/// ///////////////////////////////////////////////////////
///


//
/// MAIN
/// ///////////////////////////////////////////////////////
///
int main()
{
	Car buf[500];
	Genealogie g;

	printf("******* Init:\n\n");
	genealogieInit(&g);
	printf("done.\n");

	printf("\n******* adj+getByIndent:\n");
	Date dnull = { 0,0,0 };

	// Famille Potter
	Date jhen = { 16,2,1867 }; Date jhed = { 21,11,1932 };
	Ident ihep = adj(g, "Henri", 0, 0, jhen, jhed);

	Date jfn = { 30,7,1905 }; Date jfd = { 2,3,1979 };
	Ident ijfl = adj(g, "Fleamont", ihep, 0, jfn, jfd);

	Date jeu = { 12,6,1907 }; Date jed = { 14, 1,1979 };
	Ident ijm = adj(g, "Euphemia", 0, 0, jeu, jed);

	Date jpn = { 27, 3, 1960 }; Date jpd = { 29, 7, 1981 };
	Ident ijp = adj(g, "James", ijfl, ijm, jpn, jpd);

	Date lpn = { 30, 1, 1960 }; Date lpd = { 29, 7, 1981 };
	Ident ilp = adj(g, "Lily", 0, 0, lpn, lpd);

	Date hn = { 31, 7, 1980 };
	Ident ih = adj(g, "Harry", ijp, ilp, hn, dnull);

	// Famille Weasley
	Date an = { 6, 2, 1950 };
	Ident iaw = adj(g, "Arthur", 0, 0, an, dnull);

	Date dpre = { 8,4, 1910 }; Date ddpre = { 23, 10, 1968 };
	Ident ipre = adj(g, "Prewett", 0, 0, dpre, ddpre);

	Date dfab = { 12, 5, 1946 }; Date ddfab = { 21,12, 1982 };
	Ident ifab = adj(g, "Fabian", ipre, 0, dfab, ddfab);

	Date mn = { 30, 10, 1949 };
	Ident imw = adj(g, "Molly", ipre, 0, mn, dnull);

	// ajouter ici les autres individus
	Ident ig = omega;  // Ginny
	Ident irose = omega; // Rose
	Ident ir = omega; // Ron
	Ident ia2 = omega; // Albus
	Ident ibill = omega; // Bill
	Ident ij = omega; // James (son of Harry)
	Ident ihg = omega; // Hermione

	for (Nat i = 0; i < cardinal(g); i++) {
		printf("%s\n", nomIndividu(kieme(g,i)));
	}
	printf("nb_individus: %d\n", cardinal(g));
	printf("Identifiant de Fabian: %u (must be 9)\n", ifab);
	printf("Identifiant de Arthur: %u (must be 7)\n", iaw);

	printf("\nAdding more people:\n");
	Date dgid = { 7, 2, 1945 }; Date ddgid = { 21,12, 1982 };
	Ident igid = adj(g, "Gideon", 0, 0, dgid, ddgid);
	printf("Linking Gideon as brother of Molly\n");
	deviennent_freres_soeurs(g, igid, imw);
	Date dhugo = { 19,4,2008 };
	Ident ihugo = adj(g, "Hugo", 0, 0, dhugo, dnull);
	printf("Linking Hugo as son of Hermione\n");
	devient_mere(g, ihg, ihugo);

	printf("\nTry to add a double Harry:\n");
	Date hu2n = { 31, 7, 1980 };
	Ident ihu2 = adj(g, "Harry", 0, 0, hu2n, dnull);
	if (ihu2 != omega) printf("oh, no! Added double Harry!\n"); else printf("No double Harry: OK!\n");
	printf("Adding more Albus...\n");
	Date ho2n = { 1,8, 1909 };
	Ident itho2 = adj(g, "Albus", 0, 0, ho2n, dnull);
	if (itho2 == omega) printf("what! Could not add 2nd Albus!\n"); else printf("2nd Albus added: OK!\n");
	Date ho3n = { 2,2, 1832 };
	Ident itho3 = adj(g, "Albus", 0, 0, ho3n, dnull);
	if (itho3 == omega) printf("what! Could not add 3rd Albus!\n"); else printf("3rd Albus added: OK!\n");
	Individu idch = getByName(g, "Albus", dnull);
	if (idch != NULL) printf("Youngest Albus, date birth %d:%d:%d\n", naissIndividu(idch).jour, naissIndividu(idch).mois, naissIndividu(idch).annee);
	else printf("what? no Albus! There is a serious problem here...\n");
	printf("Now nb_individus: %d\n", cardinal(g));


	printf("\n******* fratrie:\n");
	printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, ig)));
	buf[0] = 0;  affiche_freres_soeurs(g, ig, buf);
	printf("%s\n", buf);
	printf("Freres/Soeurs de %s:\n", nomIndividu(getByIdent(g, imw)));
	buf[0] = 0;  affiche_freres_soeurs(g, imw, buf);
	printf("%s\n", buf);

	printf("\n******* les enfants:\n");
	printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ipre)));
	buf[0] = 0;  affiche_enfants(g, ipre, buf);
	printf("%s\n", buf);
	printf("Enfants de %s:\n", nomIndividu(getByIdent(g, imw)));
	buf[0] = 0;  affiche_enfants(g, imw, buf);
	printf("%s\n", buf);
	printf("Enfants de %s:\n", nomIndividu(getByIdent(g, ir)));
	buf[0] = 0;  affiche_enfants(g, ir, buf);
	printf("%s\n", buf);

	printf("\n******* les cousins:\n");
	printf("Cousins de %s:\n", nomIndividu(getByIdent(g, irose)));
	buf[0] = 0;   affiche_cousins(g, irose, buf);
	printf("%s\n", buf);

	printf("\n******* les oncles et tantes:\n");
	printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ij)));
	buf[0] = 0;  affiche_oncles(g, ij, buf);
	printf("%s\n", buf);
	printf("Oncles/tantes de %s:\n", nomIndividu(getByIdent(g, ir)));
	buf[0] = 0;  affiche_oncles(g, ir, buf);
	printf("%s\n", buf);


	printf("\n******* les ancetres:\n");
	printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)), nomIndividu(getByIdent(g, ia2)), estAncetre(g, ijfl, ia2) ? "oui" : "non");
	printf("%s ancetre de %s: %s\n", nomIndividu(getByIdent(g, ijfl)), nomIndividu(getByIdent(g, irose)), estAncetre(g, ijfl, irose) ? "oui" : "non");

	printf("\n******* les ancetres communs:\n");
	printf("%s et %s ont ancetre commun: %s\n", nomIndividu(getByIdent(g, irose)), nomIndividu(getByIdent(g, ij)), ontAncetreCommun(g, irose, ij) ? "oui" : "non");
	printf("%s et %s ont ancetre commun: %s\n", nomIndividu(getByIdent(g, ih)), nomIndividu(getByIdent(g, ibill)), ontAncetreCommun(g, ih, ibill) ? "oui" : "non");

	printf("\n******* ancetre plus ancien:\n");
	printf("L'ancetre le plus ancien de %s est %s\n", nomIndividu(getByIdent(g, ia2)), nomIndividu(getByIdent(g, plus_ancien(g, ia2))));

	printf("\n******* parente:\n");
	printf("parents de %s\n", nomIndividu(getByIdent(g, ia2)));
	buf[0] = 0; affiche_parente(g, ia2, buf);
	printf("%s\n", buf);

	printf("\n******* descendence:\n");
	printf("descendence de %s\n", nomIndividu(getByIdent(g, ijm)));
	buf[0] = 0;  affiche_descendance(g, ijm, buf);
	printf("%s\n", buf);

	printf("\n******* free:\n");
	genealogieFree(&g);
	printf("fin.(press key)\n");
	fgets(buf, 2, stdin);
	return 0;
}

