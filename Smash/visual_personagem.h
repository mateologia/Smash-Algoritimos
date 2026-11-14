#ifndef VISUAL_PERSONAGEM_H
#define VISUAL_PERSONAGEM_H

#include "definicoes.h"
#include "animacao.h"

typedef struct VisualPersonagem VisualPersonagem;

typedef enum
{
    ANIM_IDLE = 0,
    ANIM_ANDANDO,
    ANIM_ATACANDO_SOCO,
    ANIM_ATACANDO_CHUTE,
    ANIM_HIT,
    ANIM_KO,
    ANIM_TOTAL
} IdAnimacaoPersonagem;

typedef struct
{
    const char *caminho;
    int colunas;
    int linhas;
} DefinicaoFolhaSprite;

typedef struct
{
    IdAnimacaoPersonagem id;
    int folha;
    int linha;
    int colunaInicial;
    int quantidadeFrames;
    float duracaoFrame;
    bool loop;
} DefinicaoAnimacaoPersonagem;

typedef struct
{
    const DefinicaoFolhaSprite *folhas;
    int quantidadeFolhas;
    float escala;

    const DefinicaoAnimacaoPersonagem *definicoes;
    int quantidadeDefinicoes;

    DirecaoOlhar direcaoArte;
} ConfiguracaoVisualPersonagem;

struct VisualPersonagem
{
    FolhaSprite folhas[MAX_FOLHAS_POR_VISUAL];
    int totalFolhas;

    Animacao animacoes[ANIM_TOTAL];
    bool animacaoDefinida[ANIM_TOTAL];

    float escala;
    DirecaoOlhar direcaoArte;
    bool carregado;
};

bool CarregarVisualPersonagem(VisualPersonagem *visual,
                              const ConfiguracaoVisualPersonagem *configuracao);

void DescarregarVisualPersonagem(VisualPersonagem *visual);

void VincularVisualPersonagem(Personagem *personagem, VisualPersonagem *visual);

void AtualizarVisualPersonagem(Personagem *personagem, float deltaTime);

void DesenharPersonagem(const Personagem *personagem);

void DesenharPersonagemDebug(const Personagem *personagem);

#endif
