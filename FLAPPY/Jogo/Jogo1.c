#include "raylib.h"
#include <stdio.h>
#include <unistd.h>
//----------------------------------------------------------------------------------
// Algumas definições
//----------------------------------------------------------------------------------
#define MAX_TUBOS 100  // maximo de tubos do jogo
#define FLAPPY_RADIUS 25  // Aumente este valor para aumentar o tamanho do pássaro
#define TUBOS_WIDTH 80
//----------------------------------------------------------------------------------
// Tipos e Estruturas
//----------------------------------------------------------------------------------
typedef struct Flappy
{
    Vector2 position;
    int radius;
    Color color;
    Texture2D imagem;
} Flappy;

typedef struct Tubes
{
    Rectangle recTop; // Retângulo para o tubo superior
    Rectangle recBottom; // Retângulo para o tubo inferior
    Color color;
    bool active;
    Texture2D imagemTop;
    Texture2D imagemBottom;
} Tubos;
//------------------------------------------------------------------------------------
// Variáveis Globais
//------------------------------------------------------------------------------------
static const int screenWidth = 800;
static const int screenHeight = 450;

static bool gameOver = false;
static bool pause = false;
static int score = 0;
static int hiScore = 0;

static Flappy flappy = { 0 };
static Tubos tubos[MAX_TUBOS] = { 0 };
static Vector2 tubosPos[MAX_TUBOS] = { 0 };
static int tubosSpeedX = 0;
static bool superfx = false;

Texture2D background;  // Variável para a imagem de fundo
Texture2D tuboSuperior;  // Variável para a imagem do tubo superior
Texture2D tuboInferior;  // Variável para a imagem do tubo inferior
//------------------------------------------------------------------------------------
// Funções Locais
//------------------------------------------------------------------------------------
static void InitGame(void); // para iniciar o jogo
static void UpdateGame(void);
static void DrawGame(void);
static void UnloadGame(void);
static void UpdateDrawFrame(void);
//------------------------------------------------------------------------------------
// Ponto de entrada principal do programa
//------------------------------------------------------------------------------------
int main(void)
{
    // Inicialização
    InitWindow(screenWidth, screenHeight, "FLAPPYINF");//iniciar a janela com o nome.
    InitGame();//iniciar o jogo
    SetTargetFPS(60);//desempenho do jogo ta atrelado completamente ao jogo.
    

    // Loop do jogo
    while (!WindowShouldClose())    // Detectar botão de fechar ou tecla ESC
    {
        // Atualizar e Desenhar
        UpdateDrawFrame();
    }

    // Desinicialização
    UnloadGame();
    CloseWindow();

    return 0;
}
//------------------------------------------------------------------------------------
// Funções do Módulo (Locais)
//------------------------------------------------------------------------------------

// Inicialização do jogo
void InitGame(void)
{
    flappy.radius = FLAPPY_RADIUS;
    flappy.position = (Vector2)
    {
        80, screenHeight/2 - flappy.radius
    };
    tubosSpeedX = 4;// quanto maior o valor mais rapido ficam os tubos.

    flappy.imagem = LoadTexture("flappy.png"); //carregar textura do flappy
    background = LoadTexture("fundoflappy.png");  // Carregar a imagem de fundo
    tuboSuperior = LoadTexture("obstaculo.png");  // Carregar a imagem do tubo superior
    tuboInferior = LoadTexture("obstaculo2.png");  // Carregar a imagem do tubo inferior

    for (int i = 0; i < MAX_TUBOS; i++)
    {
        tubosPos[i].x =400 + 200*i;

        // Altura do espaço entre os tubos.
        int gapHeight = 200;

        // Altura aleatória para o tubo superior.
        int topTubeHeight = GetRandomValue(0, screenHeight - gapHeight - 100);

        tubos[i].recTop.x = tubosPos[i].x; //
        tubos[i].recTop.y = 0; //
        tubos[i].recTop.width = TUBOS_WIDTH; //
        tubos[i].recTop.height = topTubeHeight;// 

        tubos[i].recBottom.x = tubosPos[i].x;
        tubos[i].recBottom.y = topTubeHeight + gapHeight;
        tubos[i].recBottom.width = TUBOS_WIDTH;
        tubos[i].recBottom.height = screenHeight - tubos[i].recBottom.y;

        tubos[i].active = true;
    }

    score = 0;
    gameOver = false;
    superfx = false;
    pause = false;
}

// Atualização do jogo
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;

        if (!pause)
        {
            // Atualizar posição dos tubos
            for (int i = 0; i < MAX_TUBOS; i++) tubosPos[i].x -= tubosSpeedX;

            // Atualizar posição dos retângulos dos tubos
            for (int i = 0; i < MAX_TUBOS; i++)
            {
                tubos[i].recTop.x = tubosPos[i].x;
                tubos[i].recBottom.x = tubosPos[i].x;
            }
            // Mover o pássaro para cima quando a tecla de espaço é pressionada
            if (IsKeyPressed(KEY_SPACE) && !gameOver) flappy.position.y -= 20;
            else flappy.position.y += 1;

            //Checar colisão
            for (int i = 0; i < MAX_TUBOS; i++)
            {    
                // se a posição e o tamanho do flappy for proximo de algum tubo , o jogo acaba.
                if (CheckCollisionCircleRec(flappy.position, flappy.radius, tubos[i].recTop) &&
                        CheckCollisionCircleRec(flappy.position, flappy.radius, tubos[i].recBottom))
                {   
                    //acaba o jogo.
                    gameOver = true;
                    pause = false;
                }
                else if ((tubosPos[i].x < flappy.position.x) && tubos[i].active && !gameOver)
                {
                    // Verificar se passou pelo espaço do meio dos tubos
                    if ((flappy.position.y > tubos[i].recTop.y + tubos[i].recTop.height) && (flappy.position.y < tubos[i].recBottom.y))
                    {
                        score += 100;
                        tubos[i].active = false;

                        // se o score for maior que o ranking, então fica a ultima pontuação estabelecida.
                        if (score > hiScore) hiScore = score;
                    }
                    else
                    {
                        gameOver = true;
                        pause = false;
                    }
                }
            }
        }
    }
    else
    {
        // Reiniciar o jogo ao pressionar a tecla ENTER após o jogo terminar
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Desenhar na tela
void DrawGame(void)
{
    BeginDrawing();

    DrawTexture(background, 0, 0, WHITE);  // Desenhar a imagem de fundo

    if (!gameOver)
    {
        // Desenhar o pássaro
        Rectangle sourceRec = { 0, 0, flappy.imagem.width, flappy.imagem.height };
        Rectangle destRec = { flappy.position.x - flappy.radius, flappy.position.y - flappy.radius, 2 * flappy.radius, 2 * flappy.radius };
        Vector2 origin = { flappy.imagem.width / 2, flappy.imagem.height / 2 };
        DrawTexturePro(flappy.imagem, sourceRec, destRec, origin, 0, WHITE);

        // Desenhar os tubos
        for (int i = 0; i < MAX_TUBOS; i++)
        {
            DrawTexture(tuboSuperior, tubos[i].recTop.x, tubos[i].recTop.y, WHITE);
            DrawTexture(tuboInferior, tubos[i].recBottom.x, tubos[i].recBottom.y, WHITE);
        }


        // Mostrar pontuação na tela
        DrawText(TextFormat("%04i", score), 20, 20, 40, RED);
        DrawText(TextFormat("PONTUACAO: %04i", hiScore), 20, 70, 20, RED);

        // Mostrar mensagem de pausa se o jogo estiver pausado
        if (pause) DrawText("JOGO PAUSADO", screenWidth/2 - MeasureText("JOGO PAUSADO", 40)/2, screenHeight/2 - 40, 40, RED);
    }//telinha para aparecer a mensagem de pressionar enter para jogar novamente.
    else DrawText("PRESSIONE [ENTER] PARA JOGAR NOVAMENTE", GetScreenWidth()/2 - MeasureText("PRESSIONE [ENTER] PARA JOGAR NOVAMENTE", 20)/2, GetScreenHeight()/2 - 50, 20, RED);

    EndDrawing();
}

// Descarregar recursos
void UnloadGame(void)
{
    UnloadTexture(flappy.imagem);
    UnloadTexture(background);  // Descarregar a imagem de fundo
    UnloadTexture(tuboSuperior);  // Descarregar a imagem do tubo superior
    UnloadTexture(tuboInferior);  // Descarregar a imagem do tubo inferior
}

// Atualizar e desenhar um quadro
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}
