#include <vector>
#include <math.h>
#include "Vertex.h"

using namespace std;

#pragma once
// Classe che gestisce le rocce
class Rock
{
public:
	float px;									// Posizione asse x
	float py;									// Posizione asse y
	float r;									// lunghezza raggio roccia
	vector<Vertex> rockVertexes;				// Floating rock

	Rock() : px(0.0), py(0.0), r(0.0)
	{
		rockVertexes.clear();
		rockVertexes.push_back(Vertex(-1, -1, -5, 0, 0));
		rockVertexes.push_back(Vertex(1, -1, -5, 1, 0));
		rockVertexes.push_back(Vertex(1, 1, -5, 1, 1));
		rockVertexes.push_back(Vertex(-1, 1, -5, 0, 1));
	}
	Rock(float px, float py, float r) : px(px), py(py), r(r)
	{
		rockVertexes.clear();
		rockVertexes.push_back(Vertex(-1, -1, -5, 0, 0));
		rockVertexes.push_back(Vertex(1, -1, -5, 1, 0));
		rockVertexes.push_back(Vertex(1, 1, -5, 1, 1));
		rockVertexes.push_back(Vertex(-1, 1, -5, 0, 1));
	}
	// Funzione che torna true se il punto di input si trova all'interno della roccia
	// per calcolare se il punto si trova nella roccia, basta immaginare che la roccia
	// sia considerata come un cerchio e che se si calcola la distanza tra il centro
	// e il punto, la distanza deve essere minore del raggio del cerchio
	inline bool HasIn(float ipx, float ipy)
	{
		float d = powf(r, 2) - (powf(px - ipx, 2) + powf(py - ipy, 2));
		return d >= 0;
	}
};