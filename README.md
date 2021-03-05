#Progetto di Computer Graphics di Mirko Gualducci

##Note:

Il gioco sviluppato è il famoso Space Invaders, dove una navicella deve uccidere tutti gli alieni per passare il livello.
Nel codice ci sono tutti i commenti per capire cosa fa ogni classe, dai suoi campi ai suoi metodi/funzioni.
Il codice è stato sviluppato con Visual Studio 2019 con C++.
Le librerie sono quelle di WIN32 API.

##Struttura:

Alien.h -> header file per la gestione degli alieni
Rock.h -> header file per la gestione delle rocce
Spaceship.h -> header file per la gestione della navicella
Shot.h -> header file per la gestione degli spari di navicella/alieni
SoundSystem.h -> header file per la gestione dei suoni
resource.h -> header file per la gestione delle risorse (icona)
Game.h/Game.cpp -> file per la gestione della logica e grafica di gioco
main.cpp -> file di avvio del programma

##Sviluppo del gioco:

All'apertura il gioco presenta un menù generale con presenti i dettagli del progetto e i tasti per giocare:
Enter/Invio -> pulsante per passare al gioco dai menù
Space -> pulsante per sparare con la navicella
Left Arrow -> pulsante per spostare a sinistra la navicella
Right Arrow -> pulsante per spostare a destra la navicella
Esc -> si esce dal gioco

La seguente schermata presenta la schermata di gioco, che è composta da 5 livelli:
- lv 1: 9 alieni (3 alieni x 3 righe), presenza delle 2 rocce per difendersi e frequenza che un alieno spari per frame 1/60000
- lv 2: 15 alieni (5 alieni x 3 righe), presenza delle 2 rocce per difendersi e frequenza che un alieno spari per frame 1/50000
- lv 3: 21 alieni (7 alieni x 3 righe), presenza delle 2 rocce per difendersi e frequenza che un alieno spari per frame 1/40000
- lv 4: 27 alieni (9 alieni x 3 righe), presenza delle 2 rocce per difendersi e frequenza che un alieno spari per frame 1/30000
- lv 5: 33 alieni (11 alieni x 3 righe), non presenti le 2 rocce per difendersi e frequenza che un alieno spari per frame 1/20000
La schermata di gioco presenta un menù in altro dove sono presenti:
- numero del livello
- punteggio
- tempo rimanente (2 minuti per ogni livello)
- rappresentazione delle vite con navicelle piccole (3 vite)
Si vince il livello se: la navicella uccide tutti gli alieni
Si perde il livello se: scade il tempo oppure si finiscono le vite

Alla fine di un livello compare una schermata che ti dà un riassunto del punteggio e comunica se hai vinto o perso.
Premendo enter/invio a quel punto si può passare al livello successivo se si ha vinto il precendente.
Se si aveva perso oppure si è terminato il livello 5, allora si ricomincia a giocare dal livello 1.