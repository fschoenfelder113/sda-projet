// canevas.c   pour projet Genealogie SDA2 2024-25�
//

// SCHOENFLEDER FRANCK
///////////////////////////////////

#include "base.h"
#include <stdatomic.h>
#include <stdio.h>

// Tyes predefinis
#define LG_MAX  64

typedef struct s_date { Nat jour, mois, annee; } Date;

typedef Nat Ident;
#define omega  0

typedef struct s_individu {
	Car nom[LG_MAX];
	// a completer ici
        Ident id ;
        Ident idpere ;
        Ident idmere ;
        Date naiss ;
        Date deces ;
        Ident icadet ;
        Ident ifaine ;
} *Individu;

typedef struct s_genealogie {
	Individu* tab;				// tableau des individus tri�s par nom
	// a completer ici
        Nat nb_individus ;
        Ident id_cur ;
        Nat* rang ;
        Nat taille_max_tab ;
} *Genealogie;

// DEFINIR ICI VOS CONSTANTES

#define TAILLE_INIT 5

const Date date_null = {0, 0, 0} ;

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

// PARTIE 3: PROTOTYPES des operations imposees
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

Bool date_is_null(Date d) ;

Bool parent_compatible(Ident* pa, Ident* ma, Ident* pb, Ident* mb) ;

void ajouteBufferN(Chaine buff, Nat* n, Chaine str) ;
void affiche_freres_soeurs_n(Genealogie g, Ident x, Chaine buff,Nat *n) ;
void affiche_enfants_n(Genealogie g, Ident x, Chaine buff, Nat *n);
void affiche_neveux_n(Genealogie g, Ident x, Chaine buff, Nat *n);

Ident aine(Genealogie g, Ident id) ;

void remplirTabAncetreX(Genealogie g, Bool* tab, Ident x );
Bool parcourirTabAncetre(Genealogie g,Bool* tab, Ident y);

/*
lgen liste avec les générations
llien liste des elements liens
ptrilien pointeur de ilien index du tableau de lien (modifiable)
*/
void attache_gen_parente(Genealogie g,Ident x,Individu* lgen,Individu* llien, Nat* ptr_ilien) ;
void attache_gen_descendance(Genealogie g,Ident x,Individu* lgen,Individu* llien,Nat* ilien) ;
void lecture_gen(Genealogie g, Individu* lgen,Nat igen, Chaine buff) ;
void detache_gen(Individu* llien, Nat ilien) ;

Ident junior(Genealogie g, Ident x) ;

// attache en tete
Individu attache_gen(Genealogie g, Individu head, Individu tail, Individu* llien, Nat* ptr_ilien) ;

Nat hauteur_from(Genealogie g, Ident x) ;

Nat profondeur_from(Genealogie g, Ident x) ;

/// PARTIE 1: Construction de l�arbre g�n�alogique et acc�s 
/// ///////////////////////////////////////////////////////
/// 

//PRE: None
Ent compDate(Date d1, Date d2)
{
	Ent an = (d1.annee - d2.annee);
	Ent mo = (d1.mois - d2.mois);
	Ent jo = (d1.jour - d2.jour);

	if (an < 0)
		return -1 ;
	if (an == 0 && mo < 0)
		return -1 ;
	if (an == 0 && mo == 0 && jo < 0)
		return -1 ;
	if (an == 0 && mo == 0 && jo == 0) 
		return 0 ;
	return 1;
}

//PRE: None
void genealogieInit(Genealogie *g)
{
	// en cas d'echec d affectation des tableaux g vaudra NULL et on ne continue pas l init
	
	*g = MALLOC(struct s_genealogie);

	(*g)->tab = MALLOCN(Individu, TAILLE_INIT) ;
	if ((*g)->tab == NULL){
		*g = NULL ;
		return ;
	}

	(*g)->rang = MALLOCN(Nat, TAILLE_INIT) ;
	if ((*g)->rang == NULL){
		*g = NULL ;
		return ;
	}

	(*g)->nb_individus = 0 ;

	(*g)->id_cur = 0 ;

	(*g)->taille_max_tab = TAILLE_INIT ;
}

//PRE: None
void genealogieFree(Genealogie *g)
{
	if (*g == NULL) return ;
	for (Nat i =  0; i < (*g)->nb_individus ; i++){
		freeIndividu((*g)->tab[i]) ;
	}
	FREE((*g)->tab) ;
	FREE((*g)->rang) ;

	FREE(*g) ;
}

//PRE: None
Individu nouvIndividu(Ident i, Chaine s, Ident p, Ident m, Date n, Date d)
{
	Individu individu = MALLOC(struct s_individu) ;
	if (individu == NULL) return NULL ;

	individu->id = i ;
	chaineCopie(individu->nom, s) ;

	individu->idpere = p ;
	individu->idmere = m ;

	individu->naiss = n ;
	individu->deces = d ;

	individu->icadet = omega ;
	individu->ifaine = omega ;

	return individu ; 
}

//PRE: None
void freeIndividu(Individu id)
{
	FREE(id) ; // AKA individu
}

// Selecteur
// ////////////////////
Chaine nomIndividu(Individu ind) {
	if (ind != NULL)
		return ind->nom;
	return "" ;
}
Date naissIndividu(Individu ind) { 
	if (ind != NULL)
		return ind->naiss ;
	return date_null ;
}
Nat cardinal(Genealogie g) { 
	if (g != NULL)
		return g->nb_individus ; 
	return 0 ;
}
Individu kieme(Genealogie g, Nat k) { 
	if (g == NULL || k >= g->nb_individus)
		return NULL;
	return g->tab[k] ; // k out of range
}

//PRE: None
Individu getByIdent(Genealogie g, Ident i)
{
	if (g == NULL)
		return NULL ;
	// i est borné dans le tableau
	if (0 < i && i <= g->nb_individus){
		return g->tab[ g->rang[i-1] ] ;
	}
	return NULL ;
}

//PRE: None
Nat getPos(Genealogie g, Chaine name)
{
	if (g == NULL || g->nb_individus == 0 || name[0] == '\0')
		return 0 ;
	
	// Algo de dichotomie
	Ent milieu ;
	Ent min = 0 ;
	Ent max = g->nb_individus -1 ;
	
	Ent diff ;

	while (min <= max) {
		milieu = (min+max) / 2 ;
		diff = chaineCompare(name, (g->tab[milieu])->nom ) ;

		if ( diff < 0 )
			max = milieu-1 ;
		else if ( diff > 0)
			min = milieu +1 ;
		else if (min != max) // on a plus d un ellement
			max = milieu ;
			/*
			 * Le nom est le meme cependant on veut LE PREMIER individu avec ce nom
			 * Donc on définie le bon comme le max, sans le -1 pour le garder inclus
			 */
		else // min==max permet d eviter une boucle si on a un unique element
			max-- ;
	}
	return (Nat) min ;
}

//PRE: None
Individu getByName(Genealogie g, Chaine name, Date naissance)
{
	if (g == NULL || g->nb_individus == 0 || name[0] == '\0')
		return NULL ;

	Nat pos = getPos(g, name) ;
	/*
	 * On veut, à partir de la premier position donnée, chercher l indivdu avec le nom et la date qui correspond
	 * On retourne NULL si aucune corresondance
	 * Si naissance est nul, on cherche l individu le plus jeune et non ce lui a qui la date correspond
	 */

	Date date_saved = date_null ;
	Ent pos_saved = -1 ;
	Bool  correspondance = vrai ;

	// parcourir tab de pos à soit la fin du tableau, soit plus de correspondance
	Nat i = pos ;
	while (i < g->nb_individus && correspondance){
		Individu individu = g->tab[i] ;
		
		//si nom correspond
		if ((correspondance = (chaineCompare(individu->nom, name) == 0))){
		
			// si date correspond on retourne l individu
			if (compDate(individu->naiss, naissance) == 0){
				return individu ;
			}
			// sinon si la date est nulle
			else if (date_is_null(naissance)) {
				
				// si individu est plus jeune que la date sauvegardé
				if (compDate(individu->naiss, date_saved) > 0){
				
					// on sauvegarde sa position et sa date de naissance
					date_saved = individu->naiss ;
					pos_saved = i ;
				}
			}
		}
		i ++ ;
	}

	if (pos_saved == -1)
		return NULL ; // aucun individu ne correspond
	
	return g->tab[pos_saved]; // individu correspondant le plus jeune
}

//PRE: (pos>=1 => chaineCompare(g[pos-1]->nom,s)<=0) 
//     && (pos<g->nb_individus-1 => chaineCompare(g[pos+1]->nom,s)>=0)
void insert(Genealogie g, Nat pos, Chaine s, Ident p, Ident m, Date n, Date d)
{
	// Les préconditions vérifie que la position est la bonne
	if (g == NULL) return ;	// pas d insertion
	
	Individu nouv_idv = nouvIndividu(g->id_cur, s, p, m, n, d) ;

	g->nb_individus++ ;
	
	// le nouveau nombre d'individu dépasse le tableau
	if (g->nb_individus > g->taille_max_tab ) {
		
		Individu* nouv_tab ;		// on va réallouer les tableaux
		Ident* nouv_rang ;

		g->taille_max_tab *= 2 ;	// avcec la nouvelle taille max
		
		nouv_tab = REALLOC(g->tab, Individu, g->taille_max_tab) ;
		nouv_rang = REALLOC(g->rang, Nat, g->taille_max_tab) ;
		
		if (nouv_tab == NULL || nouv_rang == NULL){
			g->nb_individus-- ;
			return ;
		}
		g->rang = nouv_rang ;
		g->tab = nouv_tab ;
	}

	// insertion dans tab en deux temps :
	//	decaller tous les elements de pos à la fin
	//	inserer nouv_idv à pos
	for (Nat i = g->nb_individus-1 ; i > pos ; i--){
		g->tab[i] = g->tab[i-1] ;
	}
	g->tab[pos] = nouv_idv ;

	// insertion dans le tableau de rang
	// ATTENTION : i l insertion affecte tous les autres élements : le rang des elements placés apres doit etre incrémenté
	for (Nat i = 0 ; i < g->nb_individus -1 ; i++){
		if (g->rang[i] >= pos)
			g->rang[i]++ ;
	}
	g->rang[g->id_cur-1] = pos ;
}

// PRE: getByIdent(g,x)!=NULL) && getByIdent(g,filsa)!=NULL &&  (pp!=omega || mm!=omega)
void adjFils(Genealogie g, Ident idx, Ident fils, Ident pp, Ident mm)
{
	// précondiution vérifie que idx ET FILS sont inséré dans le tableau et qu il a un parent minimum
	// on doit vérifier que les parents données sont compatibles avec ceux fils
	Individu idv_x = getByIdent(g, idx) ;
	Individu idv_fils = getByIdent(g, fils) ;
	

	// la fonction vérifie la compatibilité des 4 parents met l union dans pere et mere en cas de succes, on fait ceci pour tous
	// on veut aussi vérifier que le fils ajouté est bien plus jeune que les parents

	Ident p = pp ; Ident m = mm ; // inutile
	if (!parent_compatible(&p, &m, &idv_x->idpere, &idv_x->idmere)) return ;
	if (!parent_compatible(&idv_fils->idpere, &idv_fils->idmere, &idv_x->idpere, &idv_x->idmere)) return ;

	if (idv_x->idpere != omega && compDate( getByIdent(g, idv_x->idpere)->naiss, idv_x->naiss) > 0) return ;
	if (idv_x->idmere != omega && compDate( getByIdent(g, idv_x->idmere)->naiss, idv_x->naiss) > 0) return ;

	// on parcours la liste des enfants, on pense à enregistré l idv avant celui trop jeune pour l insertion
	// on va insrerr idx dans la liste chainée dont filsa est la tete et mettre a jour les parents par la meme occastion

	Individu courant = idv_fils ;
	Individu precedant = NULL ;
	Individu point_insert = NULL ;

	if(compDate(idv_x->naiss, idv_fils->naiss) < 0){
		// cas ou le point d insertion est avant l ainée (nouvelle tete de liste)
		// insertion entre le ptn d insertion et son cadet
		// attacher en tete
		idv_x->icadet = fils ;
		// mettre a jour parents
		if (idv_x->idpere != omega) getByIdent(g, idv_x->idpere)->ifaine = idx ;
		if (idv_x->idmere != omega) getByIdent(g, idv_x->idmere)->ifaine = idx ;
		return  ;
	}
	while(courant != NULL){
		// mettre a jour les parents de la fratrie defaut partiel de complexité en allant jusqu au bout, mais ca reste du meme ordre n**2
		if (courant->idpere == idv_x->idpere) courant->idpere = idv_x->idpere;
		if (courant->idmere == idv_x->idmere) courant->idmere = idv_x->idmere;
		// si on a pas trouvé idv on chercher a affecter le point d insertion
		if (point_insert == NULL){
			if (compDate(idv_x->naiss, courant->naiss) < 0)
			point_insert = precedant ;
		}
		precedant = courant ;
		courant = getByIdent(g, courant->icadet) ;
	}
	// si  c est le plus jeune ilo n a pas trouvé de point d insertion
	if(point_insert == NULL){
		point_insert = precedant ;
	}

	// on peut insérer idx
	Ident tmp_cadet = point_insert->icadet ;
	point_insert->icadet = idx ;
	idv_x->icadet = tmp_cadet ;
		if (idv_x->idpere != omega) getByIdent(g, idv_x->idpere)->ifaine = fils ;
		if (idv_x->idmere != omega) getByIdent(g, idv_x->idmere)->ifaine = fils ;
}

//PRE:  (p==omega || getByIdent(g,p)!=NULL) && (m==omega || getByIdent(g,m)!=NULL) &&
//      !freres_soeurs(g,p,m) && getByIdent(g,p)->ifaine == getByIdent(g,m)->ifaine &&
//      compDate(getByIdent(g,p)->naiss,n)<0 && compDate(getByIdent(g,m)->naiss,n)<0
Ident adj(Genealogie g, Chaine s, Ident p, Ident m, Date n, Date d)
{
	// valeurs interdites
	if (s == NULL || s[0] == 0 || n.jour == 0 || n.mois == 0 || n.annee == 0) return omega;
	// précondition vérifie que :
	//	pere et mere soit existe soit valent omega
	//	si exsite; ne sont pas frere et soeur et ont le meme enfant ainée
	//	si existent qui ils soient plus agé que le fils
	
	if (getByName(g, s, n) != NULL){
		return omega ;
	}

	// recuperer position
	Nat pos = getPos(g, s) ;

	// ajout en tableau et incrémentation de l id cur
	g->id_cur++ ;
	insert(g, pos, s, p, m, n, d) ;
	
	// ajout dans la généalogie
	//	A SI aucun parent alors rien a faire
	//	B SINON SI parent(s) n ont pas de fils ainee alors nouveau devient l'ainée des deux
	//	C SINON recuperer l ainée de n importe quel parent(d apres la precond ils ont le meme ou aucun) et adj fils
	if (p == omega && m == omega){
		// A ne rien faire
	}else{
		Ident filsa = (p == omega) ?  getByIdent(g, m)->ifaine : getByIdent(g, p)->ifaine  ;
		if (filsa == omega){
			// B
			if (p != omega) getByIdent(g, p)->ifaine = g->id_cur ;
			if (m != omega) getByIdent(g, m)->ifaine = g->id_cur ;
		} else {
			// C
			adjFils(g, g->id_cur, filsa, p, m) ;
		}
	}
	return g->id_cur ;
}



// 
/// PARTIE 2: Affichages 
/// ///////////////////////////////////////////////////////
/// 

//PRE: None
void affiche_freres_soeurs(Genealogie g, Ident x, Chaine buff)
{
	if (g == NULL || getByIdent(g, x) == NULL) return ;
	
	Nat n = 0 ;
	affiche_freres_soeurs_n(g, x, buff, &n) ;
}

//PRE: None
void affiche_enfants(Genealogie g, Ident x, Chaine buff)
{
	if (g == NULL || getByIdent(g, x) == NULL) return ;
	
	Nat n = 0 ;
	affiche_enfants_n(g, x, buff, &n) ;
}

//PRE: None
void affiche_cousins(Genealogie g, Ident x, Chaine buff)
{
	if (g == NULL || getByIdent(g, x) == NULL) return ;
	
	Nat n = 0 ;
	affiche_neveux_n(g, getByIdent(g, x)->idpere, buff, &n) ;
	affiche_neveux_n(g, getByIdent(g, x)->idmere, buff, &n) ;
}

//PRE: None
void affiche_oncles(Genealogie g, Ident x, Chaine buff)
{
	if (g == NULL || getByIdent(g, x) == NULL) return ;
	
	Nat n = 0 ;
	affiche_freres_soeurs_n(g, getByIdent(g, x)->idpere, buff, &n) ;
	affiche_freres_soeurs_n(g, getByIdent(g, x)->idmere, buff, &n) ;
}

//
/// PARTIE 3
/////////////////////////////////////////////////////////////
///

void deviennent_freres_soeurs(Genealogie g, Ident x, Ident y) {
	if (g == NULL || x == omega || y  == omega) return;

	Individu ix = getByIdent(g, aine(g, x) );
	Individu iy = getByIdent(g, aine(g, y) );

	// Vérifier et unifier les parents
	if (!parent_compatible(&ix->idpere, &ix->idmere, &iy->idpere, &iy->idmere))
		return;  // parents incompatibles et fusionées sur x

	// verifier que les aprents sont bien plus jeunes
	if (ix->idmere != omega && compDate( getByIdent(g, ix->idmere)->naiss, iy->naiss ) > 0) return ;
	if (ix->idpere != omega && compDate( getByIdent(g, ix->idpere)->naiss, iy->naiss ) > 0) return ;

	Ident ys = iy->id ;
	Ident yc = ys ;
	while(ys != omega){
		ys = getByIdent(g, ys)->icadet ;
		adjFils(g, yc, ix->id, getByIdent(g, x)->idpere, getByIdent(g, x)->idmere) ;
		yc = ys ;
	}
}

void devient_pere(Genealogie g, Ident x, Ident y){
	if (x == y) return ;
	// si x ou g omega impossible
	if(g == NULL) return ;
	Individu idv_x = getByIdent(g, x) ;
	Individu idv_y = getByIdent(g, y) ;
	if (idv_x == NULL || idv_y == NULL) return ;
	if (compDate( idv_x->naiss, idv_y->naiss ) > 0) return ;

	// si oncompatible se n est pas possible
	// verifier compatibilité sans unification
	Ident m = omega ; Ident p  = x ;
	Ident py  = idv_y->idpere ; Ident my = idv_y->idmere ;
	if (!parent_compatible(&py, &my, &p, &m )) return ;

	if (idv_x->ifaine != omega){
		deviennent_freres_soeurs(g, idv_x->ifaine, y) ;
	
	} else {
		
		// parent n ont pas d enfant donc on rajoute juste x en aine
		getByIdent(g, p)->ifaine = aine(g, y) ;
		if (m != omega) getByIdent(g, m)->ifaine = aine(g, y) ; 
	}
}

void devient_mere(Genealogie g, Ident x, Ident y){
	if (x == y) return ;
	// si x ou g omega impossible
	if(g == NULL) return ;
	Individu idv_x = getByIdent(g, x) ;
	Individu idv_y = getByIdent(g, y) ;
	if (idv_x == NULL || idv_y == NULL) return ;
	if (compDate( idv_x->naiss, idv_y->naiss ) > 0) return ;

	// si oncompatible se n est pas possible
	// verifier compatibilité sans unification
	Ident m = x ; Ident p  = omega ;
	Ident py  = idv_y->idpere ; Ident my = idv_y->idmere ;
	if (!parent_compatible(&py, &my, &p, &m )) return ;

	if (idv_x->ifaine != omega){
		deviennent_freres_soeurs(g, idv_x->ifaine, y) ;
	
	} else {
		
		// parent n ont pas d enfant donc on rajoute juste x en aine
		getByIdent(g, m)->ifaine = aine(g, y) ;
		if (p != omega) getByIdent(g, p)->ifaine = aine(g, y) ; 
	}
}

//
/// PARTIE 4: Parcours de la genealogie
/// ///////////////////////////////////////////////////////
///

// x ancetre de y ?
// PRE: None
Bool estAncetre(Genealogie g, Ident x, Ident y) {
        // on suivra des algo recursifs
	if (x == y) return vrai ;
	if (g == NULL || y == omega) return faux ;
	if (x == omega) vrai ;
 
	Individu idv = getByIdent(g, y);
        Ident p = idv->idpere;
        Ident m = idv->idmere; 

        if (p == x) return vrai ;
        if (m == x) return vrai ;

	if (p == omega) return faux ;
	if (m == omega) return faux ;

        return estAncetre(g, x, p) || estAncetre(g, x, m) ;
}
// PRE: None
Bool ontAncetreCommun(Genealogie g, Ident x, Ident y) {
 // fonction en plusieurs etapes :
        // creer un tableau vontigue de Booleen composé du nombre totale de g+1 et tout mettre a 0 ou faux
        // parcourir x  et mettre vrai a chaque identifiant de parent recusivmement -> fonction a coté
        // ensuite on parcour y et des qu on trouve un ancetre a case qui donné on stop

	if (x == y) return vrai ;
        if (g == NULL || getByIdent(g,x) == NULL || getByIdent(g, y) == NULL) return faux ;

        Bool* tab_ancetre = CALLOCN( Bool, g->nb_individus+1 ) ; // +1 pour eviter futurs operations ;
        //calloc met a 0 (faux) 

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

// PRE : NONE
void affiche_parente(Genealogie g, Ident x, Chaine buff){
	if (g == NULL) return ;
	Individu idv ;
	if ((idv = getByIdent(g,x)) == NULL) return ;

	Nat hauteur = hauteur_from(g, x) ;

	Nat ilien = 0 ;
	Individu* lgen = CALLOCN(Individu, hauteur-1) ;
	Individu* llien = CALLOCN(Individu, g->nb_individus) ;

	attache_gen_parente(g, x, lgen, llien, &ilien) ;

	lecture_gen(g, lgen, hauteur-1, buff) ;

	detache_gen(llien, ilien) ;

	FREE(lgen) ; FREE(llien) ;
}

// PRE: None
void affiche_descendance(Genealogie g, Ident x, Chaine buff){
	if (g == NULL) return ;
	Individu idv ;
	if ((idv = getByIdent(g,x)) == NULL) return ;

	Nat profondeur = profondeur_from(g, x) ;
	Nat ilien = 0 ;
	Individu* lgen = CALLOCN(Individu, profondeur-1) ;
	Individu* llien = CALLOCN(Individu, g->nb_individus) ;

	attache_gen_descendance(g, x, lgen, llien, &ilien) ;

	lecture_gen(g, lgen, profondeur-1, buff) ;

	detache_gen( llien, ilien) ;

	FREE(lgen) ; FREE(llien) ;
}


// 
/// VOS FONCTIONS AUXILIAIRES 
/// ///////////////////////////////////////////////////////
/// 

Bool date_is_null(Date d){
	return d.annee==0 && d.jour==0 && d.mois==0 ;
}

Bool parent_compatible(Ident* pa, Ident* ma, Ident* pb, Ident* mb) {

	// si aucunu parent : incompatible pour nous
	if (*pa + *pb + *ma + *mb == 0) return faux ;

	// Vérifie la compatibilité des pères
	Bool compat_pere = (*pa == omega || *pb == omega || *pa == *pb);
	// Vérifie la compatibilité des mères
	Bool compat_mere = (*ma == omega || *mb == omega || *ma == *mb);

	if (compat_pere && compat_mere) {
		// Fusionner les valeurs : si un parent est omega, on copie l'autre
		if (*pa == omega) *pa = *pb;
		if (*pb == omega) *pb = *pa;

		if (*ma == omega) *ma = *mb;
		if (*mb == omega) *mb = *ma;
		return vrai;
	}
	return faux;
}

void ajouteBufferN(Chaine buff, Nat* n, Chaine str){
        chaineCopie(buff+*n, str) ;
        *n += chaineLongueur(str) ;
}

// PRE: None
void affiche_freres_soeurs_n(Genealogie g, Ident x, Chaine buff,Nat *n) {
        // pas de securité sur la taille du buffer, mais il aurait fallue un type [] pour que ce soit simple

        // pour recuperer tous les freres et soeur, on accède son parent. et on les ajoute au buf
        if (x == omega) return ; // pas de frere et soeur

        Ident ifils  = aine(g, x);
        Individu idv = getByIdent(g, x) ;
        
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

Ident aine(Genealogie g, Ident id){
	Individu idv ;
	if (g == NULL || (idv = getByIdent(g, id)) == NULL) return omega ;

	// regarder parents, si pas de parents omega 
	if ( idv->idpere == omega && idv->idmere == omega ) return id ;
	if (idv->idpere != omega) 
		return getByIdent(g, idv->idpere)->ifaine ;
	else 
		return getByIdent(g,idv->idmere)->ifaine ;
}

void remplirTabAncetreX(Genealogie g, Bool* tab, Ident x ){
	//if (g == NULL || tab == NULL || x == omega ) return ;
	Individu idv = getByIdent(g, x) ;
        if (idv == NULL) return ;

        tab[ x ] = vrai ; // il existe dans les ancetres

        Ident p = idv->idpere ;
        Ident m = idv->idmere ;


	if ( p != omega)
		remplirTabAncetreX(g, tab, p) ;
        if (m != omega)
		remplirTabAncetreX(g, tab, m) ;
}

Bool parcourirTabAncetre(Genealogie g,Bool* tab, Ident y){
        Individu idv = getByIdent(g, y) ;
        if (idv == NULL) return faux;

        if (tab[y]) return vrai; // on s arrete si trouvé

        Ident p = idv->idpere ;
        Ident m = idv->idmere ;

        return parcourirTabAncetre(g, tab, p) || parcourirTabAncetre(g, tab, m) ;
}


void attache_gen_parente(Genealogie g, Ident x, Individu* lgen,Individu* llien,Nat *ptr_ilien){
	if (x == omega) return ;

	Individu idv = getByIdent(g, x);
	Ident am = aine(g, idv->idmere);
	Ident ap = aine(g, idv->idpere);

	if (ap == omega && am == omega) return ;

	// adjonction de la gratrie des parent a la liste des générations actuelle
	if (am != omega) lgen[0] = attache_gen(g, getByIdent(g, am), lgen[0], llien, ptr_ilien) ;
	if (ap != omega) lgen[0] = attache_gen(g, getByIdent(g, ap), lgen[0], llien, ptr_ilien) ;

	attache_gen_parente(g, idv->idmere, lgen+1, llien, ptr_ilien) ;
	attache_gen_parente(g, idv->idpere, lgen+1, llien, ptr_ilien) ;
}

void attache_gen_descendance(Genealogie g, Ident x, Individu* lgen,Individu* llien,Nat *ptr_ilien){
	if (x == omega) return ;

	Individu idv = getByIdent(g, x) ;

	/*
	 * Probleme : l attache empeche le parcour des fils sans creer une bouclle
	 * */

	// rien a ajouté si pas d enfant sinon ajo
	if (idv->ifaine == omega) return ;
	lgen[0] = attache_gen(g, getByIdent(g, idv->ifaine), lgen[0], llien, ptr_ilien) ;
	Ident fils = idv->ifaine ;
	
	while (fils != omega){
		attache_gen_descendance(g, fils, lgen+1, llien, ptr_ilien) ;
		fils = getByIdent(g, fils)->icadet ;
	}
}

void lecture_gen(Genealogie g, Individu* lgen,Nat igen, Chaine buff) {
	Nat n = 0 ;
	Chaine natbuf = MALLOCN(Car, 16) ;
	Ident x ;

	for (Nat i = 0 ; i < igen ; i++){
		x = lgen[i]->id ;
		
		ajouteBufferN(buff, &n, "- ") ;
		NatToChaine(i+1, natbuf) ;
		ajouteBufferN(buff, &n, natbuf) ;
		ajouteBufferN(buff, &n, " :\n") ;

		while(x != omega){
			ajouteBufferN(buff, &n, getByIdent(g, x)->nom ) ;
			ajouteBufferN(buff, &n, " ") ;
			x = getByIdent(g, x)->icadet ;
		}

		ajouteBufferN(buff, &n, "\n") ;
	}
	FREE(natbuf) ;
}

void detache_gen(Individu* llien, Nat ilien) {
	// parcourir les idv et mettre leur cadet a omega
	for(Nat i = 0 ; i < ilien ; i ++){
		llien[i]->icadet = omega ;
	}
}

Ident junior(Genealogie g, Ident x){
	if (g == NULL || x == omega) return omega ;
	Individu idv = getByIdent(g, x) ;
	while (idv->icadet != omega){
		idv = getByIdent( g, idv->icadet ) ;
	}
	return idv->id ;
}

// attache en tete
Individu attache_gen(Genealogie g, Individu head, Individu tail, Individu* llien, Nat* ptr_ilien){
	
	Individu j = getByIdent(g, junior(g, head->id));
	
	if (tail == NULL)
		j->icadet = omega ; 
	else 
		j->icadet = tail->id ;
	llien[*ptr_ilien] = j ;
	*ptr_ilien += 1 ;
	return head ;
}


Nat hauteur_from(Genealogie g, Ident x) {
	if (x == omega) return 0 ;
	Nat hg = 1 + hauteur_from(g, getByIdent(g,x)->idpere) ;
	Nat hd = 1 + hauteur_from(g, getByIdent(g,x)->idmere) ;
	
	if (hg > hd) return hg ;
	else return  hd ;
}

Nat profondeur_from(Genealogie g, Ident x) {
	if (x == omega) return 0 ;
	// comparer profondeur avec ses cadet
	if (x == omega) return 0;  // Aucun individu

	Individu idv = getByIdent(g, x);
	if (idv == NULL) return 0;

	// Si l’individu n’a pas de descendant, il est une feuille
	if (idv->icadet == omega && idv->ifaine == omega) return 1;

	// Parcours de la fratrie pour trouver la plus grande hauteur
	Nat h_max = 0;
	Ident enfant = idv->ifaine;  // Premier enfant connu

	while (enfant != omega) {
		Nat h_fils = profondeur_from(g, enfant);
		if (h_fils > h_max) h_max = h_fils;
		enfant = getByIdent(g, enfant)->icadet;  // Passer au cadet
	}

	return 1 + h_max;  // Ajout de la génération actuelle
}

// a supprimer
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

        printf("\nRang : \n") ;
        for (Nat i = 0 ; i < cardinal(g) ; i ++){
                printf("%2d\n", g->rang[i]) ;
        } 
}


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
        /*Ident il = */adj(g, "Lily", ih, ig, ln, dnull) ;

        Date fgn = { 1, 4, 1978 } ;
        Date geord = {5, 6, 1998} ;
        /*Ident ifred = */adj(g, "Fred", iaw, imw, fgn, geord) ;  
        /*Ident igeor = */adj(g, "George", iaw, imw, fgn, dnull) ;  
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
