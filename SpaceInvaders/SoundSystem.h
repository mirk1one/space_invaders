#include <vector>
#include "Vertex.h"
#include "audiere.h"

using namespace audiere;

#pragma once
// La classe per gestire i suoni del gioco
class SoundSystemModel
{
private:
	AudioDevicePtr device;
	OutputStreamPtr stream;
	OutputStreamPtr fire;
	OutputStreamPtr explosion;
	OutputStreamPtr blow;
	OutputStreamPtr scream;
	OutputStreamPtr splat;
	OutputStreamPtr win;

public:
	SoundSystemModel() { }
private:
	// Esegue un suono passando lo stream di output e il livello del suono
	inline void ExecuteSound(OutputStreamPtr sound, float levelSound)
	{
		// Setto la percentuale di volume
		sound->setVolume(levelSound);
		// Se il suono è ancora attivo, lo rimuovo
		if (sound->isPlaying())
			sound->reset();
		// altrimenti lo eseguo
		else sound->play();
	}
public:
	// Inizializza i suoni
	inline bool InitilizeSounds()
	{
		// Apro il device di suono e carico tutti i suoni
		device = AudioDevicePtr(OpenDevice());
		stream = OutputStreamPtr(OpenSound(device, "../Data/principal.mp3", true));
		if (!stream) return false;
		fire = OutputStreamPtr(OpenSound(device, "../Data/fire.wav", false));
		if (!fire) return false;
		explosion = OutputStreamPtr(OpenSound(device, "../Data/explosion.wav", false));
		if (!explosion) return false;
		blow = OutputStreamPtr(OpenSound(device, "../Data/blow.wav", false));
		if (!blow) return false;
		scream = OutputStreamPtr(OpenSound(device, "../Data/scream.wav", false));
		if (!scream) return false;
		splat = OutputStreamPtr(OpenSound(device, "../Data/splat.wav", false));
		if (!splat) return false;
		win = OutputStreamPtr(OpenSound(device, "../Data/win.wav", false));
		if (!win) return false;

		return true;
	}
	// Esegue in ripetizione il sottofondo di gioco
	inline void PlayGameSound()
	{
		// Ripeto il suono all'infinito
		stream->setRepeat(true);
		// Setto il 50% di volume
		stream->setVolume(0.5f);
		// Eseguo il sottofondo
		stream->play();
	}
	// Esegue il suono di fuoco
	inline void PlayFire() { ExecuteSound(fire, 1.0f); }
	// Esegue il suono dell'esplosione
	inline void PlayExplosion() { ExecuteSound(explosion, 1.0f); }
	// Esegue il suono del colpo
	inline void PlayBlow() { ExecuteSound(blow, 0.2f); }
	// Esegue il suono di un urlo
	inline void PlayScream() { ExecuteSound(scream, 1.0f); }
	// Esegue il suono di un tonfo
	inline void PlaySplat() { ExecuteSound(splat, 1.0f); }
	// Esegue il suono di vittoria
	inline void PlayWin() { ExecuteSound(win, 1.0f); }
};

extern class SoundSystemModel SoundSystem;