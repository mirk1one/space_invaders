#include <windows.h>		// Header File For Windows
#include <stdio.h>			// Header File For Standard Input/Output
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library

#include "Game.h"
#include "SoundSystem.h"
#include "Spaceship.h"
#include "Alien.h"
#include "SOIL.h"

using namespace std;

// All Setup For OpenGL Goes Here
bool GameModel::InitGL(void)
{
	// Salta alla Texture Loading Routine
	if (!this->LoadGLTextures())
	{
		// Se la texture non viene caricata, ritorna false
		return false;
	}

	glEnable(GL_TEXTURE_2D);							// Abilita la Texture Mapping
	glShadeModel(GL_SMOOTH);							// Abilita Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Abilita Depth Testing
	glDepthFunc(GL_LEQUAL);								// Tipo di Depth Testing da fare
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	this->BuildFont();

	// eye    (0,0,0)
	// center (0,0,-1)
	// up     (0,1,0)
	gluLookAt(0.0, 0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);

	return true;										// Inizializzazione andata a buon fine
}


void GameModel::ReSizeGLScene(int width, int height)
{
	if (height == 0) height = 1;						// Previene una divisione per zero
	if (width == 0) width = 1;							// Previene una divisione per zero

	glViewport(0, 0, width, height);					// Reset della Viewport corrente

	glMatrixMode(GL_PROJECTION);						// Seleziona la matrice di proiezione
	glLoadIdentity();									// Resetta la matrice di proiezione

	// orthographic projection
	// background in [-1, 1] on both x and y
	// fill th background according to the window aspect ratio
	// void WINAPI glOrtho( GLdouble left,   GLdouble right,
	//                      GLdouble bottom, GLdouble top,
	//                      GLdouble zNear,  GLdouble zFar );
	if (width >= height) {
		this->plx = 1.0;
		this->ply = (double)height / (double)width;
	}
	else {
		this->plx = (double)width / (double)height;
		this->ply = 1.0;
	}
	glOrtho(-this->plx, this->plx, -this->ply, this->ply, 1, 5.1);

	// salvataggio la larghezza e l'altezza della finestra in pixel
	this->wHeight = height;
	this->wWidth = width;

	glMatrixMode(GL_MODELVIEW);							// Seleziona la matrice di proiezione
	glLoadIdentity();									// Resetta la matrice di proiezione
}


// Carica una texture
bool GameModel::LoadATexture(const char* name, int id)
{
	// Carico nell'array texture l'immagine
	texture[id] = SOIL_load_OGL_texture(name, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	// Se c'è stato qualche errore nel caricamento, torno false
	if (texture[id] == 0) return false;

	// Generazione tipica di texture utilizzando i dati da bitmap
	glBindTexture(GL_TEXTURE_2D, texture[id]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Tutto ok, ritorno
	return true;
}

// Carica bitmaps e le converte in textures
bool GameModel::LoadGLTextures(void)
{
	// Carica un file d'immagine direttamente come una OpenGL texture
	if (!this->LoadATexture("../Data/background.jpg", 0)) return false;
	if (!this->LoadATexture("../Data/left_button.png", 1)) return false;
	if (!this->LoadATexture("../Data/right_button.png", 2)) return false;
	if (!this->LoadATexture("../Data/space_button.png", 3)) return false;
	if (!this->LoadATexture("../Data/spaceship.png", 4)) return false;
	if (!this->LoadATexture("../Data/invader1.png", 5)) return false;
	if (!this->LoadATexture("../Data/invader2.png", 6)) return false;
	if (!this->LoadATexture("../Data/invader3.png", 7)) return false;
	if (!this->LoadATexture("../Data/invader4.png", 8)) return false;
	if (!this->LoadATexture("../Data/laser.png", 9)) return false;
	if (!this->LoadATexture("../Data/bomb.png", 10)) return false;
	if (!this->LoadATexture("../Data/rock.png", 11)) return false;

	// Tipica texture generation utilizzando i dati dalla bitmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Ritorna ok
	return true;
}

bool GameModel::DrawGLScene(void)
{
	// Pulisce la schermata e depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Seleziona la matrice Modelview
	glMatrixMode(GL_MODELVIEW);
	// Reset della View
	glLoadIdentity();

	/******************************/
	/*********BACKGROUND***********/
	/******************************/

	// Caricamento immagine di background
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(backgroundVertexes[i].u, backgroundVertexes[i].v);
		glVertex3f(backgroundVertexes[i].x, backgroundVertexes[i].y, backgroundVertexes[i].z);
	}
	glEnd();

	// Se la scena è quella del menù iniziale, apro il menù
	if (sceneType == SceneType::OpenMenu)
		DrawOpenMenuScene();
	// Altrimenti se la scena è quella di gioco, apro la schermata di gioco
	else if (sceneType == SceneType::GameLevel)
		DrawGameLevelScene();
	// Altrimenti apro la schermata di fine livello
	else
		DrawEndLevelScene();

	// Tutto ok, ritono
	return true;
}

void GameModel::DrawOpenMenuScene(void)
{
	// Disabitila Texture Mapping
	glDisable(GL_TEXTURE_2D);

	// Colore bianco
	glColor3f(1.0f, 1.0f, 1.0f);

	// Posiziona il testo sullo schermo
	glRasterPos3f(-(float)plx + PixToCoord_X(160), (float)ply - PixToCoord_Y(20), -4);
	this->glPrint("Progetto di Computer Graphics");

	glRasterPos3f(-(float)plx + PixToCoord_X(240), (float)ply - PixToCoord_Y(60), -4);
	this->glPrint("Mirko Gualducci");

	glRasterPos3f(-(float)plx + PixToCoord_X(240), (float)ply - PixToCoord_Y(140), -4);
	this->glPrint("SPACE INVADERS");

	glRasterPos3f(-(float)plx + PixToCoord_X(40), (float)ply - PixToCoord_Y(220), -4);
	this->glPrint("Muovi la navicella");

	glRasterPos3f(-(float)plx + PixToCoord_X(320), (float)ply - PixToCoord_Y(220), -4);
	this->glPrint("Spara con la navicella");

	glRasterPos3f(-(float)plx + PixToCoord_X(180), (float)ply - PixToCoord_Y(340), -4);
	this->glPrint("Premi Enter per iniziare");

	// Disegno i 3 bottoni
	DrawElement(texture[1], 100, 260, 0.08f, 0.08f, buttonsVertexes[0]);
	DrawElement(texture[2], 180, 260, 0.08f, 0.08f, buttonsVertexes[1]);
	DrawElement(texture[3], 450, 260, 0.24f, 0.08f, buttonsVertexes[2]);

	// Abitila Texture Mapping
	glEnable(GL_TEXTURE_2D);
}

void GameModel::DrawEndLevelScene()
{
	// Disabitila Texture Mapping
	glDisable(GL_TEXTURE_2D);

	// Colore bianco
	glColor3f(1.0f, 1.0f, 1.0f);

	// Posiziona il testo sullo schermo
	
	// Se ho vinto il livello, scrivo che hai vinto e i dati del gioco
	if (winLevel)
	{
		glRasterPos3f(-(float)plx + PixToCoord_X(260), (float)ply - PixToCoord_Y(140), -4);
		this->glPrint("HAI VINTO!");

		glRasterPos3f(-(float)plx + PixToCoord_X(200), (float)ply - PixToCoord_Y(180), -4);
		this->glPrint("Livello %2d completato", this->level);

		glRasterPos3f(-(float)plx + PixToCoord_X(200), (float)ply - PixToCoord_Y(220), -4);
		this->glPrint("Punteggio totale: %2d", this->scores);

		glRasterPos3f(-(float)plx + PixToCoord_X(180), (float)ply - PixToCoord_Y(280), -4);

		// Se non ho raggiunto l'ultimo livello, prosegui altrimenti rigioca
		if (this->level < N_LEVELS)
			this->glPrint("Premi Enter per proseguire");
		else
			this->glPrint("Premi Enter per rigiocare");
	}
	// Se hai perso, scrivo che hai perso e il punteggio totale
	else
	{
		glRasterPos3f(-(float)plx + PixToCoord_X(190), (float)ply - PixToCoord_Y(140), -4);
		// Se ho perso perchè ho zero vite, lo scrivo, altrimenti ho perso perchè è terminato il tempo
		if (lifes == 0)
			this->glPrint("HAI PERSO! VITE FINITE");
		else
			this->glPrint("HAI PERSO! TEMPO FINITO");

		glRasterPos3f(-(float)plx + PixToCoord_X(200), (float)ply - PixToCoord_Y(220), -4);
		this->glPrint("Punteggio totale: %2d", this->scores);

		glRasterPos3f(-(float)plx + PixToCoord_X(180), (float)ply - PixToCoord_Y(280), -4);
		this->glPrint("Premi Enter per rigiocare");
	}

	// Abilita Texture Mapping
	glEnable(GL_TEXTURE_2D);
}

void GameModel::DrawGameLevelScene(void)
{
	/******************************/
	/**********TIMER***************/
	/******************************/

	// Prendo il clock di questo istante
	clock_t t = clock();

	// Tempo trascorso in secondi dall'inizio del programma
	this->fullElapsed = double(t - tStart) / (double)CLOCKS_PER_SEC;
	this->tStamp = t;

	//Tempo del livello rimanente
	int levelTime = (int)(this->tEndLevel - this->tStamp) / CLOCKS_PER_SEC + 1;

	/******************************/
	/*****CONTROLLO*FINE*GIOCO*****/
	/******************************/

	// Se sono rimasti zero alieni -> ho vinto, allora eseguo suono applausi
	// e termino la partita andando alla schermata di fine livello
	if (nAliensAlive == 0)
	{
		winLevel = true;
		SoundSystem.PlayWin();
		sceneType = SceneType::EndLevel;
	}
	// Se ho zero vite oppure è finito il tempo -> ho perso, allora eseguo suono
	// urlo e termino la partita andando alla schermata di fine livello
	else if(lifes == 0 || levelTime <= 0)
	{
		winLevel = false;
		SoundSystem.PlayScream();
		sceneType = SceneType::EndLevel;
	}

	/******************************/
	/*********NAVICELLA************/
	/******************************/

	// Caricamento dell'immagine della navicella
	DrawElementf(texture[4], Spaceship.px, Spaceship.py, 0.08f, 0.08f, Spaceship.spaceshipVertexes);

	/******************************/
	/***********ROCCE**************/
	/******************************/

	// Caricamento delle immagini delle due rocce (se non siamo al livello 5)
	if (level < N_LEVELS)
	{
		DrawElementf(texture[11], rocks[0].px, rocks[0].py, 0.12f, 0.12f, rocks[0].rockVertexes);
		DrawElementf(texture[11], rocks[1].px, rocks[1].py, 0.12f, 0.12f, rocks[1].rockVertexes);
	}

	/******************************/
	/***********ALIENI*************/
	/******************************/

	// GESTIONE MOVIMENTI ALIENI SU BORDI LATERALI

	// Se ho raggiunto il bordo sx con l'alieno più a sx, aggiorno valore
	if (aliensFL[0].px <= FLOAT_SX_BORDER)
		aliensOnLeftBorder = true;
	else
		aliensOnLeftBorder = false;

	// Se ho raggiunto il bordo dx con l'alieno più a dx, aggiorno valore
	if (aliensFL[nAliensPerRow - 1].px >= FLOAT_DX_BORDER)
		aliensOnRightBorder = true;
	else
		aliensOnRightBorder = false;

	// GESTIONE MOVIMENTI ALIENI VERSO IL BASSO

	// Se ho raggiunto la discesa successiva con l'alieno più basso, allora gli alieni
	// sono sul livello basso successivo e assegno il successivo nuovo livello basso
	if (aliensFL[0].py <= levelTurnHoriz)
	{
		aliensOnDownLevel = true;
		levelTurnHoriz -= FLOAT_MAX_DOWN_PER_ROW;
	}
	// Altrimenti se ho raggiunto il minimo livello di discesa degli alieni,
	// allora segno che ho raggiunto il minimo livello e non devo più scendere
	else if (levelTurnHoriz <= FLOAT_MIN_DOWN_LEVEL)
	{
		aliensOnDownLevel = false;
		aliensOnMinDownLevel = true;
	}
	// Altrimenti non ho raggiunto il livello successivo in basso dove spostarmi
	else
		aliensOnDownLevel = false;

	// GESTIONE DIREZIONE DEGLI ALIENI

	// Movimento automatico degli alieni
	for (int i = 0; i < nAliensPerRow; i++)
	{
		// Se gli alieni hanno raggiunto il livello di discesa successivo e
		// si trovano sul bordo sx -> si muovono verso dx
		if (aliensOnMinDownLevel && aliensOnLeftBorder)
		{
			aliensFL[i].direction = AlienDirection::Right;
			aliensSL[i].direction = AlienDirection::Right;
			aliensTL[i].direction = AlienDirection::Right;
		}
		// Se gli alieni hanno raggiunto il livello di discesa successivo e
		// si trovano sul bordo dx -> si muovono verso sx
		else if (aliensOnMinDownLevel && aliensOnRightBorder)
		{
			aliensFL[i].direction = AlienDirection::Left;
			aliensSL[i].direction = AlienDirection::Left;
			aliensTL[i].direction = AlienDirection::Left;
		}
		// Controllo se l'alieno della fila più bassa si muove verticalmente e se ha raggiunto
		// il minimo livello di discesa e si trova sul bordo sx -> si muove verso dx
		else if (aliensFL[i].direction == AlienDirection::Down && aliensOnDownLevel && aliensOnLeftBorder)
		{
			aliensFL[i].direction = AlienDirection::Right;
			aliensSL[i].direction = AlienDirection::Right;
			aliensTL[i].direction = AlienDirection::Right;
		}
		// Altrimenti controllo se l'alieno della fila più bassa si muove verticalmente e se ha raggiunto
		// il minimo livello di discesa e si trova sul bordo dx -> si muove verso sx
		else if (aliensFL[i].direction == AlienDirection::Down && aliensOnDownLevel && aliensOnRightBorder)
		{
			aliensFL[i].direction = AlienDirection::Left;
			aliensSL[i].direction = AlienDirection::Left;
			aliensTL[i].direction = AlienDirection::Left;
		}
		// Altrimenti controllo se l'alieno della fila più bassa è arrivato al bordo sx
		// oppure se l'alieno della fila più bassa è arrivato al bordo dx e
		// se non si sta già muovendo verticalmente -> si muove verticalmente
		else if ((aliensOnLeftBorder || aliensOnRightBorder) &&
			aliensFL[i].direction != AlienDirection::Down)
		{
			aliensFL[i].direction = AlienDirection::Down;
			aliensSL[i].direction = AlienDirection::Down;
			aliensTL[i].direction = AlienDirection::Down;
		}

		// Muovo gli alieni
		aliensFL[i].Move();
		aliensSL[i].Move();
		aliensTL[i].Move();

		// Se l'alieno della prima fila è ancora vivo
		if (aliensFL[i].isAlive)
		{
			// Genero lo sparo dell'alieno
			Shot* shotAlien = aliensFL[i].MakeShot();
			if (shotAlien != NULL)
				shots.push_back(*shotAlien);

			// Disegno l'alieno
			DrawElementf(texture[5], aliensFL[i].px, aliensFL[i].py, 0.08f, 0.08f, aliensFL[i].alienVertexes);
		}

		// Se l'alieno della seconda fila è ancora vivo
		if (aliensSL[i].isAlive)
		{
			// Genero lo sparo dell'alieno
			Shot* shotAlien = aliensSL[i].MakeShot();
			if (shotAlien != NULL)
				shots.push_back(*shotAlien);

			// Disegno l'alieno
			DrawElementf(texture[6], aliensSL[i].px, aliensSL[i].py, 0.08f, 0.08f, aliensSL[i].alienVertexes);
		}

		// Se l'alieno della terza fila è ancora vivo
		if (aliensTL[i].isAlive)
		{
			// Genero lo sparo dell'alieno
			Shot* shotAlien = aliensTL[i].MakeShot();
			if (shotAlien != NULL)
				shots.push_back(*shotAlien);

			// Disegno l'alieno
			DrawElementf(texture[7], aliensTL[i].px, aliensTL[i].py, 0.08f, 0.08f, aliensTL[i].alienVertexes);
		}
	}

	/******************************/
	/***********SPARI**************/
	/******************************/

	// Scandisco tutti gli spari
	list<Shot>::iterator sh = shots.begin();
	while (sh != shots.end()) {
		// Se lo sparo è da eliminare, lo elimino e salto al successivo
		// Questo lo faccio qua e non dove dovrebbe essere eliminato per
		// non incorrere nel problema di cancellazione con iteratore successivo
		if(sh->toErase)
		{
			shots.erase(sh++);
			continue;
		}

		// Muovo lo sparo
		sh->Move();
		
		// Se lo sparo non si trova più nella finestra, lo segno da eliminare
		if (sh->py < -0.7 || sh->py > 0.7)
		{
			sh->toErase = true;
		}
		// Altrimenti se lo sparo si trova nell'area delle rocce,
		// segno lo sparo da eliminare ed eseguo il suono del colpo
		// (se non sono arrivaro al livello 5 perchè le rocce spariscono)
		else if((rocks[0].HasIn(sh->px, sh->py) || rocks[1].HasIn(sh->px, sh->py))
			&& level < N_LEVELS)
		{
			SoundSystem.PlayBlow();
			sh->toErase = true;
		}
		// Altrimenti se lo sparo di un alieno si trova nell'area della navicella,
		// tolgo una vita, eseguo il suono dell'esplosione
		// e segno lo sparo da eliminare (avendo colpito la navicella)
		else if (sh->shotType == ShotType::AlienShot &&
			Spaceship.HasIn(sh->px, sh->py))
		{
			lifes--;
			SoundSystem.PlayExplosion();
			sh->toErase = true;
		}
		// Altrimenti lo sparo è ancora nella finestra
		else
		{
			bool deleted = false;
			// Se lo sparo è della navicella e ha colpito un alieno vivo,
			// allora lo segno da rimuovere e metto l'alieno a morto, così non
			// verrà più disegnato, inoltre eseguo il suono del
			// tonfo e aggiorno il punteggio
			for (int i = 0; i < nAliensPerRow && !deleted; i++)
			{
				if(sh->shotType == ShotType::SpaceshipShot &&
					aliensFL[i].isAlive && !deleted &&
					aliensFL[i].HasIn(sh->px, sh->py))
				{
					sh->toErase = true;
					aliensFL[i].isAlive = false;
					nAliensAlive--;
					SoundSystem.PlaySplat();
					scores += aliensFL[i].score;
					deleted = true;
					break;
				}
				else if(sh->shotType == ShotType::SpaceshipShot &&
					aliensSL[i].isAlive && !deleted &&
					aliensSL[i].HasIn(sh->px, sh->py))
				{
					sh->toErase = true;
					aliensSL[i].isAlive = false;
					nAliensAlive--;
					SoundSystem.PlaySplat();
					scores += aliensSL[i].score;
					deleted = true;
					break;
				}
				else if (sh->shotType == ShotType::SpaceshipShot &&
					aliensTL[i].isAlive && !deleted &&
					aliensTL[i].HasIn(sh->px, sh->py))
				{
					sh->toErase = true;
					aliensTL[i].isAlive = false;
					nAliensAlive--;
					SoundSystem.PlaySplat();
					scores += aliensTL[i].score;
					deleted = true;
					break;
				}
			}
			// Se lo sparo non ha colpito nessun alieno e nemmeno la navicella,
			// allora si trova dentro la finestra, quindi lo disegno
			if (!deleted)
			{
				// Disegno lo sparo, se lo sparo è della navicella
				// disegno il laser, altrimenti disegno la bomba
				if (sh->shotType == ShotType::SpaceshipShot)
					DrawElementf(texture[9], sh->px, sh->py, 0.02f, 0.02f, sh->shotVertexes);
				else
					DrawElementf(texture[10], sh->px, sh->py, 0.02f, 0.02f, sh->shotVertexes);
			}

			// Vado allo sparo successivo
			++sh;
		}
	}

	/******************************/
	/*******MENU'**SUPERIORE*******/
	/******************************/

	// Abilitazione testo
	glMatrixMode(GL_MODELVIEW);				// Seleziona la Modelview Matrix
	glLoadIdentity();						// Reset Modelview Matrix corrente
	glDisable(GL_TEXTURE_2D);				// Disabitila Texture Mapping

	// Colore bianco
	glColor3f(1.0f, 1.0f, 1.0f);

	// Testo livello
	glRasterPos3f(-(float)plx + PixToCoord_X(20), (float)ply - PixToCoord_Y(20), -4);
	this->glPrint("Level: %2d", this->level);

	// Testo tempo livello
	glRasterPos3f(-(float)plx + PixToCoord_X(wWidth - 140), (float)ply - PixToCoord_Y(20), -4);
	this->glPrint("Time: %2d", levelTime);

	// Testo punteggio
	glRasterPos3f(-(float)plx + PixToCoord_X(20), (float)ply - PixToCoord_Y(50), -4);
	this->glPrint("Scores: %2d", this->scores);

	// Simboli vite
	for (int i = 0; i < N_LIFES; i++)
		if (i < lifes)
			DrawElement(texture[4], wWidth - 40 * (N_LIFES - i), 50, 0.04f, 0.04f, lifesVertexes[i]);

	glEnable(GL_TEXTURE_2D);		// Abilita mapping texture
}

void GameModel::DrawElement(GLuint texture, int px, int py, GLfloat scalex, GLfloat scaley, vector<Vertex> vector)
{
	// Caricamento della Texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Trasformazione geometrica texture
	glMatrixMode(GL_MODELVIEW);				// Seleziona la matrice di Modelview
	glLoadIdentity();						// Resetta la View

	// Posizione della texture
	glTranslatef(-(float)plx + PixToCoord_X(px), (float)ply - PixToCoord_Y(py), 0);
	// Scalatura della texture
	glScalef(scalex, scaley, 1);

	// Parametri disegni
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(vector[i].u, vector[i].v);
		glVertex3f(vector[i].x, vector[i].y, vector[i].z);
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}

void GameModel::DrawElementf(GLuint texture, float px, float py, GLfloat scalex, GLfloat scaley, vector<Vertex> vector)
{
	// Caricamento della Texture
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Trasformazione geometrica texture
	glMatrixMode(GL_MODELVIEW);				// Seleziona la matrice di Modelview
	glLoadIdentity();						// Resetta la View

	// Posizione della texture
	glTranslatef((float)px, (float)py, 0);
	// Scalatura della texture
	glScalef(scalex, scaley, 1);

	// Parametri disegni
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(vector[i].u, vector[i].v);
		glVertex3f(vector[i].x, vector[i].y, vector[i].z);
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_ALPHA_TEST);
}


//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//  bitmap fonts
void GameModel::BuildFont(void)					// Crea il Bitmap Font
{
	HFONT	font;								// Windows Font ID
	HFONT	oldfont;							// Usato per tenercelo in casa

	base = glGenLists(96);						// Memoria per 96 characters

	font = CreateFont(-20,						// Altezza del Font
		0,										// Larghezza del Font
		0,										// Angolo di Escapement
		0,										// Angolo di orientazione
		FW_BOLD,								// Spessore font
		FALSE,									// Corsivo
		FALSE,									// Sottolineato
		FALSE,									// Strikeout
		ANSI_CHARSET,							// Identificatore insieme caratteri
		OUT_TT_PRECIS,							// Precisione di output
		CLIP_DEFAULT_PRECIS,					// Precisione di Clipping
		ANTIALIASED_QUALITY,					// Qualità di output
		FF_DONTCARE | DEFAULT_PITCH,			// Family e Pitch
		"Courier New");							// Nome Font

	oldfont = (HFONT)SelectObject(hDC, font);   // Seleziono il font che voglio
	wglUseFontBitmaps(hDC, 32, 96, base);		// Costruisco 96 caratteri iniziando dal carattere 32
	SelectObject(hDC, oldfont);					// Selezioni il font che voglio
	DeleteObject(font);							// Elimina il font
}

void GameModel::KillFont(void)					// Cancella la lista di font
{
	glDeleteLists(base, 96);					// Cancella tutti i 96 caratteri
}

void GameModel::glPrint(const char* fmt, ...)			// Custom GL "Print" Routine
{
	char		text[256];								// Salviamo la nostra stringa
	va_list		ap;										// Puntatore alla lista di argomenti

	if (fmt == NULL)									// Se non c'è testo, non faccio nulla
		return;

	va_start(ap, fmt);									// Parsing della stringa per le variabili
	vsprintf(text, fmt, ap);							// E converte i simboli in numeri attuali
	va_end(ap);											// Risultati salvati in text

	glPushAttrib(GL_LIST_BIT);							// Push del Display List Bits
	glListBase(base - 32);								// Imposta il carattere base a 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Disegna la Display List Text
	glPopAttrib();										// Pop del Display List Bits
}