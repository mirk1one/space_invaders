#include <vector>
#include "Vertex.h"
#include "Shot.h"

using namespace std;

#define FLOAT_ALIEN_MOVE 0.0001f		// valore float movimento alieno
#define START_RANDOM_TIMES 60000		// probabilità di sparo 1/MAX_RANDOM_TIMES

// FirstLine: tipo alieno prima linea -> linea bassa
// SecondLine: tipo alieno seconda linea -> linea centrale
// ThirdLine: tipo alieno terza linea -> linea alta
enum class AlienType { FirstLine, SecondLine, ThirdLine, FourthLine };

// Left: si muove verso sinistra
// Right: si muove verso destra
// Down: si muove verso il basso
enum class AlienDirection { Left, Right, Down };

#pragma once
// Classe che gestisce gli alieni
class Alien
{
public:
	float px;							// Posizione asse x
	float py;							// Posizione asse y
	AlienType type;						// Tipo di alieno
	AlienDirection direction;			// Direzione alieno
	bool isAlive;						// True se l'alieno è ancora vivo
	int score;							// Punteggio alieno se ucciso
	int randomTimes;					// Probabilità di sparo
	vector<Vertex> alienVertexes;		// floating spaceship

	Alien() : px(0.0), py(0.0), type(AlienType::FirstLine),
		direction(AlienDirection::Left), isAlive(true), score(0), randomTimes(40000)
	{
		alienVertexes.clear();
		alienVertexes.push_back(Vertex(-1, -1, -5, 0, 0));
		alienVertexes.push_back(Vertex(1, -1, -5, 1, 0));
		alienVertexes.push_back(Vertex(1, 1, -5, 1, 1));
		alienVertexes.push_back(Vertex(-1, 1, -5, 0, 1));
		// Inizializzo il randomizzatore
		srand((unsigned)time(0));
	}

	Alien(float px, float py, AlienType type, AlienDirection direction, int score, int randomTimes)
		: px(px), py(py), type(type), direction(direction), isAlive(true), score(score), randomTimes(randomTimes)
	{
		alienVertexes.clear();
		alienVertexes.push_back(Vertex(-1, -1, -5, 0, 0));
		alienVertexes.push_back(Vertex(1, -1, -5, 1, 0));
		alienVertexes.push_back(Vertex(1, 1, -5, 1, 1));
		alienVertexes.push_back(Vertex(-1, 1, -5, 0, 1));
		// Inizializzo il randomizzatore
		srand((unsigned)time(0));
	}
	// Funzione che muove l'alieno a secondo della direzione
	// Left -> decresce il valore di px
	// Right -> cresce il valore di px
	// Down -> descresce il valore di py
	inline void Move()
	{
		if (direction == AlienDirection::Left)
			px -= FLOAT_ALIEN_MOVE;
		else if (direction == AlienDirection::Right)
			px += FLOAT_ALIEN_MOVE;
		else
			py -= FLOAT_ALIEN_MOVE;
	}
	// Dato un punto ip, vede se si trova all'interno dell'alieno
	// Questo per capire se un lasera ha colpito l'alieno
	// La forma dell'alieno è fatta così:
	//	  as ___               as = alto a sinistra
	//	    |___|
	//           bd            bd = basso a destra
	inline bool HasIn(float ipx, float ipy)
	{
		float lx = 0.05f;
		float ly = 0.035f;
		float asx = px - lx;
		float asy = py + ly;
		float bdx = px + lx;
		float bdy = py - ly;
		return asx <= ipx && ipx <= bdx && bdy <= ipy && ipy <= asy;
	}
	// Funzione che produce uno sparo randomico da parte dell'alieno
	// C'è una probabilità 1/MAX_RANDOM_TIMES che l'alieno produca uno sparo
	// Se questo numero random è uno, lo produce, altrimenti torna NULL
	inline Shot* MakeShot(void)
	{
		int randomNumber = (rand() % randomTimes) + 1;
		if (randomNumber == 1)
			return new Shot(px, py, ShotType::AlienShot);
		else
			return NULL;
	}
};