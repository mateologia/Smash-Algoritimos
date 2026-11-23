#ifndef DEFINICOES_H
#define DEFINICOES_H

#include "raylib.h"
#include "animacao.h"
#include <stdbool.h>

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 600
#define VIRTUAL_WIDTH 400
#define VIRTUAL_HEIGHT 254
#define TARGET_FPS 60

#define CHAO_Y 500.0f
#define LARGURA_CORPO 80.0f
#define ALTURA_CORPO 140.0f
#define LARGURA_HITBOX 60.0f
#define ALTURA_HITBOX 40.0f

#define HP_MAX 100
#define VELOCIDADE_PADRAO 5.0f
#define DANO_SOCO_BAIXO 8
#define DANO_SOCO_ALTO 12
#define DANO_CHUTE_BAIXO 10
#define DANO_CHUTE_ALTO 16

#define TEMPO_ATAQUE 18 // Duração total do ataque em frames (~300ms a 60 FPS)
#define TEMPO_HIT 12    // Duração do atordoamento ao levar golpe

#define NUM_PERSONAGENS 2
#define ROUNDS_PARA_VENCER 2
#define TEMPO_MAXIMO_ROUND 67.0f

// Macros para BONUS-01 (Bloqueio) e BONUS-03 (Combos)
#define FATOR_DANO_BLOQUEIO 0.25f   // Bloqueio reduz o dano para 25% (75% de redução)
#define TEMPO_BLOQUEIO 8            // Frames de recuperação após defender um golpe
#define MULTIPLICADOR_COMBO 0.15f   // +15% de bônus de dano por hit consecutivo no combo
#define JANELA_COMBO 45             // Frames para manter o contador de combo visível/ativo

// Macros para BONUS-02 (Movimentação Vertical: Pulo e Agachar)
#define GRAVIDADE 0.8f
#define FORCA_PULO -16.0f
#define ALTURA_CORPO_AGACHADO 80.0f

// Macro para BONUS-09 (Persistência de Histórico em .txt)
#define ARQUIVO_HISTORICO "historico_partidas.txt"

struct VisualPersonagem;

typedef enum {
    TELA_MENU = 0,
    TELA_SELECAO,
    TELA_COMBATE,
    TELA_VITORIA
} TelaJogo;

typedef enum
{
    ESTADO_IDLE = 0,
    ESTADO_ANDANDO,
    ESTADO_ATACANDO_SOCO_BAIXO,
    ESTADO_ATACANDO_SOCO_ALTO,
    ESTADO_ATACANDO_CHUTE_BAIXO,
    ESTADO_ATACANDO_CHUTE_ALTO,
    ESTADO_HIT,
    ESTADO_KO
} EstadoPersonagem;

typedef enum
{
    OLHANDO_DIREITA = 1,
    OLHANDO_ESQUERDA = -1
} DirecaoOlhar;

typedef struct
{
    KeyboardKey cima;
    KeyboardKey baixo;
    KeyboardKey esquerda;
    KeyboardKey direita;
    KeyboardKey socoBaixo;
    KeyboardKey socoAlto;
    KeyboardKey chuteBaixo;
    KeyboardKey chuteAlto;
} MapeamentoTeclas;

typedef struct Personagem
{
    int id;
    Vector2 posicao; // Posição base (pé do personagem: x centralizado, y no chão)
    int hp;
    EstadoPersonagem estado;
    DirecaoOlhar direcao;
    int direcaoMovimento; // -1 = esquerda, 0 = parado, +1 = direita

    Rectangle hurtbox;
    Rectangle hitbox;
    bool hitboxAtiva;

    bool bloqueando;
    int contadorCombo;
    int tempoCombo;

    int temporizador;
    MapeamentoTeclas controles;
    Color cor;
    struct VisualPersonagem *visual;

    float velocidadeY;
    bool agachado;

    Animador animador;
} Personagem;

typedef struct EstadoJogo
{
    TelaJogo telaAtual;
    int opcaoMenu;      // 0: Novo Jogo, 1: Sair
    int selecaoP1;      // Índice do lutador escolhido pelo P1 (0 ou 1)
    int selecaoP2;      // Índice do lutador escolhido pelo P2 (0 ou 1)
    bool prontoP1;
    bool prontoP2;
    int roundsP1;
    int roundsP2;
    int roundAtual;     // Round atual (1, 2, 3...)
    float tempoRound;   // Cronômetro regressivo de 67 segundos
    float pausaRound;   // Pausa para exibir "ROUND X - FIGHT!" ou "K.O."
    bool encerrandoRound;
    int vencedorPartida; // 1 = P1, 2 = P2, 0 = Empate
    int historicoVitoriasP1;
    int historicoVitoriasP2;
    int historicoEmpates;
    bool sairJogo;

    Texture2D retratosLutadores[NUM_PERSONAGENS];
    bool retratosCarregados;
} EstadoJogo;

#endif