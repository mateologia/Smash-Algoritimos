#ifndef DEFINICOES_H
#define DEFINICOES_H

#include "raylib.h"

// Dimensões e parâmetros gerais do jogo
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 800
#define TARGET_FPS 60

#define HP_MAX 100
#define VELOCIDADE_PADRAO 5.0f
#define DANO_SOCO 10
#define DANO_CHUTE 15
#define ROUNDS_PARA_VENCER 2

// Estados das telas (Fluxo de Telas)
typedef enum {
    TELA_MENU = 0,
    TELA_SELECAO,
    TELA_COMBATE,
    TELA_VITORIA
} TelaJogo;

// Estados do personagem
typedef enum {
    ESTADO_IDLE = 0,
    ESTADO_ANDANDO,
    ESTADO_ATACANDO_SOCO,
    ESTADO_ATACANDO_CHUTE,
    ESTADO_HIT,
    ESTADO_KO
} EstadoPersonagem;

// Identificação da orientação
typedef enum {
    OLHANDO_DIREITA = 1,
    OLHANDO_ESQUERDA = -1
} DirecaoOlhar;

// Mapeamento de teclas de cada jogador
typedef struct {
    KeyboardKey cima;
    KeyboardKey baixo;
    KeyboardKey esquerda;
    KeyboardKey direita;
    KeyboardKey soco;
    KeyboardKey chute;
} MapeamentoTeclas;

// Estrutura do Personagem
typedef struct {
    int idPersonagem;
    Vector2 posicao;
    Vector2 velocidade;
    int hp;
    EstadoPersonagem estado;
    DirecaoOlhar direcao;

    // Caixas delimitadoras de colisão
    Rectangle hurtbox;
    Rectangle hitbox;
    bool hitboxAtiva;

    // Controlo de temporização (frames de ataque e reação a dano)
    int temporizadorEstado;
    MapeamentoTeclas controlos;
} Personagem;

// Estrutura global da partida
typedef struct {
    TelaJogo telaAtual;
    int roundsP1;
    int roundsP2;
    int roundAtual;
    int vencedorPartida; // 0: em curso, 1: P1, 2: P2
    Personagem p1;
    Personagem p2;
} EstadoJogo;

#endif
