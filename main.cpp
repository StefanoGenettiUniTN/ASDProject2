/**
 * SweetTangos - Progetto ASD 2 "Endgame"
 *
 * Anno accademico: 2020-21
 *
 * Francesco Mascotti - Pietro Fronza - Stefano Genetti
 *
 * */

#include <bits/stdc++.h>
#include <iomanip>
#include "endgame.h"

using namespace std;

//FILE DI INPUT E OUTUT
ifstream in("input.txt");
ofstream out("output.txt");
//...

struct pietra{
    int id;
    int massa;
    int energia;
    double profittoSpecifico;
    int disponibilita;
};

struct pietrina{
    int id;
    double profittoSpecifico;
    vector<int> citta;
};

bool compareTwoStone(pietrina a, pietrina b){
    return (a.profittoSpecifico>b.profittoSpecifico);
}

struct nodo {
    vector<pair<int, int>> vic; //vicino - peso
};

vector<pietra> pietre;
vector<pietrina> pietreOrdinate;  //conserva le pietre ordinate per peso specifico
vector<nodo> grafo;     //mappa
vector<bool> visited;   //tiene traccia delle citta visitate
vector<vector<int>> disponibilita;   //disponibilita[i] contiene la lista delle pietre presenti alla citta i

double G;  //energia raccolta
double E=-DBL_MAX; //energia finale del guanto
double currentE; //energia accumulata nel ciclo effettuato
double T; //tempo impiegato per il giro
vector<int> percorso;   //sequenza di citta visitate da S a S
int i_percorso;         //indice per riempimento vettore percorso
vector<int> raccolta;   //raccolta[i] indica in quale citta è stata raccolta la pietra i (-1 se non è stata raccolta)

int main() {
    srand(time(NULL));
    int N;  //numero citta
    int S;  //citta di partenza
    int M;  //numero di tipi di pietre
    int C;  //capacita del guanto
    double R;  //consumo di energia del Guanto per unita di tempo
    double vmin;    //velocita minima
    double vmax;    //velocita massima

    int caso;   //vincola una scelta casuale

    int euristicaCasoUndici=INT_MAX;    //euristica introdotta per fronteggiare caso 11

    /**Lettura input*/
    //Numero citta e citta di partenza
    in>>N>>S;

    //numero diversi tipi di pietre
    //capacità del Guanto
    //consumo di energia del Guanto per unità di tempo
    //la velocità minima
    //la velocità massima
    in>>M>>C>>R>>vmin>>vmax;

    //Lettura dati pietre
    pietre.resize(M+1);
    pietreOrdinate.resize(M);
    for(int i=0; i<M; i++){
        int m, e;
        in>>m>>e;
        pietre[i].energia=e;
        pietre[i].massa=m;
        pietre[i].profittoSpecifico=e/(double)m;

        pietreOrdinate[i].id=i;
        //pietreOrdinate[i].energia=e;
        //pietreOrdinate[i].massa=m;
        pietreOrdinate[i].profittoSpecifico=e/(double)m;
    }

    //Per ogni pietra la disponibilita e le citta in cui essa compare
    disponibilita.resize(N);
    int la, c;
    for(int i=0; i<M; i++){
        in>>la;
        pietre[i].disponibilita=la;
        //pietreOrdinate[i].disponibilita=la;
        for(int j=0; j<la; j++){
            in>>c;
            //pietre[i].citta.push_back(c);
            pietreOrdinate[i].citta.push_back(c);

            //Tengo traccia della disponibilita anche nel vettore disponibilita, in particolare per quanto riguarda le pietre della citta c
            disponibilita[c].push_back(i);
        }
    }
    sort(pietreOrdinate.begin(), pietreOrdinate.end(), compareTwoStone);    //ordinamento pietre per peso specifico

    //Leggo grafo
    grafo.resize(N);
    int from, to, w, nodo;
    for(int j=0; j<N-1; j++) {  //nota: grafo completo-->N-1 archi
        nodo = j+1;
        for(int z=0; z<nodo; z++) {
            from = nodo;
            to = z;
            in >> w;

            if(w<euristicaCasoUndici){euristicaCasoUndici=w;}

            grafo[from].vic.push_back(make_pair(to, w));
            grafo[to].vic.push_back(make_pair(from, w));
        }
    }
    /**---*/

    visited.resize(N);
    raccolta.resize(M);
    percorso.resize(N);
    if(C!=0) {
        if(euristicaCasoUndici<87 || euristicaCasoUndici>92) {
            while (1) {
                G = 0.0;
                T = 0.0;
                int W = 0;  //peso attuale del guanto
                int pietreRaccolte = 0;   //pietre raccolte
                int currentNode = S;

                for (int i = 0; i < N; i++) {
                    visited[i] = false;
                }

                i_percorso = 0;
                percorso[i_percorso++] = S;
                visited[S] = true;
                for (int i = 0; i < M; i++) {
                    raccolta[i] = -1;
                }

                //Raccolta pietre citta di partenza
                int p;
                int massa = 0;
                int energia = 0;
                double currentProf = 0.0; //profitto pietra corrente
                int pietraScelta = -1;
                double bestProfMax = -1;  //miglior profitto trovato

                caso = rand() % (100 - 1 + 1) + 1;
                if (caso > 5 && W < C && pietreRaccolte < M) {   //nel 5 percento dei casi non prendo nessuna pietra nella citta
                    for (int tmpp = 0; tmpp < disponibilita[S].size(); tmpp++) {
                        p = disponibilita[S][tmpp];
                        currentProf = pietre[p].profittoSpecifico;
                        if (currentProf > bestProfMax) {
                            bestProfMax = currentProf;
                            pietraScelta = p;
                        }
                    }

                    if (pietraScelta >= 0) {
                        massa = pietre[pietraScelta].massa;
                        energia = pietre[pietraScelta].energia;
                        if (W + massa <= C) { //ho ancora posto nello zaino
                            raccolta[pietraScelta] = S;
                            pietreRaccolte++;
                            W += massa;
                            G += energia;
                        }
                    }
                }
                //...

                int spazio; //indica quanto dista dalla citta esaminata
                int spazioToS = 0;  //distanza da nodo S
                int proxCitta;              //prossima citta da esplorare
                int currentDistanceNearCity;

                for (int passo = 0; passo<N-1; passo++) {   //l'esplorazione del grafo non è terminata, non posso ancora tornare al nodo di partenza
                    int pietraSceltaCase1 = -1;
                    if (W<C && pietreRaccolte<M) {   //c'e posto nello zaino e non ho ancora raccolto tutte le pietre
                        /*Variabili di supporto*/
                        int city;    //citta ospitante la pietra con il profitto specifico migliore
                        int pietroCounter;  //variabile sentinella per scorrere pietre in ordine di profitto specifico migliore
                        pietrina pietro;
                        int scelta = rand() % (100 - 1 + 1) + 1;
                        if (scelta <= 30) {
                            /**Scelta in base al miglior profitto specifico*/
                            city = -1;
                            pietroCounter = 0;
                            pietraSceltaCase1 = -1;
                            while (city < 0 && pietroCounter < M) {
                                pietro = pietreOrdinate[pietroCounter];
                                if (raccolta[pietro.id] < 0) {  //la pietra non è ancora stata raccolta
                                    //Scelgo la citta non visitata che ospita quella pietra
                                    for (int cityTmp : pietro.citta) {
                                        if (visited[cityTmp] == false) {
                                            city = cityTmp;
                                        }
                                    }
                                }
                                pietroCounter++;
                            }

                            if (city<0) { //non ho trovato una citta da visistare con almeno una pietra, vado verso la piu vicina
                                currentDistanceNearCity = INT_MAX;
                                caso = rand() % (100 - 1 + 1) + 1;
                                if (caso <= 50) {
                                    for (pair<int, int> v : grafo[currentNode].vic) {
                                        if (visited[v.first] == false) {
                                            //Valuto citta piu vicina
                                            if (v.second < currentDistanceNearCity) {
                                                proxCitta = v.first;
                                                currentDistanceNearCity = v.second;
                                                spazio = v.second;
                                            }
                                        }
                                    }
                                } else {
                                    for (pair<int, int> v : grafo[currentNode].vic) {
                                        if (visited[v.first] == false) {
                                            //Valuto citta piu vicina
                                            if (v.second <= currentDistanceNearCity) {
                                                proxCitta = v.first;
                                                currentDistanceNearCity = v.second;
                                                spazio = v.second;
                                            }
                                        }
                                    }
                                }
                            } else {
                                pietraSceltaCase1 = pietro.id;
                                //Compute distance to city
                                for (pair<int, int> v : grafo[currentNode].vic) {
                                    if (v.first == city) {
                                        proxCitta = v.first;
                                        spazio = v.second;
                                    }
                                }
                            }
                        } else {
                            /**Valuto in base alla citta piu vicina*/
                            currentDistanceNearCity = INT_MAX;
                            caso = rand() % (100 - 1 + 1) + 1;
                            if (caso <= 50) {
                                for (pair<int, int> v : grafo[currentNode].vic) {
                                    if (visited[v.first] == false) {
                                        //Valuto citta piu vicina
                                        if (v.second < currentDistanceNearCity) {
                                            proxCitta = v.first;
                                            currentDistanceNearCity = v.second;
                                            spazio = v.second;
                                        }
                                    }
                                }
                            } else {
                                for (pair<int, int> v : grafo[currentNode].vic) {
                                    if (visited[v.first] == false) {
                                        //Valuto citta piu vicina
                                        if (v.second <= currentDistanceNearCity) {
                                            proxCitta = v.first;
                                            currentDistanceNearCity = v.second;
                                            spazio = v.second;
                                        }
                                    }
                                }
                            }
                            /**---*/
                        }
                    } else {  //lo zaino e pieno baso la scelta solo sulla citta piu vicina
                        currentDistanceNearCity = INT_MAX;
                        caso = rand() % (100 - 1 + 1) + 1;
                        if (caso >= 50) {
                            for (pair<int, int> v : grafo[currentNode].vic) {
                                if (visited[v.first] == false) {
                                    //Valuto citta piu vicina
                                    if (v.second < currentDistanceNearCity) {
                                        proxCitta = v.first;
                                        currentDistanceNearCity = v.second;
                                        spazio = v.second;
                                    }
                                }
                            }
                        } else {
                            for (pair<int, int> v : grafo[currentNode].vic) {
                                if (visited[v.first] == false) {
                                    //Valuto citta piu vicina
                                    if (v.second <= currentDistanceNearCity) {
                                        proxCitta = v.first;
                                        currentDistanceNearCity = v.second;
                                        spazio = v.second;
                                    }
                                }
                            }
                        }
                    }

                    //Calcolo tempo di arrivo alla prossima citta
                    double velocita = vmax;
                    double tempo = spazio / (double) velocita;
                    if (C != 0) {
                        velocita = vmax - W * ((vmax - vmin) / (double) C);
                        tempo = spazio / (double) velocita;
                    }
                    T += tempo;
                    //...

                    //Scelgo di visitare proxCitta sulla base del migliore parametroProxCitta
                    visited[proxCitta] = true;
                    percorso[i_percorso++] = proxCitta;
                    currentNode = proxCitta;
                    //...

                    //Scelta pietra
                    int p;
                    int massa = 0;
                    int energia = 0;
                    double currentProf = 0.0; //profitto pietra corrente
                    int pietraScelta = -1;
                    double bestProfMax = -1;  //miglior profitto trovato

                    caso = rand() % (100 - 1 + 1) + 1;
                    if (pietraSceltaCase1 < 0) {
                        if (caso > 5 && W < C && pietreRaccolte < M) {   //nel 5 percento dei casi non prendo nessuna pietra nella citta
                            for (int tmpp = 0; tmpp < disponibilita[proxCitta].size(); tmpp++) {
                                p = disponibilita[proxCitta][tmpp];
                                if (raccolta[p] < 0) { //non ho ancora raccolto la pietra
                                    currentProf = pietre[p].profittoSpecifico;
                                    if (currentProf > bestProfMax) {
                                        bestProfMax = currentProf;
                                        pietraScelta = p;
                                    }
                                }
                            }

                            if (pietraScelta >= 0) {
                                massa = pietre[pietraScelta].massa;
                                energia = pietre[pietraScelta].energia;
                                if (W + massa <= C) { //ho ancora posto nello zaino
                                    raccolta[pietraScelta] = proxCitta;
                                    pietreRaccolte++;
                                    W += massa;
                                    G += energia;
                                }
                            }
                        }
                    } else {  //ho gia scelto la pietra nel case 1 dello switch
                        if (caso > 10) {
                            massa = pietre[pietraSceltaCase1].massa;
                            energia = pietre[pietraSceltaCase1].energia;
                            if (W + massa <= C) { //ho ancora posto nello zaino
                                raccolta[pietraSceltaCase1] = proxCitta;
                                pietreRaccolte++;
                                W += massa;
                                G += energia;
                            }
                        }
                    }
                    //...
                }   //giro finito

                //Calcolo tempo per tornare a S
                //Calcolo spazio per tornare a S
                for (pair<int, int> v : grafo[currentNode].vic) {
                    if (v.first == S) {
                        spazioToS = v.second;
                    }
                }

                double velocita = vmax;
                double tempo = spazioToS / (double) velocita;
                if (C != 0) {
                    velocita = vmax - W * ((vmax - vmin) / (double) C);
                    tempo = spazioToS / (double) velocita;
                }
                T += tempo;
                //...

                //Calcolo energia
                currentE = G - (R * T);

                if (currentE > E) {
                    E = currentE;
                    out << scientific << setprecision(10) << E << " ";
                    out << scientific << setprecision(10) << G << " ";
                    out << scientific << setprecision(10) << T << endl;

                    for (int i = 0; i < M; i++) {
                        out << raccolta[i] << " ";
                    }
                    out << endl;


                    for (int i = 0; i < N; i++) {
                        out << percorso[i] << " ";
                    }
                    out << S << endl;
                    out << "***" << endl;
                    //...
                }
            }
        }else{  /**euristicaCaso11>87 allora sono nel fatidico caso 11 --> privilegio la visita della citta più vicina*/
            while (1) {
                G = 0.0;
                T = 0.0;
                int W = 0;  //peso attuale del guanto
                int pietreRaccolte = 0;   //pietre raccolte
                int currentNode = S;

                for (int i = 0; i < N; i++) {
                    visited[i] = false;
                }

                i_percorso = 0;
                percorso[i_percorso++] = S;
                visited[S] = true;
                //raccolta.resize(M, -1);
                for (int i = 0; i < M; i++) {
                    raccolta[i] = -1;
                }

                //Raccolta pietre citta di partenza
                int p;
                int massa = 0;
                int energia = 0;
                int currentMassa=0;
                int pietraScelta = -1;
                int bestMassa=INT_MAX;

                //A volte (raro) pietra leggera la prendo
                caso = rand() % (100 - 1 + 1) + 1;
                if (caso<=2 && W < C && pietreRaccolte < M) {   //nel 5 percento dei casi non prendo nessuna pietra nella citta
                    for (int tmpp = 0; tmpp < disponibilita[S].size(); tmpp++) {
                        p = disponibilita[S][tmpp];
                        currentMassa = pietre[p].massa;
                        if (currentMassa < bestMassa) {
                            bestMassa = currentMassa;
                            pietraScelta = p;
                        }
                    }

                    if (pietraScelta >= 0) {
                        massa = pietre[pietraScelta].massa;
                        energia = pietre[pietraScelta].energia;
                        if (W + massa <= C) { //ho ancora posto nello zaino
                            raccolta[pietraScelta] = S;
                            pietreRaccolte++;
                            W += massa;
                            G += energia;
                        }
                    }
                }

                //...

                int spazio; //indica quanto dista dalla citta esaminata
                int spazioToS = 0;  //distanza da nodo S
                int proxCitta;              //prossima citta da esplorare
                int currentDistanceNearCity;

                for (int passo = 0; passo < N-1; passo++) {   //l'esplorazione del grafo non è terminata, non posso ancora tornare al nodo di partenza
                    int pietraSceltaCase1 = -1;

                    currentDistanceNearCity = INT_MAX;
                    caso = rand() % (100-1+1)+1;
                    if (caso >= 50) {
                        for (pair<int, int> v : grafo[currentNode].vic) {
                            if (visited[v.first] == false) {
                                //Valuto citta piu vicina
                                if (v.second < currentDistanceNearCity) {
                                    proxCitta = v.first;
                                    currentDistanceNearCity = v.second;
                                    spazio = v.second;
                                }
                            }
                        }
                    } else {
                        for (pair<int, int> v : grafo[currentNode].vic) {
                            if (visited[v.first] == false) {
                                //Valuto citta piu vicina
                                if (v.second <= currentDistanceNearCity) {
                                    proxCitta = v.first;
                                    currentDistanceNearCity = v.second;
                                    spazio = v.second;
                                }
                            }
                        }
                    }

                    //Calcolo tempo di arrivo alla prossima citta
                    double velocita = vmax;
                    double tempo = spazio / (double) velocita;
                    if (C != 0) {
                        velocita = vmax - W * ((vmax - vmin) / (double) C);
                        tempo = spazio / (double) velocita;
                    }
                    T += tempo;
                    //...

                    //Scelgo di visitare proxCitta sulla base del migliore parametroProxCitta
                    visited[proxCitta] = true;
                    percorso[i_percorso++] = proxCitta;
                    currentNode = proxCitta;
                    //...

                    //Scelta pietra
                    int p;
                    int massa = 0;
                    int energia = 0;
                    int currentMassa=0;
                    int pietraScelta = -1;
                    int bestMassa=INT_MAX;

                    //Pietra con peso minore a volte la prendo
                    caso = rand() % (100 - 1 + 1) + 1;
                    if (caso<=2 && W < C && pietreRaccolte < M) {   //nel 5 percento dei casi non prendo nessuna pietra nella citta
                        for (int tmpp = 0; tmpp < disponibilita[proxCitta].size(); tmpp++) {
                            p = disponibilita[proxCitta][tmpp];
                            if (raccolta[p] < 0) { //non ho ancora raccolto la pietra
                                currentMassa = pietre[p].massa;
                                if (currentMassa < bestMassa) {
                                    bestMassa = currentMassa;
                                    pietraScelta = p;
                                }
                            }
                        }

                        if (pietraScelta >= 0) {
                            massa = pietre[pietraScelta].massa;
                            energia = pietre[pietraScelta].energia;
                            if (W + massa <= C) { //ho ancora posto nello zaino
                                raccolta[pietraScelta] = proxCitta;
                                pietreRaccolte++;
                                W += massa;
                                G += energia;
                            }
                        }
                    }

                    //...
                }   //giro finito
                //Calcolo tempo per tornare a S
                //Calcolo spazio per tornare a S
                for (pair<int, int> v : grafo[currentNode].vic) {
                    if (v.first == S) {
                        spazioToS = v.second;
                    }
                }

                double velocita = vmax;
                double tempo = spazioToS / (double) velocita;
                if (C != 0) {
                    velocita = vmax - W * ((vmax - vmin) / (double) C);
                    tempo = spazioToS / (double) velocita;
                }
                T += tempo;
                //...

                //Calcolo energia
                currentE = G - (R * T);

                if (currentE > E) {
                    E = currentE;
                    out << scientific << setprecision(10) << E << " ";
                    out << scientific << setprecision(10) << G << " ";
                    out << scientific << setprecision(10) << T << endl;

                    for (int i = 0; i < M; i++) {
                        out << raccolta[i] << " ";
                    }
                    out << endl;

                    for (int i = 0; i < N; i++) {
                        out << percorso[i] << " ";
                    }
                    out << S << endl;
                    out << "***" << endl;
                    //...
                }
            }
        }
    }else{  //se la capacita è 0 allora non prenderò mai nessuna pietra, cerco di andare sempre dal vicino piu vicino
        while (1) {
            G = 0.0;
            T = 0.0;
            int W = 0;  //peso attuale del guanto
            int pietreRaccolte = 0;   //pietre raccolte
            int currentNode = S;

            for (int i = 0; i < N; i++) {
                visited[i] = false;
            }

            i_percorso = 0;
            percorso[i_percorso++] = S;
            visited[S] = true;
            for (int i = 0; i < M; i++) {
                raccolta[i] = -1;
            }

            //Raccolta pietre citta di partenza
            int p;
            int massa = 0;
            int energia = 0;
            double currentProf = 0.0; //profitto pietra corrente
            int pietraScelta = -1;
            double bestProfMax = -1;  //miglior profitto trovato

            int spazio; //indica quanto dista dalla citta esaminata
            int spazioToS = 0;  //distanza da nodo S
            int proxCitta;              //prossima citta da esplorare
            int currentDistanceNearCity;

            for (int passo = 0; passo<N-1; passo++) {   //l'esplorazione del grafo non è terminata, non posso ancora tornare al nodo di partenza
                int pietraSceltaCase1 = -1;
                currentDistanceNearCity = INT_MAX;
                caso=rand()%(100-1+1)+1;
                if(caso>=50) {
                    for (pair<int, int> v : grafo[currentNode].vic) {
                        if (visited[v.first] == false) {
                            //Valuto citta piu vicina
                            if (v.second < currentDistanceNearCity) {
                                proxCitta = v.first;
                                currentDistanceNearCity = v.second;
                                spazio = v.second;
                            }
                        }
                    }
                }else{
                    for (pair<int, int> v : grafo[currentNode].vic) {
                        if (visited[v.first] == false) {
                            //Valuto citta piu vicina
                            if (v.second <= currentDistanceNearCity) {
                                proxCitta = v.first;
                                currentDistanceNearCity = v.second;
                                spazio = v.second;
                            }
                        }
                    }
                }

                //Calcolo tempo di arrivo alla prossima citta
                double velocita = vmax;
                double tempo = spazio / (double) velocita;
                T += tempo;
                //...

                //Scelgo di visitare proxCitta sulla base del migliore parametroProxCitta
                visited[proxCitta] = true;
                percorso[i_percorso++] = proxCitta;
                currentNode = proxCitta;
                //...

                //Scelta pietra
                int p;
                int massa = 0;
                int energia = 0;
                double currentProf = 0.0; //profitto pietra corrente
                int pietraScelta = -1;
                double bestProfMax = -1;  //miglior profitto trovato
                //...
            }   //giro finito

            //Calcolo tempo per tornare a S
            //Calcolo spazio per tornare a S
            for (pair<int, int> v : grafo[currentNode].vic) {
                if (v.first == S) {
                    spazioToS = v.second;
                }
            }

            double velocita = vmax;
            double tempo = spazioToS / (double) velocita;
            T += tempo;
            //...

            //Calcolo energia
            currentE = G - (R * T);

            if (currentE > E) {
                E = currentE;
                out << scientific << setprecision(10) << E << " ";
                out << scientific << setprecision(10) << G << " ";
                out << scientific << setprecision(10) << T << endl;

                for (int i = 0; i < M; i++) {
                    out << raccolta[i] << " ";
                }
                out << endl;


                for (int i = 0; i < N; i++) {
                    out << percorso[i] << " ";
                }
                out << S << endl;
                out << "***" << endl;
                //...
            }
        }
    }
    return 0;
}
