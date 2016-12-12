Description rapidos du squelette.

Le main va servir à lancer l'exécution du simulateur.
LightSim est le core du simulateur, c'est lui qui va gérer les instances d'agents et la simulation.

Agent, c'est la classe de base, faite pour mettre les fonctions dont tous les agents vont avoir besoin, notement les déplacements.
Du coup Predator et Prey parlent d'eux-mêmes.

Retina sera faite pour simuler la rétine. 
Avec des méthodes du genre observe t_agent, count t_agent, qui va pouvoir renvoyer un chouette vecteur qu'on aura plus qu'a metre dans les champs des agents prévu à cet effet. 

Pour le test de présence dans un segement de rétine, le plus rapide que j'ai trouvé pour l'instant c'est :

p1 : position de l'agent
p2 : position testée.
s1 : point extremité inférieure du segment 1
s2 : point extremité supérieure du segment 1

Si la distance entre p1 et p2 est inférieure au radius et p2 est au dessus de la droite p1/s1 et en dessous de la droite p1/s2, on est dans le segment.
On devrait pouvoir avoir une complexité en O(N*M) pour chaque obseravion, N étant le nombre de segments (On peut stopper la recherche dès qu'on a trouvé, et M le nombre d'agents. 
Ce qui permet aussi d'avoir un simple vecteur de points pour les segments de vue en plus d'être beaucoup plus simple à développer qu'avec des capteurs sur un simulateur de robots.
