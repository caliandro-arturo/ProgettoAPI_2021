## 3 Luglio

### Descrizione del programma:

Essenzialmente, il programma stipula una classifica limitata di grafi pesati diretti, inseriti in quantità
non limitata.

Un grafo pesato mette un valore, detto peso, sugli archi che collegano i nodi.

Un grafo diretto pone una direzione per ogni arco, che quindi può essere uscente o entrante rispetto a un 
nodo.

Si considerano grafi connessi.

I pesi rientrano nel dominio D = [0, 2^32 - 1], dove 0 rappresenta la non presenza dell'arco.

Il programma riceve ingressi nel modo seguente:

- `DIM K`: questi sono interi utili per inizializzare il programma
  - DIM = dimensione dei grafi considerati;
  - K = lunghezza della classifica.
- `AggiungiGrafo`: avvia la scansione di un grafo descritto secondo la matrice di adiacenza, che va espressa mettendo
  DIM numeri separati con virgola in DIM righe. Es (DIM = 3):

      3,4,0
      5,12,3
      6,8,9

  dove:
  - la riga x-esima descrive la stella del nodo x (gli archi che escono);
  - la colonna x-esima descrive gli archi entranti nel nodo x.

- `TopK`: stampa la classifica dei primi K grafi inseriti dall'avvio del programma fino al comando, mettendoli su una
  riga, indicandoli attraverso il loro indice (a partire da 0) e separandoli con spazio.

La metrica dei grafi viene stipulata in tale modo:

- si parte dal nodo 0;
- si calcola il cammino minimo da 0 a ognuno dei DIM - 1 nodi rimanenti;
- si sommano i cammini ottenuti.

La classifica si ottiene mettendo al primo posto il grafo con il valore più piccolo della seguente metrica,
inserito prima di ogni altro grafo che abbia la stessa metrica.

### Idee

#### Memoria

Del grafo importa soltanto la metrica: una volta calcolata la metrica, il resto del grafo può
essere immediatamente deallocato.

La classifica contiene al più K elementi. Se K è grande ma vi sono pochi grafi inseriti, potrebbe non
essere richiesto l'allocazione di K celle.

#### Struttura

L'assenza di pesi negativi induce a considerare l'algoritmo di Dijkstra. Il problema, tuttavia, è quello
di calcolare la somma dei cammini minimi, non tanto quello di tenere memoria del percorso stesso.
Forse le due cose sono coniugabili attraverso una semplificazione dell'algoritmo, forse vale la pena trovare
un'altra soluzione.

La classifica è una collezione ordinata ascendente senza ripetizioni, ogni grafo con la stessa metrica di
un grafo già presente nella classifica va escluso. Questo porta a ragionare su tre problemi:

- ricerca di un elemento nella collezione;
- inserimento nella collezione, nel caso in cui la ricerca precedente abbia avuto esito negativo;
- eliminazione di un elemento dalla collezione, qualora questa dovesse aver già raggiunto la dimensione K.

Un albero binario è noto per avere buona complessità temporale - O(log n) - ma vale la pena capire se il sistema di
reintroduzione sia altrettanto efficace.

### Idea attuale di progetto

Provo a usare Dijkstra per calcolare la metrica e uso un albero binario (un RB che già possiedo dai residui del
tentativo di progetto dell'anno scorso).

Per memorizzare il grafo posso utilizzare la rappresentazione a vettore di liste, quando il numero di archi è inferiore
al quadrato del numero di nodi.
---
Mi era passata in mente l'idea che la prima riga di una matrice potesse essere usata subito per trovare i primi valori
utili per la somma dei cammini minimi, ma non funziona.

Es:

    2,3,5
    5,1,3
    6,8,9

0 -> 2 costa 5, totale = 5

0 -> 1 costa 3, 1 -> 2 costa 1, totale = 4 < 5

Ergo, per ora mi limito a pensare a Dijkstra.

---

Un'altra idea è quella di mettere il valore peggiore inserito in classifica direttamente in una variabile di controllo,
in modo tale che qualora la somma di un nuovo grafo dovesse essere peggiore dell'ultima e la classifica dovesse già
essere piena, interromperei subito il calcolo della somma dei nodi.

---

Altra ottimizzazione: salvare la stringa stampata da TopK e un booleano che verifica se la classifica sia stata cambiata
nel frattempo o meno. Questo consente di non perdere tempo quando viene chiamato spesso TopK senza che sia cambiato
nulla.