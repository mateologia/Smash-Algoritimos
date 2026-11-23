#include "visual_personagem.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static bool ResolverCaminhoAsset(const char *caminhoRelativo, char *saida, size_t tamanho)
{
    if (caminhoRelativo == NULL || saida == NULL || tamanho == 0)
        return false;

    static const char *prefixos[] = {
        "Smash/",
        "",
        "../Smash/",
        "../",
        "../../Smash/",
        "../../",
    };

    for (size_t i = 0; i < sizeof(prefixos) / sizeof(prefixos[0]); i++)
    {
        int escritos = snprintf(saida, tamanho, "%s%s", prefixos[i], caminhoRelativo);
        if (escritos > 0 && (size_t)escritos < tamanho && FileExists(saida))
            return true;
    }

    int escritos = snprintf(saida, tamanho, "%s%s", GetApplicationDirectory(), caminhoRelativo);
    if (escritos > 0 && (size_t)escritos < tamanho && FileExists(saida))
        return true;

    return false;
}

static int AnimacaoFallback(const VisualPersonagem *visual)
{
    if (visual->animacaoDefinida[ANIM_IDLE])
        return ANIM_IDLE;

    for (int i = 0; i < ANIM_TOTAL; i++)
    {
        if (visual->animacaoDefinida[i])
            return i;
    }

    return -1;
}

static int IndiceAnimacaoParaEstado(const VisualPersonagem *visual, EstadoPersonagem estado)
{
    int id = -1;

    switch (estado)
    {
    case ESTADO_IDLE:
        id = ANIM_IDLE;
        break;
    case ESTADO_ANDANDO:
        id = ANIM_ANDANDO;
        break;
    case ESTADO_ATACANDO_SOCO_BAIXO:
    case ESTADO_ATACANDO_SOCO_ALTO:
        id = ANIM_ATACANDO_SOCO;
        break;
    case ESTADO_ATACANDO_CHUTE_BAIXO:
    case ESTADO_ATACANDO_CHUTE_ALTO:
        id = visual->animacaoDefinida[ANIM_ATACANDO_CHUTE] ? ANIM_ATACANDO_CHUTE : ANIM_ATACANDO_SOCO;
        break;
    case ESTADO_HIT:
        id = ANIM_HIT;
        break;
    case ESTADO_KO:
        id = ANIM_KO;
        break;
    default:
        break;
    }

    if (id >= 0 && id < ANIM_TOTAL && visual->animacaoDefinida[id])
        return id;

    return AnimacaoFallback(visual);
}

bool CarregarVisualPersonagem(VisualPersonagem *visual,
                              const ConfiguracaoVisualPersonagem *configuracao)
{
    if (visual == NULL || configuracao == NULL)
        return false;

    memset(visual, 0, sizeof(*visual));

    if (configuracao->folhas == NULL || configuracao->quantidadeFolhas <= 0 ||
        configuracao->definicoes == NULL || configuracao->quantidadeDefinicoes <= 0)
    {
        TraceLog(LOG_WARNING, "VISUAL: Configuracao incompleta");
        return false;
    }

    if (configuracao->quantidadeFolhas > MAX_FOLHAS_POR_VISUAL)
    {
        TraceLog(LOG_WARNING,
                 "VISUAL: Configuracao tem %d folhas, mas o limite e %d",
                 configuracao->quantidadeFolhas, MAX_FOLHAS_POR_VISUAL);
        return false;
    }

    for (int i = 0; i < configuracao->quantidadeFolhas; i++)
    {
        const DefinicaoFolhaSprite *def = &configuracao->folhas[i];
        char caminhoResolvido[1024];

        if (!ResolverCaminhoAsset(def->caminho, caminhoResolvido, sizeof(caminhoResolvido)))
        {
            TraceLog(LOG_WARNING, "VISUAL: Arquivo nao encontrado: %s", def->caminho);
            DescarregarVisualPersonagem(visual);
            return false;
        }

        if (!CarregarFolhaSprite(&visual->folhas[i], caminhoResolvido, def->colunas, def->linhas))
        {
            DescarregarVisualPersonagem(visual);
            return false;
        }

        visual->totalFolhas = i + 1;
    }

    visual->escala = configuracao->escala;
    if (visual->escala <= 0.0f)
    {
        float alturaFrame = (float)visual->folhas[0].alturaFrame;
        visual->escala = (alturaFrame > 0.0f) ? (ALTURA_CORPO / alturaFrame) : 1.0f;
    }

    visual->direcaoArte = (configuracao->direcaoArte == OLHANDO_ESQUERDA)
                              ? OLHANDO_ESQUERDA
                              : OLHANDO_DIREITA;

    int definidas = 0;

    for (int i = 0; i < configuracao->quantidadeDefinicoes; i++)
    {
        const DefinicaoAnimacaoPersonagem *def = &configuracao->definicoes[i];

        if (def->id < 0 || def->id >= ANIM_TOTAL)
        {
            TraceLog(LOG_WARNING, "VISUAL: Id de animacao invalido: %d", def->id);
            continue;
        }

        if (def->folha < 0 || def->folha >= visual->totalFolhas)
        {
            TraceLog(LOG_WARNING,
                     "VISUAL: Animacao %d referencia folha inexistente: %d",
                     def->id, def->folha);
            continue;
        }

        const FolhaSprite *folha = &visual->folhas[def->folha];

        if (def->quantidadeFrames <= 0)
        {
            TraceLog(LOG_WARNING, "VISUAL: Animacao %d sem frames", def->id);
            continue;
        }

        if (def->linha < 0 || def->linha >= folha->linhas ||
            def->colunaInicial < 0 ||
            def->colunaInicial + def->quantidadeFrames > folha->colunas)
        {
            TraceLog(LOG_WARNING,
                     "VISUAL: Recorte da animacao %d fora da folha (linha %d, coluna %d, %d frames)",
                     def->id, def->linha, def->colunaInicial, def->quantidadeFrames);
            continue;
        }

        visual->animacoes[def->id] = (Animacao){
            .indiceFolha = def->folha,
            .linha = def->linha,
            .colunaInicial = def->colunaInicial,
            .quantidadeFrames = def->quantidadeFrames,
            .duracaoFrame = (def->duracaoFrame > 0.0f) ? def->duracaoFrame : (1.0f / 60.0f),
            .loop = def->loop,
        };
        visual->animacaoDefinida[def->id] = true;
        definidas++;
    }

    if (definidas == 0)
    {
        TraceLog(LOG_WARNING, "VISUAL: Nenhuma animacao valida foi definida");
        DescarregarVisualPersonagem(visual);
        return false;
    }

    visual->carregado = true;
    return true;
}

void DescarregarVisualPersonagem(VisualPersonagem *visual)
{
    if (visual == NULL)
        return;

    for (int i = 0; i < visual->totalFolhas; i++)
        DescarregarFolhaSprite(&visual->folhas[i]);

    memset(visual, 0, sizeof(*visual));
}

void VincularVisualPersonagem(Personagem *personagem, VisualPersonagem *visual)
{
    if (personagem == NULL)
        return;

    personagem->visual = NULL;
    personagem->animador = (Animador){0};

    if (visual == NULL || !visual->carregado)
        return;

    personagem->visual = visual;

    int inicial = IndiceAnimacaoParaEstado(visual, personagem->estado);
    if (inicial < 0)
        return;

    IniciarAnimador(&personagem->animador,
                    visual->folhas, visual->totalFolhas,
                    visual->animacoes, ANIM_TOTAL,
                    inicial);
}

void AtualizarVisualPersonagem(Personagem *personagem, float deltaTime)
{
    if (personagem == NULL || personagem->visual == NULL ||
        !AnimadorValido(&personagem->animador))
        return;

    int alvo = IndiceAnimacaoParaEstado(personagem->visual, personagem->estado);
    if (alvo >= 0)
        TrocarAnimacao(&personagem->animador, alvo, false);

    bool reverso = false;
    if (alvo == ANIM_ANDANDO && personagem->direcaoMovimento != 0)
    {
        int sinalOlhar = (personagem->direcao == OLHANDO_DIREITA) ? 1 : -1;
        reverso = (personagem->direcaoMovimento != sinalOlhar);
    }
    DefinirAnimacaoReversa(&personagem->animador, reverso);

    AtualizarAnimador(&personagem->animador, deltaTime);
}

void DesenharPersonagem(const Personagem *personagem)
{
    if (personagem == NULL)
        return;

    bool desenhouSprite = false;
    float fatorAgachado = personagem->agachado ? (ALTURA_CORPO_AGACHADO / ALTURA_CORPO) : 1.0f;

    if (personagem->visual != NULL && AnimadorValido(&personagem->animador))
    {
        bool espelhar = (personagem->direcao != personagem->visual->direcaoArte);

        DesenharAnimador(&personagem->animador,
                         personagem->posicao,
                         espelhar,
                         personagem->visual->escala * fatorAgachado,
                         WHITE);

        desenhouSprite = true;
    }

    if (!desenhouSprite)
    {
        Color cor = personagem->cor;
        if (personagem->estado == ESTADO_HIT)
            cor = ORANGE;
        if (personagem->estado == ESTADO_KO)
            cor = DARKGRAY;

        DrawRectangleRec(personagem->hurtbox, cor);

        float alturaAtual = personagem->agachado ? ALTURA_CORPO_AGACHADO : ALTURA_CORPO;
        float olhoX = (personagem->direcao == OLHANDO_DIREITA)
                          ? (personagem->posicao.x + 15.0f)
                          : (personagem->posicao.x - 25.0f);
        DrawRectangle((int)olhoX, (int)(personagem->posicao.y - alturaAtual + 15.0f),
                      10, 10, WHITE);
    }
}

void DesenharPersonagemDebug(const Personagem *personagem)
{
    if (personagem == NULL)
        return;

    Color corCorpo = personagem->cor;
    if (personagem->estado == ESTADO_HIT)
        corCorpo = ORANGE;
    if (personagem->estado == ESTADO_KO)
        corCorpo = DARKGRAY;

    DrawRectangleLinesEx(personagem->hurtbox, 2.0f, corCorpo);

    if (personagem->hitboxAtiva)
    {
        DrawRectangleRec(personagem->hitbox, Fade(YELLOW, 0.35f));
        DrawRectangleLinesEx(personagem->hitbox, 2.0f, RED);
    }
}
