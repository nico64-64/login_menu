#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <errno.h>


#define VERSION "1.4"


#define mvaddstrc(position_y, texte);	mvaddstr(position_y, (COLS - strlen(texte)) / 2, texte); //Affiche une string centrée en x


//Structure interne:
struct option
{
	char nom[100]; //nom de l'option (sera affiché dans le menu)
	char cmd[500]; //commande à exécuter
	bool final; //est-ce que le programme doit continuer après cette commande (FALSE) ou pas (TRUE)
};


struct option* options = NULL;

unsigned nbre_options = 0; //nombre d'options lues dans le fichier de configuration
unsigned longueur_sel; //longueur de la "barre" de sélection (déterminée par la longueur du plus long nom d'option)
unsigned debut_liste; //position en y du 1er choix de la liste (mis à jour par rafraichir())
unsigned choix = 0; //option présentement sélectionnée
char nom_tty[20]; //nom du tty (tty1, pts0, etc.)
char date_heure[100]; //date et heure actuelle


//Parametres modifiables à l'invocation:
unsigned espacement = 1; //nombre de lignes vides entre chaque option
bool quittable = FALSE; //indique si on peut quitter en entrant 'q'
bool souris = FALSE; //active le support de la souris
char nom_fichier[100] = "/etc/login_menu.conf"; //chemin d'accès et nom du fichier de configuration du programme


//Fonctions:
void erreur(char[], char[], int);
void gestion_arguments(char[]);
bool lecture_fichier();
int main(int, char*[]);
void quitter();
void rafraichir();
int taille_nbre(int);