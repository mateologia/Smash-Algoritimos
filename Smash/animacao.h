#ifndef ANIMACAO_H
#define ANIMACAO_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_FOLHAS_POR_VISUAL 12

typedef struct
{
    Texture2D textura;
    int colunas;
    int linhas;
    int larguraFrame;
    int alturaFrame;

    float *pivosX;
    float *basesY;
} FolhaSprite;

typedef struct
{
    int indiceFolha;
    int linha;
    int colunaInicial;
    int quantidadeFrames;
    float duracaoFrame;
    bool loop;
    bool pingPong; // Reproduz de frente e, ao chegar no fim, volta para o inicio
} Animacao;

typedef struct
{
    const FolhaSprite *folhas;
    int totalFolhas;
    const Animacao *animacoes;
    int totalAnimacoes;

    int indiceAtual;
    int frameAtual;
    float tempoFrame;
    bool reverso;   // Inverte a leitura dos frames (ex.: andando para tras)
    bool invertido; // Direcao atual de uma animacao ping-pong (falso = avancando)
} Animador;

bool CarregarFolhaSprite(FolhaSprite *folha, const char *caminho, int colunas, int linhas);
void DescarregarFolhaSprite(FolhaSprite *folha);
bool FolhaSpriteValida(const FolhaSprite *folha);

void IniciarAnimador(Animador *animador, const FolhaSprite *folhas, int totalFolhas, const Animacao *animacoes, int totalAnimacoes, int indiceInicial);

bool AnimadorValido(const Animador *animador);
const Animacao *AnimacaoAtual(const Animador *animador);
const FolhaSprite *FolhaAtual(const Animador *animador);

bool TrocarAnimacao(Animador *animador, int indice, bool reiniciar);
void DefinirAnimacaoReversa(Animador *animador, bool reverso);
void AtualizarAnimador(Animador *animador, float deltaTime);
bool AnimadorTerminou(const Animador *animador);
int FrameAbsolutoAtual(const Animador *animador);
void DesenharAnimador(const Animador *animador, Vector2 posicaoPes, bool espelhar, float escala, Color tinta);

#endif
