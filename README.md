# login_menu
Menu de connexion TUI pour Linux.

Ce menu est destiné à s'afficher avant le login manager (agetty, la plupart du temps). Il permet aux utilisateurs d'effectuer quelques actions de base sans avoir à se connecter, mais aussi de choisir quel login manager utiliser si on en a plusieurs.

Ce programme ne devrait pas être utilisé dans un contexte où la sécurité du login est vraiment importante; voir la section Sécurité.

## Installation
Vous devez d'abord installer les dépendances suivantes:
- un compilateur C (gcc ou clang)
- les en-têtes de développement de ncurses, préférablement la version wide-characters (cherchez un paquet dont le nom ressemble à libncursesw-dev)
- systemd (pour le démarrage automatique)
- git et un programme install compatible BSD (facultatifs; seulement pour suivre les instructions)

Suivez les instructions suivantes pour installer correctement le programme:

1. Clonez ce repo
```bash
git clone https://github.com/nico64-64/login_menu.git
cd login_menu
```

2. Compilez le programme
```bash
gcc menu.c -o login_menu -lncursesw
```

3. Installez le fichier de configuration par défaut
```bash
sudo install -vm644 -o0 -g0 login_menu.conf /etc
```

4. Installez le programme lui-même
```bash
sudo install -vm755 -o0 -g0 login_menu /sbin
```

### Démarrage automatique avec systemd
5. Installez le service systemd fourni
```bash
sudo install -vm644 -o0 -g0 login_menu@.service /etc/systemd/system
```

6. Activez le service systemd sur le ou les tty de votre choix
```bash
systemctl enable login_menu@tty1 #remplacez tty1 par les tty voulus
```

7. Redémarrez votre PC ou exécutez les commandes suivantes pour que systemd démarre le programme sur les tty spécifiés
```bash
systemctl daemon-reload
systemctl start login_menu@tty1 #remplacez tty1 par les tty voulus
```

## Usage
Si vous avez suivi les instructions pour le démarrage automatique avec systemd, aucune autre action n'est requise.

Sinon, vous pouvez démarrer le programme manuellement depuis un terminal avec `sudo /sbin/login_menu`. Le programme est installé dans /sbin, qui n'est normalement pas sur le `$PATH` des usagers normaux parce qu'il ne sert justement pas aux usager normaux. Il est important de le démarrer en tant que root.

### Arguments
Vous pouvez afficher la liste des arguments supportés avec `/sbin/login_menu --help`.

Un argument particulièrement utile si vous démarrer le programme manuellement (pour le tester, par exemple) est `/sbin/login_menu -q`, puisque cela permet de quitter le programme en appuyant sur `q`. Je déconseille fortement d'activer cette option si ce n'est pas pour tester le programme.

### Fichier de configuration
Le fichier de configuration par défaut (/etc/login_menu.conf) permet de se connecter au système, de suspendre l'ordinateur, de le redémarrer et de l'éteindre. Vous pouvez y ajouter autant d'autres options que vous voulez (ou en enlever), mais assurez-vous de ne pas en mettre trop, car le programme ne vérifie pas si toutes les entrées fittent sur l'écran. Ça ne devrait quand même pas être un problème...

## Sécurité
Ce programme, lorsque démarré par systemd avec le fichier de service par défaut, s'exécute en tant que root avant le login manager habituel (agetty) sur les tty où il est actif.

Toutefois, agetty n'est pas désactivé. login_manager s'arrange seulement pour s'exécuter avant lui. Cela signifie que si jamais le programme ne fonctionnait plus (parce qu'on l'a tué trop de fois d'affilée, par exemple) ou que systemd n'était plus capable de l'ouvrir, systemd devrait démarrer automatiquement agetty, puisqu'il est le "prochain service en file". Les risques d'ouvrir une shell root par accident sont donc normalement très faibles, mais peut-être pas impossibles.

Ce programme exécute d'autres programmes. Cela peut sembler banal, mais ce ne l'est pas quand on parle de programmes exécutés en tant que root avant même qu'usager se connecte. Ce programme n'effectue AUCUNE vérification sur la nature des programmes qu'il exécute. Une entrée peu réfléchie dans le fichier de configuration peut donc amener une faille MAJEURE de sécurité, pouvant permettre l'accès à une shell root sans connexion.

Par conséquent, bien que les failles de sécurité les plus évidentes aient été comblées ou soient facilement évitables, ce programme NE PEUT PAS être considéré aussi sécuritaire que le login manager déjà en place.

**Si la sécurité de votre ordinateur au moment du login vous concerne, n'utilisez pas ce programme**.