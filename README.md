	Autor: Soare Ion-Alexandru
	Universitatea si facultatea: UPB, ACS, CTI
	Grupa si ceria: 333 CC
	Profesor coordonator: Cristian Chilipirea
	Anul universitar: 2018-2019

TEMA 1: Antialiasing si micro render

	Partea I: Antialiasing

Prima parte are la baza structura image, ce contine o variabila de tip int (color), reprezentand
tipul de imagine, respectiv doua matrice de unsigned chars, in functie de variabila mentionata precedent.
		Se incearca folosirea cat mai posibila a unui buffer pentru operatiile de scriere/citire, intrucat
grabesc timpul de executie al programului, evitand un apel numeros de interventii asupra fisierelor.
		Cele doua functii, odd_resizer si even_resizer joaca rolul de a returna valorile corespunzatoare
blocurilor de pixeli pe care lucram in acest moment. 
		Inaltimea noilor imagini, respectiv latimea vor fi cele actuale impartite la resize_factor, fiecare
bloc de resize_factor * resize_factor constituind un pixel din imaginea finala. Asadar, parcurgerea se va
face din resize_factor in resize_factor, evitand astfel impunerea unei conditii de ignorare a pixelilor ramasi,
intrucat se fac automat.
		Pentru paralelizare se folosesc doua imagini globale, de input si de output, si doua functii care
imparte algoritmul intr-o parte color si una pentru matrice colorless. Diferenta dintre cele doua este ca
se lucreaza cu tipul corespunzator de matrice, mentionat mai sus in image. Divizarea intervalului de lucru
se face cu ajutorul lui thread_id, impartind in bucati egale liniile matricei. La final se va egala imaginea
din parametrii functiei cu cea globala pe care lucram pentru a fi trimisa mai departe la writeData, unde va fi
transpusa in fisier. 

Observatie:	in functia readInput, a trebuit sa fie citit un byte nefolositor pentru ca imaginea sa nu aiba
				pierderi de date. In absenta acestuia (variabila useless), imaginii ii lipsea un caracter de la final.

	Partea a II-a: micro render

Partea a II-a presupune aceeasi impartire a numarului de linii pe thread-uri pentru a paraleliza executia.
De asemenea, se foloseste o matrice declarata global pe care lucram, urmand ca la final sa fie atribuita out-ului img.
		Matricea se completeaza direct, pornind de la i maxim, descrescator, datorita inversarii pozitiilor normale de
inceput. Asadar, i-ul va merge din X = numarul de linii / numarul de thread-uri, descrescator, de la resolution - 1 
(imaginea fiind de resolution x resolution).
		Imaginea este scalata la centimetri ( * 100 / resolution ), pixelii luati in calcul de la centrul lor (i+= 0.5,
j += 0.5), iar calculele sunt facute cu variabile de tip float, datorita numeroaselor erori de +- un rand de pixeli.


