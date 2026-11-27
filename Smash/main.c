#include "raylib.h"
#include "definicoes.h"
#include "combate.h"
#include "jogo.h"
#include "visual_personagem.h"
#include "audio.h"

#include <math.h>
#include <time.h>

// Folhas do DIO (cada arquivo e uma unica linha de frames do mesmo tamanho)
static const DefinicaoFolhaSprite FOLHAS_DIO[] = {
    {
        .caminho = "animations/DioBrando/dio-idle-sheet.png",
        .colunas = 6,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-walking-sheet.png",
        .colunas = 16,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-attack-low-sheet.png",
        .colunas = 5,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-light-attack-up-sheet.png",
        .colunas = 9,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-heavy-attack-up.png",
        .colunas = 8,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-hit-mid-sheet.png",
        .colunas = 8,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-hit-up.png",
        .colunas = 8,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-defeated-sheet.png",
        .colunas = 31,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/dio-jumping-sheet.png",
        .colunas = 9,
        .linhas = 1,
    },
};

// Animacoes do DIO: ataques cobrem os 0.3s (TEMPO_ATAQUE) e o dano os 0.2s (TEMPO_HIT)
static const DefinicaoAnimacaoPersonagem ANIMACOES_DIO[] = {
    {
        .id = ANIM_IDLE,
        .folha = 0,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 6,
        .duracaoFrame = 0.12f,
        .loop = true,
    },
    {
        .id = ANIM_ANDANDO,
        .folha = 1,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 16,
        .duracaoFrame = 0.05f,
        .loop = true,
    },
    {
        .id = ANIM_ATAQUE_BAIXO,
        .folha = 2,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 5,
        .duracaoFrame = 0.06f,
        .loop = false,
    },
    {
        .id = ANIM_ATAQUE_ALTO_LEVE,
        .folha = 3,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 9,
        .duracaoFrame = 0.033f,
        .loop = false,
    },
    {
        .id = ANIM_ATAQUE_ALTO_PESADO,
        .folha = 4,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 8,
        .duracaoFrame = 0.0375f,
        .loop = false,
    },
    {
        .id = ANIM_HIT_BAIXO,
        .folha = 5,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 8,
        .duracaoFrame = 0.025f,
        .loop = false,
    },
    {
        .id = ANIM_HIT_ALTO,
        .folha = 6,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 8,
        .duracaoFrame = 0.025f,
        .loop = false,
    },
    {
        .id = ANIM_KO,
        .folha = 7,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 31,
        .duracaoFrame = 0.06f, // 1.86s: cabe na pausa de 2s do K.O.
        .loop = false,
    },
    {
        .id = ANIM_PULANDO,
        .folha = 8,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 9,
        .duracaoFrame = 0.033f, // 0.3s; depois segura a pose no ar ate pousar
        .loop = false,
    },
};

static const ConfiguracaoVisualPersonagem VISUAL_DIO = {
    .folhas = FOLHAS_DIO,
    .quantidadeFolhas = (int)(sizeof(FOLHAS_DIO) / sizeof(FOLHAS_DIO[0])),
    .escala = 0.0f,
    .definicoes = ANIMACOES_DIO,
    .quantidadeDefinicoes = (int)(sizeof(ANIMACOES_DIO) / sizeof(ANIMACOES_DIO[0])),
    .direcaoArte = OLHANDO_DIREITA,
};

// Folhas do BLANKA (a blanka-stunted e usada no FINISH HIM)
static const DefinicaoFolhaSprite FOLHAS_BLANKA[] = {
    {
        .caminho = "animations/Blanka/blanka-idle-sheet.png",
        .colunas = 3,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-walking-sheet.png",
        .colunas = 8,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-attack-low-sheet.png",
        .colunas = 3,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-light-attack-up-sheet.png",
        .colunas = 3,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-heavy-attack-up-sheet.png",
        .colunas = 4,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-hit-sheet.png", // vira o golpe baixo (hit-mid)
        .colunas = 5,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-hit-up-sheet.png",
        .colunas = 3,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-defeated-sheet.png",
        .colunas = 5,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-jumping-sheet.png",
        .colunas = 7,
        .linhas = 1,
    },
    {
        .caminho = "animations/Blanka/blanka-stunted-sheet.png",
        .colunas = 5,
        .linhas = 1,
    },
};

// Animacoes do BLANKA: idle e golpes marcados como "reversed after end" (ping-pong)
static const DefinicaoAnimacaoPersonagem ANIMACOES_BLANKA[] = {
    {
        .id = ANIM_IDLE,
        .folha = 0,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 3,
        .duracaoFrame = 0.12f,
        .loop = true,
        .pingPong = true,
    },
    {
        .id = ANIM_ANDANDO,
        .folha = 1,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 8,
        .duracaoFrame = 0.05f,
        .loop = true,
    },
    {
        .id = ANIM_ATAQUE_BAIXO,
        .folha = 2,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 3,
        .duracaoFrame = 0.06f, // 5 frames efetivos (3 ida + 2 volta) = 0.3s
        .loop = false,
        .pingPong = true,
    },
    {
        .id = ANIM_ATAQUE_ALTO_LEVE,
        .folha = 3,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 3,
        .duracaoFrame = 0.06f, // 5 frames efetivos = 0.3s
        .loop = false,
        .pingPong = true,
    },
    {
        .id = ANIM_ATAQUE_ALTO_PESADO,
        .folha = 4,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 4,
        .duracaoFrame = 0.0429f, // 7 frames efetivos (4 ida + 3 volta) = 0.3s
        .loop = false,
        .pingPong = true,
    },
    {
        .id = ANIM_HIT_BAIXO, // blanka-hit-sheet: dano de golpes baixos
        .folha = 5,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 5,
        .duracaoFrame = 0.04f,
        .loop = false,
    },
    {
        .id = ANIM_HIT_ALTO, // dano de golpes altos, em vaivem
        .folha = 6,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 3,
        .duracaoFrame = 0.04f, // 5 frames efetivos (3 ida + 2 volta) = 0.2s
        .loop = false,
        .pingPong = true,
    },
    {
        .id = ANIM_TONTO, // blanka-stunted: loop durante o FINISH HIM
        .folha = 9,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 5,
        .duracaoFrame = 0.12f,
        .loop = true,
    },
    {
        .id = ANIM_KO,
        .folha = 7,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 5,
        .duracaoFrame = 0.06f,
        .loop = false,
    },
    {
        .id = ANIM_PULANDO,
        .folha = 8,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 7,
        .duracaoFrame = 0.033f, // 0.23s; depois segura a pose no ar ate pousar
        .loop = false,
    },
};

static const ConfiguracaoVisualPersonagem VISUAL_BLANKA = {
    .folhas = FOLHAS_BLANKA,
    .quantidadeFolhas = (int)(sizeof(FOLHAS_BLANKA) / sizeof(FOLHAS_BLANKA[0])),
    .escala = 0.0f,
    .definicoes = ANIMACOES_BLANKA,
    .quantidadeDefinicoes = (int)(sizeof(ANIMACOES_BLANKA) / sizeof(ANIMACOES_BLANKA[0])),
    .direcaoArte = OLHANDO_DIREITA,
};

static void AplicarVisuaisSelecionados(const EstadoJogo *estado, Personagem *p1, Personagem *p2,
                                       VisualPersonagem *visualDio, VisualPersonagem *visualBlanka)
{
    VisualPersonagem *visuais[NUM_PERSONAGENS] = { visualDio, visualBlanka };

    if (estado->selecaoP1 >= 0 && estado->selecaoP1 < NUM_PERSONAGENS &&
        visuais[estado->selecaoP1] != NULL && visuais[estado->selecaoP1]->carregado)
    {
        VincularVisualPersonagem(p1, visuais[estado->selecaoP1]);
    }

    if (estado->selecaoP2 >= 0 && estado->selecaoP2 < NUM_PERSONAGENS &&
        visuais[estado->selecaoP2] != NULL && visuais[estado->selecaoP2]->carregado)
    {
        VincularVisualPersonagem(p2, visuais[estado->selecaoP2]);
    }
}

typedef struct
{
    Texture2D texturaFundo;   // back-background.png (camada distante com parallax)
    Texture2D texturaCenario; // background.png (camada principal logo atras dos lutadores)
} CenarioArena;

static Texture2D CarregarTexturaCenario(const char *caminhoRelativo)
{
    char caminhoResolvido[1024];
    if (!ResolverCaminhoAsset(caminhoRelativo, caminhoResolvido, sizeof(caminhoResolvido)))
    {
        TraceLog(LOG_WARNING, "CENARIO: Asset nao encontrado: %s", caminhoRelativo);
        return (Texture2D){0};
    }

    Texture2D textura = LoadTexture(caminhoResolvido);
    if (textura.id == 0)
    {
        TraceLog(LOG_WARNING, "CENARIO: Falha ao carregar textura: %s", caminhoResolvido);
        return (Texture2D){0};
    }

    SetTextureFilter(textura, TEXTURE_FILTER_POINT);
    SetTextureWrap(textura, TEXTURE_WRAP_CLAMP);
    return textura;
}

static void CarregarCenarioArena(CenarioArena *cenario)
{
    if (cenario == NULL)
        return;

    *cenario = (CenarioArena){0};
    cenario->texturaFundo = CarregarTexturaCenario("scenarios/back-background.png");
    cenario->texturaCenario = CarregarTexturaCenario("scenarios/background.png");
}

static void DescarregarCenarioArena(CenarioArena *cenario)
{
    if (cenario == NULL)
        return;

    if (cenario->texturaFundo.id != 0)
    {
        UnloadTexture(cenario->texturaFundo);
        cenario->texturaFundo = (Texture2D){0};
    }

    if (cenario->texturaCenario.id != 0)
    {
        UnloadTexture(cenario->texturaCenario);
        cenario->texturaCenario = (Texture2D){0};
    }
}

static void DesenharCenarioPrototipo(void)
{
    // Fallback geometrico caso a textura principal do cenario nao seja carregada
    DrawRectangle(0, 0, (int)LARGURA_ARENA, (int)CHAO_Y, (Color){ 235, 238, 245, 255 });
    DrawRectangle(0, (int)CHAO_Y, (int)LARGURA_ARENA, (int)(SCREEN_HEIGHT - CHAO_Y), (Color){ 210, 214, 222, 255 });

    for (int x = 100; x < (int)LARGURA_ARENA; x += 350)
    {
        DrawRectangle(x, 120, 48, (int)(CHAO_Y - 120), (Color){ 215, 220, 230, 255 });
        DrawRectangleLines(x, 120, 48, (int)(CHAO_Y - 120), (Color){ 185, 192, 205, 255 });
    }

    DrawRectangle(0, 0, 12, (int)CHAO_Y, DARKGRAY);
    DrawRectangle((int)LARGURA_ARENA - 12, 0, 12, (int)CHAO_Y, DARKGRAY);
    DrawLine(0, (int)CHAO_Y, (int)LARGURA_ARENA, (int)CHAO_Y, DARKGRAY);
}

static void DesenharCenarioFundo(const CenarioArena *cenario, const Camera2D *camera)
{
    if (cenario == NULL || cenario->texturaFundo.id == 0 || camera == NULL)
        return;

    Texture2D tex = cenario->texturaFundo;
    float escala = (float)SCREEN_HEIGHT / (float)tex.height;
    float largura = (float)tex.width * escala;
    float deslocamentoCamera = camera->target.x - (LARGURA_ARENA * 0.5f);

    float posX = ((float)SCREEN_WIDTH - largura) * 0.5f - (deslocamentoCamera * FATOR_PARALLAX_FUNDO);
    if (largura >= (float)SCREEN_WIDTH)
    {
        if (posX > 0.0f)
            posX = 0.0f;
        if (posX + largura < (float)SCREEN_WIDTH)
            posX = (float)SCREEN_WIDTH - largura;
    }

    Rectangle origem = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle destino = { posX, 0.0f, largura, (float)SCREEN_HEIGHT };
    DrawTexturePro(tex, origem, destino, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
}

static void DesenharCenarioFrente(const CenarioArena *cenario, const Camera2D *camera)
{
    if (cenario == NULL || cenario->texturaCenario.id == 0 || camera == NULL)
    {
        DesenharCenarioPrototipo();
        return;
    }

    Texture2D tex = cenario->texturaCenario;
    float escala = (float)SCREEN_HEIGHT / (float)tex.height;
    float larguraTile = (float)tex.width * escala;

    // Ancorado simetricamente no meio da arena para que os pilares centrais
    // emoldurem a posicao inicial dos dois jogadores.
    float origemX = (LARGURA_ARENA - larguraTile) * 0.5f;
    float esquerdaVisivel = camera->target.x - ((float)SCREEN_WIDTH * 0.5f);
    int primeiroTile = (int)floorf((esquerdaVisivel - origemX) / larguraTile);
    float x0 = origemX + ((float)primeiroTile * larguraTile);

    Rectangle origem = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle destinoA = { x0, 0.0f, larguraTile, (float)SCREEN_HEIGHT };
    Rectangle destinoB = { x0 + larguraTile, 0.0f, larguraTile, (float)SCREEN_HEIGHT };

    DrawTexturePro(tex, origem, destinoA, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    DrawTexturePro(tex, origem, destinoB, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
}

static void DesenharTextoComSombra(const char *texto, int x, int y, int tamanhoFonte, Color cor)
{
    DrawText(texto, x + 1, y + 1, tamanhoFonte, BLACK);
    DrawText(texto, x, y, tamanhoFonte, cor);
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jogo de Luta 2D - INF01202");
    SetTargetFPS(TARGET_FPS);

    // Semente para as variacoes aleatorias de som de hit
    SetRandomSeed((unsigned int)time(NULL));

    GerenciadorAudio audio = {0};
    InicializarAudio(&audio);
    TocarMusica(&audio, MUSICA_PILLAR_MEN);

    EstadoJogo estado = {0};
    InicializarEstadoJogo(&estado);

    Personagem p1 = {0}, p2 = {0};
    VisualPersonagem visualDio = {0};
    VisualPersonagem visualBlanka = {0};
    CenarioArena cenario = {0};

    InicializarPersonagens(&p1, &p2);
    CarregarCenarioArena(&cenario);

    if (!CarregarVisualPersonagem(&visualDio, &VISUAL_DIO))
    {
        TraceLog(LOG_WARNING, "Sprite do DIO nao carregado - fallback para retangulo de debug");
    }

    if (!CarregarVisualPersonagem(&visualBlanka, &VISUAL_BLANKA))
    {
        TraceLog(LOG_WARNING, "Sprite do BLANKA nao carregado - fallback para retangulo de debug");
    }

    bool mostrarCaixas = true;

    Camera2D camera = {
        .offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f },
        .target = { LARGURA_ARENA / 2.0f, SCREEN_HEIGHT / 2.0f },
        .rotation = 0.0f,
        .zoom = 1.0f
    };

    while (!WindowShouldClose() && !estado.sairJogo)
    {
        float deltaTime = GetFrameTime();
        TelaJogo telaAnterior = estado.telaAtual;

        // --- ATUALIZAÇÃO POR TELA ---
        switch (estado.telaAtual)
        {
            case TELA_MENU:
                AtualizarMenu(&estado, &audio);
                break;

            case TELA_SELECAO:
                AtualizarSelecao(&estado, &p1, &p2, &audio);
                if (estado.telaAtual == TELA_COMBATE)
                {
                    camera.target.x = LARGURA_ARENA / 2.0f;
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio, &visualBlanka);
                }
                break;

            case TELA_COMBATE:
                // Atualiza temporizador de 67s e verifica fim/troca de round
                if (AtualizarCombateRounds(&estado, &p1, &p2, deltaTime, &audio))
                {
                    camera.target.x = LARGURA_ARENA / 2.0f;
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio, &visualBlanka);
                }

                // Só processa golpes e movimento quando não estiver na pausa ("ROUND X" / "K.O.") ou cena de Fatality
                if (estado.pausaRound <= 0.0f && !estado.fatalityExecutado)
                {
                    AtualizarControles(&p1, &audio, deltaTime);
                    AtualizarControles(&p2, &audio, deltaTime);

                    AtualizarOrientacao(&p1, &p2);

                    AtualizarCaixas(&p1);
                    AtualizarCaixas(&p2);

                    ProcessarColisoes(&p1, &p2, &audio);
                }

                AtualizarVisualPersonagem(&p1, deltaTime);
                AtualizarVisualPersonagem(&p2, deltaTime);

                // Atualiza Câmera 2D deslizante seguindo o ponto médio entre P1 e P2 (BONUS-06)
                {
                    float pontoMedioX = (p1.posicao.x + p2.posicao.x) / 2.0f;
                    float minCameraX = SCREEN_WIDTH / 2.0f;
                    float maxCameraX = LARGURA_ARENA - (SCREEN_WIDTH / 2.0f);
                    if (pontoMedioX < minCameraX) pontoMedioX = minCameraX;
                    if (pontoMedioX > maxCameraX) pontoMedioX = maxCameraX;

                    camera.target.x += (pontoMedioX - camera.target.x) * SUAVIZACAO_CAMERA;
                }

                if (IsKeyPressed(KEY_F1))
                {
                    mostrarCaixas = !mostrarCaixas;
                }

                // Reiniciar partida completa (zera rounds) ao apertar 'R'
                if (IsKeyPressed(KEY_R))
                {
                    estado.roundsP1 = 0;
                    estado.roundsP2 = 0;
                    estado.roundAtual = 1;
                    camera.target.x = LARGURA_ARENA / 2.0f;
                    IniciarNovoRound(&estado, &p1, &p2, &audio);
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio, &visualBlanka);
                }
                break;

            case TELA_VITORIA:
                AtualizarVitoria(&estado, &audio);
                break;

            default:
                break;
        }

        // --- AUDIO: efeito de fim de jogo ao entrar na tela de vitoria ---
        if (estado.telaAtual != telaAnterior && estado.telaAtual == TELA_VITORIA)
        {
            TocarEfeito(&audio, SOM_FIM_DE_JOGO);
        }
        AtualizarAudio(&audio);

        // --- DESENHO POR TELA ---
        BeginDrawing();

        switch (estado.telaAtual)
        {
            case TELA_MENU:
                DesenharMenu(&estado);
                break;

            case TELA_SELECAO:
                DesenharSelecao(&estado);
                break;

            case TELA_COMBATE:
                ClearBackground((Color){ 18, 22, 36, 255 });

                // Camada mais distante do cenario (parallax lento em espaco de tela)
                DesenharCenarioFundo(&cenario, &camera);

                // --- MUNDO 2D COM CÂMERA DESLIZANTE (BONUS-06) ---
                BeginMode2D(camera);

                DesenharCenarioFrente(&cenario, &camera);

                DesenharPersonagem(&p1);
                DesenharPersonagem(&p2);

                if (mostrarCaixas)
                {
                    DesenharPersonagemDebug(&p1);
                    DesenharPersonagemDebug(&p2);
                }

                // Feedback visual de Bloqueio (BONUS-01) e Golpe Especial (BONUS-04) no espaço do mundo
                if (p1.bloqueando)
                {
                    DrawRectangleLinesEx(p1.hurtbox, 3, SKYBLUE);
                    DrawText("BLOCK", (int)p1.hurtbox.x, (int)p1.hurtbox.y - 22, 16, SKYBLUE);
                }
                if (p2.bloqueando)
                {
                    DrawRectangleLinesEx(p2.hurtbox, 3, SKYBLUE);
                    DrawText("BLOCK", (int)p2.hurtbox.x, (int)p2.hurtbox.y - 22, 16, SKYBLUE);
                }

                if (p1.estado == ESTADO_ESPECIAL)
                {
                    DrawText("SPECIAL!", (int)p1.hurtbox.x, (int)p1.hurtbox.y - 26, 20, GOLD);
                }
                if (p2.estado == ESTADO_ESPECIAL)
                {
                    DrawText("SPECIAL!", (int)p2.hurtbox.x, (int)p2.hurtbox.y - 26, 20, GOLD);
                }

                // Feixe de energia do Fatality no mundo (BONUS-05)
                if (estado.fatalityExecutado)
                {
                    float fx = estado.posicaoFatality.x;
                    float fy = estado.posicaoFatality.y;

                    DrawRectangle((int)(fx - 35), 0, 70, (int)fy, Fade(RED, 0.55f));
                    DrawRectangle((int)(fx - 14), 0, 28, (int)fy, Fade(GOLD, 0.85f));
                    DrawCircleLines((int)fx, (int)(fy - 70), 65.0f, RED);
                    DrawCircleLines((int)fx, (int)(fy - 70), 90.0f, GOLD);
                }

                EndMode2D();

                // --- HUD FIXA NA TELA ---
                if (estado.fatalityExecutado)
                {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.45f));

                    const char *bannerFatality = "FATALITY!";
                    int larguraFatality = MeasureText(bannerFatality, 68);
                    int tx = (SCREEN_WIDTH - larguraFatality) / 2;
                    int ty = SCREEN_HEIGHT / 2 - 45;
                    DrawText(bannerFatality, tx + 4, ty + 4, 68, BLACK);
                    DrawText(bannerFatality, tx, ty, 68, RED);
                }

                // Barras de Vida + Nome do Lutador Selecionado (REQ-21)
                const char *nomeP1 = ObterNomeLutador(estado.selecaoP1);
                DrawText(TextFormat("%s (1P)", nomeP1), 50, 16, 20, BLUE);
                DrawRectangle(50, 40, 300, 25, LIGHTGRAY);
                DrawRectangle(50, 40, (300 * p1.hp) / HP_MAX, 25, GREEN);
                DrawRectangleLines(50, 40, 300, 25, BLACK);
                DrawText(TextFormat("HP: %d/%d", p1.hp, HP_MAX), 50, 72, 18, DARKBLUE);

                const char *nomeP2 = ObterNomeLutador(estado.selecaoP2);
                int larguraNomeP2 = MeasureText(TextFormat("(2P) %s", nomeP2), 20);
                DrawText(TextFormat("(2P) %s", nomeP2), SCREEN_WIDTH - 50 - larguraNomeP2, 16, 20, RED);
                int vidaP2 = (300 * p2.hp) / HP_MAX;
                DrawRectangle(SCREEN_WIDTH - 350, 40, 300, 25, LIGHTGRAY);
                DrawRectangle(SCREEN_WIDTH - 50 - vidaP2, 40, vidaP2, 25, GREEN);
                DrawRectangleLines(SCREEN_WIDTH - 350, 40, 300, 25, BLACK);
                const char *textoHPp2 = TextFormat("HP: %d/%d", p2.hp, HP_MAX);
                DrawText(textoHPp2, SCREEN_WIDTH - 50 - MeasureText(textoHPp2, 18), 72, 18, MAROON);

                // Relógio de 67s, bolinhas de rounds vencidos e banners ("ROUND 1", "FIGHT!", "FINISH HIM!", "K.O.!")
                DesenharHUDRounds(&estado);

                if (p1.contadorCombo >= 2 && p1.tempoCombo > 0)
                {
                    DrawText(TextFormat("%d HIT COMBO!", p1.contadorCombo), 50, 105, 24, ORANGE);
                }
                if (p2.contadorCombo >= 2 && p2.tempoCombo > 0)
                {
                    const char *textoComboP2 = TextFormat("%d HIT COMBO!", p2.contadorCombo);
                    int larguraComboP2 = MeasureText(textoComboP2, 24);
                    DrawText(textoComboP2, SCREEN_WIDTH - 50 - larguraComboP2, 105, 24, ORANGE);
                }

                // Instruções na tela com fundo sombreado para legibilidade sobre o piso
                DrawRectangle(16, SCREEN_HEIGHT - 58, SCREEN_WIDTH - 32, 48, Fade(BLACK, 0.6f));
                DesenharTextoComSombra("P1: WASD (mover/pular/agachar) | C/F (socos) | V/G (chutes) | Especial: S -> D + C", 30, SCREEN_HEIGHT - 52, 14, RAYWHITE);
                DesenharTextoComSombra("P2: Setas (mover/pular/agachar) | N/J (socos) | M/K (chutes) | Especial: Baixo -> Esq + N", 30, SCREEN_HEIGHT - 30, 14, RAYWHITE);
                DesenharTextoComSombra("R: Reset | F1: Caixas", SCREEN_WIDTH - 175, SCREEN_HEIGHT - 30, 14, LIGHTGRAY);
                break;

            case TELA_VITORIA:
                DesenharVitoria(&estado);
                break;

            default:
                break;
        }

        EndDrawing();
    }

    DescarregarRecursosJogo(&estado);
    DescarregarCenarioArena(&cenario);
    DescarregarVisualPersonagem(&visualDio);
    DescarregarVisualPersonagem(&visualBlanka);
    DescarregarAudio(&audio);

    CloseWindow();
    return 0;
}