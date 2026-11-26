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

// Vida e dano: x = vida maxima de cada lutador, derivada para que os golpes
// encaixem exatos na vida maxima (8 golpes leves ou 6 golpes pesados levam ao K.O.)
#define HP_MAX 96
#define DANO_ATAQUE_LEVE (HP_MAX / 8)   // 12: golpes com hitbox baixa + light-attack-up
#define DANO_ATAQUE_PESADO (HP_MAX / 6) // 16: heavy-attack-up

#define VELOCIDADE_PADRAO 5.0f
#define TEMPO_ATAQUE 18 // Duração total do ataque em frames (~300ms a 60 FPS)
#define JANELA_HITBOX_INICIAL 14 // Primeiro frame em que a hitbox do ataque fica ativa
#define JANELA_HITBOX_FINAL 6    // Último frame em que a hitbox do ataque fica ativa
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

// Macros para BONUS-04 (Golpes Especiais via Sequência de Comandos)
#define DANO_ESPECIAL 28
#define LARGURA_HITBOX_ESPECIAL 130.0f
#define ALTURA_HITBOX_ESPECIAL 70.0f
#define JANELA_COMANDO_ESPECIAL 18

// Macros para BONUS-05 (Finishing Moves / Fatalities)
#define TEMPO_FINISH_HIM 4.0f
#define JANELA_COMANDO_FATALITY 24
#define ALCANCE_FATALITY 180.0f

// Macros para BONUS-06 (Cenário Dinâmico e Câmera 2D Deslizante)
#define LARGURA_ARENA 1600.0f
#define SUAVIZACAO_CAMERA 0.12f

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
    ESTADO_ESPECIAL,
    ESTADO_TONTO,
    ESTADO_FATALITY,
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
    bool levouGolpeAlto; // true se o ultimo golpe recebido mirou em cima (escolhe a animacao de dano)
    bool golpeAplicado;  // true quando o ataque atual ja conectou (cada ataque acerta uma unica vez)
    int contadorCombo;
    int tempoCombo;

    int temporizador;
    MapeamentoTeclas controles;
    Color cor;
    struct VisualPersonagem *visual;

    float velocidadeY;
    bool agachado;

    int etapaEspecial; // 0 = nada, 1 = apertou Baixo, 2 = apertou Baixo -> Frente
    int timerEspecial; // Janela em frames para completar o próximo passo

    int etapaFatality; // 0 = nada, 1 = apertou Baixo 1x, 2 = apertou Baixo 2x
    int timerFatality; // Janela em frames para completar o Fatality

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
    bool emFinishHim;
    float timerFinishHim;
    bool fatalityExecutado;
    float timerEfeitoFatality;
    Vector2 posicaoFatality;

    Texture2D retratosLutadores[NUM_PERSONAGENS];
    bool retratosCarregados;
} EstadoJogo;

#endif