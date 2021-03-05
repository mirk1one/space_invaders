#include <vector>
#include "Vertex.h"

using namespace std;

#define FLOAT_SHOT_MOVE 0.0002f		// valore float movimento sparo

// AlienShot: sparo lanciato da un alieno
// SpaceshipShot: sparo lanciato dalla navicella
enum class ShotType { AlienShot, SpaceshipShot };

#pragma once
// Classe che gestisce gli spari
class Shot
{
public:
	float px;							// Posizione asse x
	float py;							// Posizione asse y
	bool toErase;						// Sparo da eliminare
	ShotType shotType;					// Tipo di sparo
	vector<Vertex> shotVertexes;		// Floating shot

	Shot(float px, float py, ShotType shotType) : px(px), py(py), shotType(shotType), toErase(false)
	{
		shotVertexes.clear();
		shotVertexes.push_back(Vertex(-1, -1, -5, 0, 0));
		shotVertexes.push_back(Vertex(1, -1, -5, 1, 0));
		shotVertexes.push_back(Vertex(1, 1, -5, 1, 1));
		shotVertexes.push_back(Vertex(-1, 1, -5, 0, 1));
	}
	// Funzione che muove in automatico lo sparo
	inline void Move()
	{
		// Se lo sparo è quello di un alieno, deve scendere
		if (shotType == ShotType::AlienShot)
			py -= FLOAT_SHOT_MOVE;
		// Altrimenti lo sparo è della navicella, deve salire
		else
			py += FLOAT_SHOT_MOVE;
	}
};