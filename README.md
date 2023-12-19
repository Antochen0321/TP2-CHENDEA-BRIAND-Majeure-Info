# TP2-CHENDEA-BRIAND-Majeure-Info
Pour le gettftp, nous n'avons pas réussi à faire la question 4.3.  
Pour le puttftp, nous avons fait un code pour les 5.1 et 5.2 mais il semble qu'il y ait un problème d'accès au dossier destination dans lequel on cherche à mettre un fichier.  

Question 1  
Nous avons simplement implémenté la structure générale des deux fonctions que nous utiliserons dans ce TP. 
On peut noter que la fonction prends 2 arguments, le serveur (une adresse IP) source ainsi que le nom du fichier (à copier dans le serveur pour puttftp et à chercher dans le serveur pour gettftp).
  
Question 2  
Nous nous sommes, pour les prochaines questions, uniquement concentrés sur gettftp.  
Dans cette question, il nous fallait ajouter une structure addrinfo afin d'utiliser la fonction getaddrinfo permettant de parcourir le dossier source dans lequel se trouve le fichier que l'on veut lire.  
Nous avons utlisé les documentations en ligne, notamment le manuel linux (https://man7.org/linux/man-pages/man3/getaddrinfo.3.html) afin de trouver comment créer correctement la structure. Cependant, nous avons constaté par la suite que notre code comportait plusieurs erreurs, notamment dans l'appel du PORT qui doit être une chaine de caractère appelée directement dans le getaddrinfo et non un int crée en variable globale (cela génère un segmentation false), sa valeur est d'ailleurs de '1069' et non de '69'.  
L'autre erreur importante est le paramètrage du hint, notamment sur le choix du protocole (il faut choisir le protocole UDP, donc IPPROTO_UDP) et définir un socktype (SOCK_DGRM). Ces modifications sont apportées dans la question 4.2).  
  
Question 3  
Dans cette question, nous avons crée la socket de réseau dans la fonction gettftp, c'est elle qui permet de se connecter au serveur afin de chercher le fichier voulu. Comme pour la question précédente, nous avons changé le code dans la question 4.2 pour des raisons de lisibilité, de simplification mais aussi parce que le code écrit ici provoquait des erreurs lors de la connexion au serveur, la socket gardant une valeur de -1, la connexion était impossible.  
  
Question 4.1-4.2  
Nous donnons le code de la question 4.1 à titre indicatif car nous avons changé beaucoup de chose dans la question 4.2 car nous n'avions pas testé notre programme sur le serveur donné sur moodle avant.
Nous avons crée deux nouvelles structures afin de construire, d'une part la requête de lecture RRQ ainsi que, d'une autre, l'envoi au serveur.  







Question 5  
Nous avons fait la fonction puttftp mais nous n'avons pas réussi à aboutir. Dans le code, on peut retrouver la fonction puttftp, très similaire à gettftp dans la forme ainsi que deux fonctions auxiliaires send_wrq ainsi que send_file permettant de déplacer le fichier dans le serveur host. Nous utilisons les trois strucutures codés dans le même fichier, wrq, data et hack. Le fonctionnement est assez proche de celui de gettftp, on peut cependant préciser qu'ici la difficulté était la fonction send_file qui ouvre le fichier que l'on veut mettre dans le serveur, le lit et envoie les données par paquets (dans la boucle while). Voici ce que l'on voit dans la console ainsi que sur wireshark lorsque l'on teste notre code :  





Nous n'avons pas eu le temps de corriger le problème cependant, cela semble être une erreur de connexion au serveur donc probablement une mauvaise configuration de la socket
