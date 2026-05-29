#ifndef _MENU_H
#define _MENU_H

//Structure interne:
struct option
{
	char nom[100]; //nom de l'option (sera affiché dans le menu)
	char cmd[500]; //commande à exécuter
	bool final; //est-ce que le programme doit continuer après cette commande (FALSE) ou pas (TRUE)
};

//Fonctions:
void erreur(char[], char[], int);
void gestion_arguments(char[]);
bool lecture_fichier();
int main(int, char*[]);
void quitter();
void rafraichir();
int taille_nbre(int);

#define mvaddstrc(position_y, texte);	mvaddstr(position_y, (COLS - strlen(texte)) / 2, texte); //Affiche une string centrée en x

#endif
