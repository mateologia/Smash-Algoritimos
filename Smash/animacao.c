#include "animacao.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ALFA_MINIMO 8

bool CarregarFolhaSprite(FolhaSprite *folha, const char *caminho, int colunas, int linhas)
{
    if (folha == NULL)
        return false;

    *folha = (FolhaSprite){0};

    if (caminho == NULL || colunas <= 0 || linhas <= 0)
    {
        TraceLog(LOG_WARNING, "ANIMACAO: Parametros invalidos ao carregar folha de sprites");
        return false;
    }

    if (!FileExists(caminho))
    {
        TraceLog(LOG_WARNING, "ANIMACAO: Folha de sprites nao encontrada: %s", caminho);
        return false;
    }

    Image imagem = LoadImage(caminho);
    if (imagem.data == NULL)
    {
        TraceLog(LOG_WARNING, "ANIMACAO: Falha ao carregar imagem: %s", caminho);
        return false;
    }

    if ((imagem.width % colunas) != 0 || (imagem.height % linhas) != 0)
    {
        TraceLog(LOG_WARNING,
                 "ANIMACAO: Imagem %s (%dx%d) nao e divisivel pela grade %dx%d",
                 caminho, imagem.width, imagem.height, colunas, linhas);
        UnloadImage(imagem);
        return false;
    }

    if (imagem.format != PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
        ImageFormat(&imagem, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Texture2D textura = LoadTextureFromImage(imagem);
    if (textura.id == 0)
    {
        TraceLog(LOG_WARNING, "ANIMACAO: Falha ao criar textura: %s", caminho);
        UnloadImage(imagem);
        return false;
    }

    SetTextureFilter(textura, TEXTURE_FILTER_POINT);

    folha->textura = textura;
    folha->colunas = colunas;
    folha->linhas = linhas;
    folha->larguraFrame = imagem.width / colunas;
    folha->alturaFrame = imagem.height / linhas;

    int totalFrames = colunas * linhas;
    folha->pivosX = (float *)calloc((size_t)totalFrames, sizeof(float));
    folha->basesY = (float *)calloc((size_t)totalFrames, sizeof(float));
    double *pesosColuna = (double *)calloc((size_t)folha->larguraFrame, sizeof(double));

    if (folha->pivosX != NULL && folha->basesY != NULL && pesosColuna != NULL)
    {
        for (int linha = 0; linha < folha->linhas; linha++)
        {
            for (int coluna = 0; coluna < folha->colunas; coluna++)
            {
                int indice = linha * folha->colunas + coluna;
                int origemX = coluna * folha->larguraFrame;
                int origemY = linha * folha->alturaFrame;

                memset(pesosColuna, 0, (size_t)folha->larguraFrame * sizeof(double));

                double pesoTotal = 0.0;
                int baseRelativa = folha->alturaFrame;

                for (int y = 0; y < folha->alturaFrame; y++)
                {
                    for (int x = 0; x < folha->larguraFrame; x++)
                    {
                        int alfa = GetImageColor(imagem, origemX + x, origemY + y).a;
                        if (alfa > ALFA_MINIMO)
                        {
                            pesosColuna[x] += (double)alfa;
                            pesoTotal += (double)alfa;
                            baseRelativa = y + 1;
                        }
                    }
                }

                float pivoX = (float)folha->larguraFrame * 0.5f;

                if (pesoTotal > 0.0)
                {
                    double acumulado = 0.0;
                    for (int x = 0; x < folha->larguraFrame; x++)
                    {
                        acumulado += pesosColuna[x];
                        if (acumulado >= pesoTotal * 0.5)
                        {
                            pivoX = (float)x + 0.5f;
                            break;
                        }
                    }
                }

                folha->pivosX[indice] = pivoX;
                folha->basesY[indice] = (float)baseRelativa;
            }
        }
    }
    else
    {
        free(folha->pivosX);
        free(folha->basesY);
        folha->pivosX = NULL;
        folha->basesY = NULL;
        TraceLog(LOG_WARNING,
                 "ANIMACAO: Sem memoria para calcular ancoras de %s; usando centro do frame",
                 caminho);
    }

    free(pesosColuna);
    UnloadImage(imagem);

    TraceLog(LOG_INFO, "ANIMACAO: Folha carregada: %s (%dx%d, frame %dx%d)",
             caminho, textura.width, textura.height,
             folha->larguraFrame, folha->alturaFrame);

    return true;
}

void DescarregarFolhaSprite(FolhaSprite *folha)
{
    if (folha == NULL)
        return;

    if (folha->textura.id != 0)
        UnloadTexture(folha->textura);

    free(folha->pivosX);
    free(folha->basesY);

    *folha = (FolhaSprite){0};
}

bool FolhaSpriteValida(const FolhaSprite *folha)
{
    return folha != NULL &&
           folha->textura.id != 0 &&
           folha->colunas > 0 &&
           folha->linhas > 0 &&
           folha->larguraFrame > 0 &&
           folha->alturaFrame > 0;
}

void IniciarAnimador(Animador *animador,
                     const FolhaSprite *folhas, int totalFolhas,
                     const Animacao *animacoes, int totalAnimacoes,
                     int indiceInicial)
{
    if (animador == NULL)
        return;

    *animador = (Animador){
        .folhas = folhas,
        .totalFolhas = totalFolhas,
        .animacoes = animacoes,
        .totalAnimacoes = totalAnimacoes,
        .indiceAtual = 0,
        .frameAtual = 0,
        .tempoFrame = 0.0f,
        .reverso = false,
    };

    if (totalAnimacoes > 0 && indiceInicial >= 0 && indiceInicial < totalAnimacoes)
        animador->indiceAtual = indiceInicial;
}

bool AnimadorValido(const Animador *animador)
{
    if (animador == NULL ||
        animador->folhas == NULL ||
        animador->animacoes == NULL ||
        animador->totalFolhas <= 0 ||
        animador->totalAnimacoes <= 0)
        return false;

    if (animador->indiceAtual < 0 || animador->indiceAtual >= animador->totalAnimacoes)
        return false;

    const Animacao *animacao = &animador->animacoes[animador->indiceAtual];

    return animacao->indiceFolha >= 0 &&
           animacao->indiceFolha < animador->totalFolhas &&
           FolhaSpriteValida(&animador->folhas[animacao->indiceFolha]) &&
           animacao->quantidadeFrames > 0 &&
           animador->frameAtual >= 0 &&
           animador->frameAtual < animacao->quantidadeFrames;
}

const Animacao *AnimacaoAtual(const Animador *animador)
{
    if (animador == NULL || animador->animacoes == NULL ||
        animador->indiceAtual < 0 || animador->indiceAtual >= animador->totalAnimacoes)
        return NULL;

    return &animador->animacoes[animador->indiceAtual];
}

const FolhaSprite *FolhaAtual(const Animador *animador)
{
    const Animacao *animacao = AnimacaoAtual(animador);
    if (animacao == NULL || animador->folhas == NULL ||
        animacao->indiceFolha < 0 || animacao->indiceFolha >= animador->totalFolhas)
        return NULL;

    return &animador->folhas[animacao->indiceFolha];
}

bool TrocarAnimacao(Animador *animador, int indice, bool reiniciar)
{
    if (animador == NULL || animador->animacoes == NULL ||
        indice < 0 || indice >= animador->totalAnimacoes)
        return false;

    if (indice == animador->indiceAtual && !reiniciar)
        return false;

    if (animador->animacoes[indice].quantidadeFrames <= 0)
        return false;

    animador->indiceAtual = indice;
    animador->frameAtual = 0;
    animador->tempoFrame = 0.0f;
    animador->reverso = false;

    return true;
}

void DefinirAnimacaoReversa(Animador *animador, bool reverso)
{
    if (animador == NULL)
        return;

    animador->reverso = reverso;
}

void AtualizarAnimador(Animador *animador, float deltaTime)
{
    const Animacao *animacao = AnimacaoAtual(animador);
    if (animacao == NULL || animacao->quantidadeFrames <= 0)
        return;

    if (deltaTime <= 0.0f)
        return;

    float duracao = animacao->duracaoFrame > 0.0f
                        ? animacao->duracaoFrame
                        : (1.0f / 60.0f);

    animador->tempoFrame += deltaTime;

    int seguranca = 0;
    while (animador->tempoFrame >= duracao && seguranca < 1000)
    {
        animador->tempoFrame -= duracao;

        if (animador->reverso)
        {
            if (animador->frameAtual > 0)
            {
                animador->frameAtual--;
            }
            else if (animacao->loop)
            {
                animador->frameAtual = animacao->quantidadeFrames - 1;
            }
            else
            {
                animador->frameAtual = 0;
                animador->tempoFrame = 0.0f;
                break;
            }
        }
        else if (animador->frameAtual + 1 < animacao->quantidadeFrames)
        {
            animador->frameAtual++;
        }
        else if (animacao->loop)
        {
            animador->frameAtual = 0;
        }
        else
        {
            animador->frameAtual = animacao->quantidadeFrames - 1;
            animador->tempoFrame = 0.0f;
            break;
        }

        seguranca++;
    }
}

bool AnimadorTerminou(const Animador *animador)
{
    const Animacao *animacao = AnimacaoAtual(animador);
    if (animacao == NULL || animacao->loop)
        return false;

    if (animador->reverso)
        return animador->frameAtual == 0;

    return animador->frameAtual == animacao->quantidadeFrames - 1;
}

int FrameAbsolutoAtual(const Animador *animador)
{
    const Animacao *animacao = AnimacaoAtual(animador);
    const FolhaSprite *folha = FolhaAtual(animador);
    if (animacao == NULL || folha == NULL)
        return -1;

    return animacao->linha * folha->colunas +
           animacao->colunaInicial + animador->frameAtual;
}

void DesenharAnimador(const Animador *animador, Vector2 posicaoPes,
                      bool espelhar, float escala, Color tinta)
{
    const Animacao *animacao = AnimacaoAtual(animador);
    const FolhaSprite *folha = FolhaAtual(animador);

    if (animacao == NULL || folha == NULL || escala <= 0.0f)
        return;

    int coluna = animacao->colunaInicial + animador->frameAtual;
    if (coluna < 0 || coluna >= folha->colunas)
        return;
    if (animacao->linha < 0 || animacao->linha >= folha->linhas)
        return;

    Rectangle origem = {
        (float)(coluna * folha->larguraFrame),
        (float)(animacao->linha * folha->alturaFrame),
        (float)folha->larguraFrame,
        (float)folha->alturaFrame,
    };

    if (espelhar)
        origem.width = -origem.width;

    int indiceFrame = animacao->linha * folha->colunas + coluna;

    float pivoX = folha->larguraFrame * 0.5f;
    float baseY = (float)folha->alturaFrame;

    if (folha->pivosX != NULL && folha->basesY != NULL)
    {
        pivoX = folha->pivosX[indiceFrame];
        baseY = folha->basesY[indiceFrame];
    }

    float ancoraX = espelhar ? ((float)folha->larguraFrame - pivoX) : pivoX;

    float largura = folha->larguraFrame * escala;
    float altura = folha->alturaFrame * escala;

    Rectangle destino = {
        posicaoPes.x - (ancoraX * escala),
        posicaoPes.y - (baseY * escala),
        largura,
        altura,
    };

    DrawTexturePro(folha->textura, origem, destino, (Vector2){0.0f, 0.0f}, 0.0f, tinta);
}
