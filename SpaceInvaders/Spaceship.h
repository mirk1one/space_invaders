#include <vector>
#include "Vertex.h"

using namespace std;

#define FLOAT_START_POSITION_X 0.0f		// Posizione iniziale navicella x
#define FLOAT_START_POSITION_Y -0.6f	// Posizione iniziale navicella y
#define FLOAT_LENGTH_X = 0.075f			// Lunghezza tra centro/ala dx e centro/ala sx della navicella
#define FLOAT_LENGTH_Y = 0.065f			// Lunghezza tra centro/punta centro/fondo della navicella

#pragma once
// Classe che gestisce la navicella
class SpaceshipModel
{
public:
	float px;								// Posizione asse x
	float py;								// Posizione asse y
	vector<Vertex> spaceshipVertexes;       // Floating spaceship

	SpaceshipModel() : px(FLOAT_START_POSITION_X), py(FLOAT_START_POSITION_Y)
	{
		spaceshipVertexes.clear();
		spaceshipVertexes.push_back(Vertex(-1, -1, -5, 0, 0));
		spaceshipVertexes.push_back(Vertex(1, -1, -5, 1, 0));
		spaceshipVertexes.push_back(Vertex(1, 1, -5, 1, 1));
		spaceshipVertexes.push_back(Vertex(-1, 1, -5, 0, 1));
	}
private:
	// Funzione per il calcolo del punto nel triangolo
	inline float sign(float x1, float y1, float x2, float y2, float x3, float y3)
	{
		return (x1 - x3) * (y2 - y3) - (x2 - x3) * (y1 - y3);
	}
public:
	// Dato un punto ip, vede se si trova all'interno della navicella
	// Questo per capire se una bomba ha colpito la navicella
	// La forma della navicella è fatta così:
	// ac = alto al centro    pc = punto centrale (px,py) della navicella
	//       ac			      ms = medio a sinistra
	//       /\			      md = medio a destra
	//	  ms/pc\md            bs = basso a sinistra
	//	  bs|__|bd            bd = basso a destra
	inline bool HasIn(float ipx, float ipy)
	{
		float acx = px;
		float acy = py + 0.06f;
		float msx = px - 0.06f;
		float msy = py - 0.02f;
		float mdx = px + 0.06f;
		float mdy = py - 0.02f;
		//float bsx = px - 0.06f;	<--- al fine dei calcoli è inutile
		//float bsy = py - 0.065f;	<--- al fine dei calcoli è inutile
		float bdx = px + 0.06f;
		float bdy = py - 0.065f;
		// Guardo se il punto si trova nel rettangolo in basso
		if (msx <= ipx && ipx <= bdx && bdy <= ipy && ipy <= msy)
			return true;
		// Guardo se il punto si trova nel triangolo in alto
		float d1, d2, d3;
		bool has_neg, has_pos;
		d1 = sign(ipx, ipy, msx, msy, mdx, mdy);
		d2 = sign(ipx, ipy, mdx, mdy, acx, acy);
		d3 = sign(ipx, ipy, acx, acy, msx, msy);
		has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
		return !(has_neg && has_pos);
	}
	// Funzione che produce uno sparo da parte della navicella
	inline Shot MakeShot(void)
	{
		return Shot(px, py, ShotType::SpaceshipShot);
	}
};

extern class SpaceshipModel Spaceship;