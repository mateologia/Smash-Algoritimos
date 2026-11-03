#ifndef DEFINICOES_H
#define DEFINICOES_H

#include "raylib.h"
#include <stdbool.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define TARGET_FPS 60

#define CHAO_Y 500.0f
#define LARGURA_CORPO 80.0f
#define ALTURA_CORPO 140.0f
#define LARGURA_HITBOX 60.0f
#define ALTURA_HITBOX 40.0f

#define HP_MAX 100
#define VELOCIDADE_PADRAO 5.0f
#define DANO_SOCO 10
#define DANO_CHUTE 15

#define TEMPO_ATAQUE 18 // Duração total do ataque em frames (~300ms a 60 FPS)
#define TEMPO_HIT 12    // Duração do atordoamento ao levar golpe

typedef enum {
    ESTADO_IDLE = 0,
    ESTADO_ANDANDO,
    ESTADO_ATACANDO_SOCO,
    ESTADO_ATACANDO_CHUTE,
    ESTADO_HIT,
    ESTADO_KO
} EstadoPersonagem;

typedef enum {
    OLHANDO_DIREITA = 1,
    OLHANDO_ESQUERDA = -1
} DirecaoOlhar;

typedef struct {
    KeyboardKey esquerda;
    KeyboardKey direita;
    KeyboardKey soco;
    KeyboardKey chute;
} MapeamentoTeclas;

typedef struct {
    int id;
    Vector2 posicao; // Posição base (pé do personagem: x centralizado, y no chão)
    int hp;
    EstadoPersonagem estado;
    DirecaoOlhar direcao;

    Rectangle hurtbox;
    Rectangle hitbox;
    bool hitboxAtiva;

    int temporizador;
    MapeamentoTeclas controlos;
    Color cor;
} Personagem;

#endif
