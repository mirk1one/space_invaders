#include <vector>
#include <list>
#include <time.h>
#include "Vertex.h"
#include "Alien.h"
#include "Rock.h"
#include "Shot.h"

using namespace std;

#define N_LIFES 3                       // numero totale delle vite
#define N_LEVELS 5                      // numero totale livelli
#define MAX_ALIENS_PER_ROW 15           // massimo numero di alieni per riga
#define SCORE_FL_ALIEN 10               // punteggio alieni prima fila
#define SCORE_SL_ALIEN 20               // punteggio alieni seconda fila
#define SCORE_TL_ALIEN 30               // punteggio alieni terza fila
#define LEVEL_TIME 120                  // secondi per terminare il livello
#define FLOAT_SPACESHIP_MOVE 0.0002f    // valore float dello spostamento della navicella
#define FLOAT_DX_BORDER 0.9f            // valore in float del bordo destro
#define FLOAT_SX_BORDER -0.9f           // valore in float del bordo sinistro
#define FLOAT_MAX_DOWN_PER_ROW 0.1f     // massima discesa alieni quando toccano il bordo
#define FLOAT_MIN_DOWN_LEVEL -0.3f      // livello minimo oltre i quali gli alieni non scendono più

// OpenMenu: finestra di inizio gioco
// GameLevel: finestra di gioco
// EndLevel: finestra di livello 
enum class SceneType { OpenMenu, GameLevel, EndLevel };

class GameModel {
public:
    // Dati generali della finestra e dei controlli di input
    HDC			hDC;		    // Private GDI Device Context
    HGLRC		hRC;		    // Permanent Rendering Context
    HWND		hWnd;		    // Holds Our Window Handle
    HINSTANCE	hInstance;		// Holds The Instance Of The Application

    bool keys[256];			    // Array usato per le Keyboard Routine
    bool active;		        // Flag di finestra attiva impostato su TRUE per impostazione predefinita
    bool fullscreen;	        // Fullscreen Flag

    SceneType sceneType;        // Tipo di finestra visualizzata

    list<Shot> shots;           // Lista degli spari di alieni e navicella

    bool winLevel;              // True se ho vinto il livello
    int lifes;                  // Numero delle vite
    int scores;                 // Punteggio del gioco
    int level;                  // Numero del livello
    int nAliensPerRow;          // Numero degli alieni per riga

private:
    // limiti di proiezione in X e Y : x in[-plx, plx], y in[-ply, ply]
    double plx, ply;
    int wHeight, wWidth;                          // dimensioni finestra in pixel

    // model data
    vector<Vertex> backgroundVertexes;            // floating background
    vector<Vertex> lifesVertexes[N_LIFES];        // floating lifes
    vector<Vertex> buttonsVertexes[3];            // floating buttons

    clock_t tStamp;                               // Tempo di inizio dall'avvio del gioco
    clock_t tStart;                               // Tempo di avvio del livello
    clock_t tEndLevel;                            // Tempo di fine del livello
    double fullElapsed;                           // Tempo trascorso in secondi dall'inizio del programma

    Rock rocks[2];                                // Le due rocce

    Alien aliensFL[MAX_ALIENS_PER_ROW];           // Alieni first line
    Alien aliensSL[MAX_ALIENS_PER_ROW];           // Alieni second line
    Alien aliensTL[MAX_ALIENS_PER_ROW];           // Alieni third line

    int nAliensAlive;                             // Numero degli alieni vivi nel livello
    
    float levelTurnHoriz;                         // float che quando viene raggiunto mentre alieni scendono,
                                                  // iniziano a muoversi orizzontalmente
    bool aliensOnLeftBorder;                      // true se gli alieni hanno raggiunto il bordo sinistro
    bool aliensOnRightBorder;                     // true se gli alieni hanno raggiunto il bordo destro
    bool aliensOnDownLevel;                       // true se gli alieni hanno raggiunto il livello sotto successivo
    bool aliensOnMinDownLevel;                    // true se gli alieni hanno raggiunto il livello sotto minimo

    GLuint texture[12];			                  // Spazio per 12 Textures
    GLuint base;				                  // Lista base di visualizzazione dei font set

public:
    // inizializzazione gioco con livello 1
    GameModel() : hDC(NULL), hRC(NULL), hWnd(NULL), active(true), fullscreen(true), sceneType(SceneType::OpenMenu)
    {

        backgroundVertexes.clear();
        backgroundVertexes.push_back(Vertex(-1, -1, -5, 0, 0));
        backgroundVertexes.push_back(Vertex(1, -1, -5, 1, 0));
        backgroundVertexes.push_back(Vertex(1, 1, -5, 1, 1));
        backgroundVertexes.push_back(Vertex(-1, 1, -5, 0, 1));

        for (int i = 0; i < N_LIFES; i++)
        {
            lifesVertexes[i].clear();
            lifesVertexes[i].push_back(Vertex(-1, -1, -5, 0, 0));
            lifesVertexes[i].push_back(Vertex(1, -1, -5, 1, 0));
            lifesVertexes[i].push_back(Vertex(1, 1, -5, 1, 1));
            lifesVertexes[i].push_back(Vertex(-1, 1, -5, 0, 1));
        }

        for (int i = 0; i < 3; i++)
        {
            buttonsVertexes[i].clear();
            buttonsVertexes[i].push_back(Vertex(-1, -1, -5, 0, 0));
            buttonsVertexes[i].push_back(Vertex(1, -1, -5, 1, 0));
            buttonsVertexes[i].push_back(Vertex(1, 1, -5, 1, 1));
            buttonsVertexes[i].push_back(Vertex(-1, 1, -5, 0, 1));
        }

        SetLevelConfig(N_LIFES, 1, 0, 3, START_RANDOM_TIMES);

        // Inizializzazione timers
        tStamp = clock();
        fullElapsed = 0;

        // Posiziona le due rocce
        rocks[0] = Rock(-0.65, -0.4, 0.11);
        rocks[1] = Rock(0.65, -0.4, 0.11);
    }
    ~GameModel() {
        this->KillFont();
    }
    // Funzione per settare i parametri di inizio livello
    inline void SetLevelConfig(int lifesIn, int levelIn, int scoresIn, int nAliensPerRowIn, int alienRandomTimes)
    {
        tStart = clock();
        tEndLevel = tStart + (LEVEL_TIME * CLOCKS_PER_SEC);
        winLevel = false;
        lifes = lifesIn;
        nAliensAlive = nAliensPerRowIn * 3;
        level = levelIn;
        scores = scoresIn;
        nAliensPerRow = nAliensPerRowIn;
        levelTurnHoriz = 0.1;
        aliensOnLeftBorder = false;
        aliensOnRightBorder = false;
        aliensOnDownLevel = false;
        aliensOnMinDownLevel = false;

        // Svuota lista dagli spari
        shots.clear();

        // Carica gli alieni
        for (int i = 0; i < nAliensPerRow; i++)
        {
            float startPos = -0.30f - 0.15f * (level - 1);
            aliensFL[i] = Alien(startPos + i * 0.15f, 0.2f, AlienType::FirstLine, AlienDirection::Left, SCORE_FL_ALIEN, alienRandomTimes);
            aliensSL[i] = Alien(startPos + i * 0.15f, 0.3f, AlienType::SecondLine, AlienDirection::Left, SCORE_SL_ALIEN, alienRandomTimes);
            aliensTL[i] = Alien(startPos + i * 0.15f, 0.4f, AlienType::ThirdLine, AlienDirection::Left, SCORE_TL_ALIEN, alienRandomTimes);
        }
    }
    // Se i punti in pixel si trovano dentro la finestra
    inline bool IsInClient(int x, int y) {
        if (x >= 0 && x < wWidth && y >= 0 && y < wHeight) return true;
        return false;
    }
    bool DrawGLScene(void);
    bool InitGL(void);
    void ReSizeGLScene(int width, int height);
    void glPrint(const char* fmt, ...);			// Custom GL "Print" Routine
    void DrawOpenMenuScene(void);
    void DrawEndLevelScene(void);
    void DrawGameLevelScene(void);

private:
    bool LoadATexture(const char* name, int id);
    bool LoadGLTextures(void);
    void DrawElement(GLuint texture, int px, int py, GLfloat scalex, GLfloat scaley, std::vector<Vertex> vector);
    void DrawElementf(GLuint texture, float px, float py, GLfloat scalex, GLfloat scaley, std::vector<Vertex> vector);
    void BuildFont(void);
    void KillFont(void);
    // Conversione dalla distanza in pixel alla distanza X e Y in float
    inline float PixToCoord_X(int pix)
    {
        return (2.0f * (float)pix * (float)plx) / (float)wWidth;
    }
    inline float PixToCoord_Y(int pix)
    {
        return (2.0f * (float)pix * (float)ply) / (float)wHeight;
    }

};

extern class GameModel Data;