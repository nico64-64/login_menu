#include "menu.h"


int taille_nbre (int nbre)
//Trouve le nombre de caracteres occupes par le nombre recu en parametre.
{
	unsigned compteur = 0;
	int n = nbre;

	if (n < 0)
	{compteur++; n *= -1;}
	for (; n >= 10; compteur++)
	{n /= 10;}

	return compteur;
}


void quitter ()
//Ferme le programme en liberant les ressources nesessaires et en reinitialisant le terminal.
{
	endwin();
	if (options != NULL)
	{free(options);}
	exit(0);
}


void erreur (char msg[], char details[], int valeur)
//Affiche un message d'erreur.
{
	rafraichir();
	mvprintw(LINES - 5, (COLS - strlen(msg) - strlen("Erreur: ")) / 2, "Erreur: %s", msg);
	mvprintw(LINES - 4, (COLS - strlen(details) - strlen("Details: ")) / 2, "Details: %s", details);
	mvprintw(LINES - 3, (COLS - taille_nbre(valeur) - strlen("Valeur: ")) / 2, "Valeur: %d", valeur);
}


void gestion_arguments (char arg[])
//Gere les arguments recus par le programme.
{
	static bool fconfig_en_attente = FALSE;
	static bool espacement_en_attente = FALSE;

	if (espacement_en_attente)
	{
		if (!strcmp(arg, "0"))
		{espacement = 0;}
		else if (!strcmp(arg, "1")) {} //par defaut
		else if (!strcmp(arg, "2"))
		{espacement = 2;}
		else if (!strcmp(arg, "3"))
		{espacement = 3;}
		else if (!strcmp(arg, "4"))
		{espacement = 4;}
		else if (!strcmp(arg, "5"))
		{espacement = 5;}
		else if (!strcmp(arg, "6"))
		{espacement = 6;}
		else
		{printf("L'espacement fourni en argument n'est pas valide.\n"); sleep(2);}
		espacement_en_attente = FALSE;
	}

	else if (fconfig_en_attente)
	{strcpy(nom_fichier, arg); fconfig_en_attente = FALSE;}

	else if (!strcmp(arg, "--version") || !strcmp(arg, "-v"))
	{printf("version 1.3\n"); exit(0);}

	else if (!strcmp(arg, "--help") || !strcmp(arg, "-h") || !strcmp(arg, "--aide") || !strcmp(arg, "-a") || !strcmp(arg, "-?"))
	{
		printf("Menu de connexion (pre-login)\n\n");
		printf("Usage:\n/sbin/login_menu [--arguments]\n\n");

		printf("Ce petit programme génère un menu ncurses permettant de choisir de quelle manière se connecter a l'ordinateur.\n");
		printf("Il est normalement appelé par systemd en tant que \"login manager\".\n\n");

		printf("Voici la liste des arguments que peut recevoir ce programme:\n");
		printf("--version (-v) -> Affiche la version du programme et quitte.\n");
		printf("--aide (-?) -> Affiche ce texte et quitte.\n");
		printf("--quittable (-q) -> Permet de quitter le programme en entrant \"q\".\n");
		printf("--espacement (-e) -> Indique que le prochain argument sera le nombre de lignes vides a laisser entre chaque option (de 0 a 6 seulement).\n");
		printf("--fconfig (-f) -> Indique que le prochain argument sera le chemin d'acces au fichier de configuration (sans espace!).\n");

		exit(0);
	}

	else if (!strcmp(arg, "--quittable") || !strcmp(arg, "--quitable") || !strcmp(arg, "-q"))
	{quittable = TRUE;}

	else if (!strcmp(arg, "--espacement") || !strcmp(arg, "-e"))
	{espacement_en_attente = TRUE;}

	else if (!strcmp(arg, "--fconfig") || !strcmp(arg, "--config") || !strcmp(arg, "-f"))
	{fconfig_en_attente = TRUE;}
}


bool lecture_fichier ()
//Lit et applique le fichier de configuration du programme.
//Renvoie TRUE en cas de reussite et FALSE en cas d'echec.
{
	FILE* fconfig = fopen(nom_fichier, "r");
	char ligne[200];
	char* mot;
	bool pas_de_rafraichissement = FALSE;

	if (fconfig == NULL)
	{rafraichir(); quittable = TRUE; erreur("Impossible d'ouvrir le fichier de configuration!", (char*) nom_fichier, 0); return FALSE;}

	//Trouve le nombre d'options inscrites dans le fichier:
	nbre_options = 0;
	while (fgets(ligne, sizeof(ligne), fconfig) != NULL)
	{
		mot = strtok(ligne, " \t\n");
		if (mot != NULL && !strcmp(mot, "["))
		{nbre_options++;}
	}!
	fclose(fconfig);

	if (!nbre_options)
	{rafraichir(); quittable = TRUE; erreur("Le fichier de configuration ne contient aucune option valide!", (char*) nom_fichier, 0); return FALSE;}

	//Initialise l'array des options:
	options = calloc(nbre_options, sizeof(struct option));

	//Lit les options du fichier:
	fconfig = fopen(nom_fichier, "r");
	for (unsigned c = 0; c < nbre_options; )
	{
		fgets(ligne, sizeof(ligne), fconfig);
		mot = strtok(ligne, " \t\n");
		if (mot != NULL && !strcmp(mot, "["))
		{
			//nom:
			mot = strtok(NULL, " \t\n");
			if (mot == NULL || !strcmp(mot, ";") || !strcmp(mot, ",") || !strcmp(mot, ":") || !strcmp(mot, ":") || !strcmp(mot, "]"))
			{rafraichir(); quittable = TRUE; erreur("Le fichier de configuration contient une option sans nom! (valeur = numero d'option)", (char*) nom_fichier, c); return FALSE;}
			strcpy(options[c].nom, mot);
			mot = strtok(NULL, " \t\n");
			while (mot != NULL && strcmp(mot, ";") != 0 && strcmp(mot, ",") != 0 && strcmp(mot, ":") != 0)
			{strcat(options[c].nom, " "); strcat(options[c].nom, mot); mot = strtok(NULL, " \t\n");}
			if (!c || strlen(options[c].nom) > strlen(options[c-1].nom))
			{longueur_sel = strlen(options[c].nom) + 4;}

			//cmd:
			mot = strtok(NULL, " \t\n");
			if (mot == NULL || !strcmp(mot, ";") || !strcmp(mot, ",") || !strcmp(mot, ":"))
			{rafraichir(); quittable = TRUE; erreur("Le fichier de configuration contient une option sans commande! (valeur = numero d'option)", (char*) nom_fichier, c); return FALSE;}
			strcpy(options[c].cmd, mot);
			mot = strtok(NULL, " \t\n");
			while (mot != NULL && strcmp(mot, ";") != 0 && strcmp(mot, ",") != 0 && strcmp(mot, ":") != 0)
			{strcat(options[c].cmd, " "); strcat(options[c].cmd, mot); mot = strtok(NULL, " \t\n");}

			//final:
			mot = strtok(NULL, " \t\n");
			if (mot == NULL || (strcmp(mot, "FALSE") != 0 && strcmp(mot, "TRUE") != 0))
			{rafraichir(); erreur("Le fichier de configuration contient une option qui n'indique pas comment elle doit etre executee! (valeur = numero d'option)", (char*) nom_fichier, c); options[c].final = FALSE; pas_de_rafraichissement = TRUE;}
			else
			{
				if (!strcmp(mot, "FALSE"))
				{options[c].final = FALSE;}
				else
				{options[c].final = TRUE;}
			}

			c++;
		}
	}

	fclose(fconfig);
	if (!pas_de_rafraichissement)
	{rafraichir();}
	return TRUE;
}


void rafraichir ()
//Affiche le menu a l'ecran.
{
	time_t heure_raw;
	struct tm* heure;

	erase();
	debut_liste = (LINES - nbre_options * (espacement + 1)) / 2;

	//Date et heure:
	time(&heure_raw);
	heure = localtime(&heure_raw);
	strftime(date_heure, sizeof(date_heure), " %A le %d %B %Y, %H:%M ", heure);

	//Contours:
	mvhline(0, 1, ACS_HLINE, COLS - 2);
	mvhline(LINES - 1, 1, ACS_HLINE, COLS - 2);
	mvvline(1, 0, ACS_VLINE, LINES - 2);
	mvvline(1, COLS - 1, ACS_VLINE, LINES - 2);
	mvaddch(0, 0, ACS_ULCORNER);
	mvaddch(0, COLS - 1, ACS_URCORNER);
	mvaddch(LINES - 1, 0, ACS_LLCORNER);
	mvaddch(LINES - 1, COLS - 1, ACS_LRCORNER);
	mvaddstrc(0, " Connexion ");
	mvaddch(0, COLS - strlen(nom_tty) - 3, ' ');
	addstr(nom_tty);
	mvaddstr(0, 1, date_heure);

	//Options:
	for (unsigned c = 0; c < nbre_options; c++)
	{
		if (choix == c)
		{attrset(COLOR_PAIR(10)); mvhline(debut_liste + (espacement + 1) * c, (COLS - longueur_sel) / 2, ' ', longueur_sel);}
		mvaddstrc(debut_liste + (espacement + 1) * c, options[c].nom);
		if (choix == c)
		{standend();}
	}

	//Affichage:
	refresh();
}


int main (int argc, char* argv[])
//S'occupe de l'initialisation du programme et de la main loop.
{
	int input;


	//Gestion des arguments:
	for (unsigned c = 1; c < argc; c++)
	{gestion_arguments(argv[c]);}

	//Création d'une locale de base si on n'en trouve pas une:
	if (!setlocale(LC_ALL, ""))
	{setlocale(LC_ALL, "C.UTF-8");}

	//Initialisation "graphique":
	initscr();
	raw();
	nonl();
	keypad(stdscr, TRUE);
	noecho();
	start_color();
	init_pair(10, COLOR_BLACK, COLOR_WHITE);
	curs_set(0);

	//Trouve le nom du tty:
	strcpy(nom_tty, ttyname(STDIN_FILENO));
	for (unsigned c = 5; nom_tty[c-1] != '\000'; c++)
	{nom_tty[c-5] = nom_tty[c];}
	if (nom_tty[3] == '/')
	{
		for (unsigned c = 3; nom_tty[c] != '\000'; c++)
		{nom_tty[c] = nom_tty[c+1];}
	}

	//Initialisation "logique":
	if (!lecture_fichier())
	{/*panique!*/ getch(); quitter();}


	//Main Loop:
	while (1)
	{
		do
		{input = getch();} while (input == ERR);

		switch (input)
		{
		//Resize (ne devrait jamais arriver):
		case KEY_RESIZE:
			rafraichir();
			break;

		//Descendre dans le menu:
		case KEY_DOWN:
			if (choix < nbre_options - 1)
			{choix++;}
			rafraichir();
			break;

		//Monter dans le menu:
		case KEY_UP:
			if (choix > 0)
			{choix--;}
			rafraichir();
			break;

		//Quitter:
		case 'q':
		case 'Q':
			if (quittable)
			{quitter();}
			break; //inutile

		//Choisir une option
		case 13: //enter
			if (options[choix].final)
			{
				endwin();
				execl("/bin/sh", "sh", "-c", options[choix].cmd, NULL);
				erreur("execl n'a pas pu executer la commande choisie!", options[choix].cmd, errno);
			}
			else //option != finale
			{system(options[choix].cmd);}
			break;
		}
	}
}