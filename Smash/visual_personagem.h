#ifndef VISUAL_PERSONAGEM_H
#define VISUAL_PERSONAGEM_H

#include "definicoes.h"
#include "animacao.h"
#include <stddef.h>

typedef struct VisualPersonagem VisualPersonagem;

// Procura o asset nos prefixos usados pelo jogo (raiz do projeto, pasta Smash, ...)
bool ResolverCaminhoAsset(const char *caminhoRelativo, char *saida, size_t tamanho);

typedef enum
{
    ANIM_IDLE = 0,
    ANIM_ANDANDO,
    ANIM_PULANDO,           // jumping: substitui idle/andando enquanto esta no ar
    ANIM_ATAQUE_BAIXO,      // attack-low: hitbox projetada rente ao chao
    ANIM_ATAQUE_ALTO_LEVE,  // light-attack-up: hitbox alta, golpe leve
    ANIM_ATAQUE_ALTO_PESADO,// heavy-attack-up: hitbox alta, golpe pesado
    ANIM_HIT_BAIXO,         // hit-mid: dano recebido de golpes baixos
    ANIM_HIT_ALTO,          // hit-up: dano recebido de golpes altos
    ANIM_HIT,               // dano generico (quando o lutador nao distingue a altura)
    ANIM_TONTO,             // atordoado no FINISH HIM (Blanka usa a folha stunted)
    ANIM_KO,                // defeat
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
    bool pingPong; // Ao terminar, reproduz invertida ate voltar ao primeiro frame
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
