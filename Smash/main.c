#include "raylib.h"
#include "definicoes.h"
#include "combate.h"
#include "jogo.h"
#include "visual_personagem.h"

static const DefinicaoFolhaSprite FOLHAS_DIO[] = {
    {
        .caminho = "animations/DioBrando/idle-sheet.png",
        .colunas = 6,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/attack-up-sheet.png",
        .colunas = 9,
        .linhas = 1,
    },
    {
        .caminho = "animations/DioBrando/walking-sheet.png",
        .colunas = 16,
        .linhas = 1,
    },
};

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
        .id = ANIM_ATACANDO_SOCO,
        .folha = 1,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 9,
        .duracaoFrame = 0.033f,
        .loop = false,
    },
    {
        .id = ANIM_ANDANDO,
        .folha = 2,
        .linha = 0,
        .colunaInicial = 0,
        .quantidadeFrames = 16,
        .duracaoFrame = 0.05f,
        .loop = true,
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

static void AplicarVisuaisSelecionados(const EstadoJogo *estado, Personagem *p1, Personagem *p2, VisualPersonagem *visualDio)
{
    if (visualDio->carregado)
    {
        if (estado->selecaoP1 == 0)
        {
            VincularVisualPersonagem(p1, visualDio);
        }
        if (estado->selecaoP2 == 0)
        {
            VincularVisualPersonagem(p2, visualDio);
        }
    }
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jogo de Luta 2D - INF01202");
    SetTargetFPS(TARGET_FPS);

    EstadoJogo estado = {0};
    InicializarEstadoJogo(&estado);

    Personagem p1 = {0}, p2 = {0};
    VisualPersonagem visualDio = {0};

    InicializarPersonagens(&p1, &p2);

    if (!CarregarVisualPersonagem(&visualDio, &VISUAL_DIO))
    {
        TraceLog(LOG_WARNING, "Sprite nao carregado - fallback para retangulo de debug");
    }

    bool mostrarCaixas = true;

    while (!WindowShouldClose() && !estado.sairJogo)
    {
        float deltaTime = GetFrameTime();

        // --- ATUALIZAÇÃO POR TELA ---
        switch (estado.telaAtual)
        {
            case TELA_MENU:
                AtualizarMenu(&estado);
                break;

            case TELA_SELECAO:
                AtualizarSelecao(&estado, &p1, &p2);
                if (estado.telaAtual == TELA_COMBATE)
                {
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio);
                }
                break;

            case TELA_COMBATE:
                // Atualiza temporizador de 67s e verifica fim/troca de round
                if (AtualizarCombateRounds(&estado, &p1, &p2, deltaTime))
                {
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio);
                }

                // Só processa golpes e movimento quando não estiver na pausa ("ROUND X" / "K.O.") ou cena de Fatality
                if (estado.pausaRound <= 0.0f && !estado.fatalityExecutado)
                {
                    AtualizarControles(&p1);
                    AtualizarControles(&p2);

                    AtualizarOrientacao(&p1, &p2);

                    AtualizarCaixas(&p1);
                    AtualizarCaixas(&p2);

                    ProcessarColisoes(&p1, &p2);
                }

                AtualizarVisualPersonagem(&p1, deltaTime);
                AtualizarVisualPersonagem(&p2, deltaTime);

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
                    IniciarNovoRound(&estado, &p1, &p2);
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio);
                }
                break;

            case TELA_VITORIA:
                AtualizarVitoria(&estado);
                break;

            default:
                break;
        }

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
                ClearBackground(RAYWHITE);

                DrawLine(0, CHAO_Y, SCREEN_WIDTH, CHAO_Y, DARKGRAY);

                DesenharPersonagem(&p1);
                DesenharPersonagem(&p2);

                if (mostrarCaixas)
                {
                    DesenharPersonagemDebug(&p1);
                    DesenharPersonagemDebug(&p2);
                }

                // Efeito visual do Fatality executado (BONUS-05)
                if (estado.fatalityExecutado)
                {
                    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.65f));

                    float fx = estado.posicaoFatality.x;
                    float fy = estado.posicaoFatality.y;

                    DrawRectangle((int)(fx - 35), 0, 70, (int)fy, Fade(RED, 0.55f));
                    DrawRectangle((int)(fx - 14), 0, 28, (int)fy, Fade(GOLD, 0.85f));
                    DrawCircleLines((int)fx, (int)(fy - 70), 65.0f, RED);
                    DrawCircleLines((int)fx, (int)(fy - 70), 90.0f, GOLD);

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
                DrawRectangle(50, 40, 3 * p1.hp, 25, GREEN);
                DrawRectangleLines(50, 40, 300, 25, BLACK);
                DrawText(TextFormat("HP: %d", p1.hp), 50, 72, 18, DARKBLUE);

                const char *nomeP2 = ObterNomeLutador(estado.selecaoP2);
                int larguraNomeP2 = MeasureText(TextFormat("(2P) %s", nomeP2), 20);
                DrawText(TextFormat("(2P) %s", nomeP2), SCREEN_WIDTH - 50 - larguraNomeP2, 16, 20, RED);
                DrawRectangle(SCREEN_WIDTH - 350, 40, 300, 25, LIGHTGRAY);
                DrawRectangle(SCREEN_WIDTH - 50 - (3 * p2.hp), 40, 3 * p2.hp, 25, GREEN);
                DrawRectangleLines(SCREEN_WIDTH - 350, 40, 300, 25, BLACK);
                DrawText(TextFormat("HP: %d", p2.hp), SCREEN_WIDTH - 115, 72, 18, MAROON);

                // Relógio de 67s, bolinhas de rounds vencidos e banners ("ROUND 1", "FIGHT!", "FINISH HIM!", "K.O.!")
                DesenharHUDRounds(&estado);

                // Feedback visual de Bloqueio (BONUS-01), Combos (BONUS-03) e Golpe Especial (BONUS-04)
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

                // Instruções na tela
                DrawText("P1: WASD (mover/pular/agachar) | C/F (socos) | V/G (chutes) | Especial: S -> D + C", 30, SCREEN_HEIGHT - 52, 14, DARKGRAY);
                DrawText("P2: Setas (mover/pular/agachar) | N/J (socos) | M/K (chutes) | Especial: Baixo -> Esq + N", 30, SCREEN_HEIGHT - 30, 14, DARKGRAY);
                DrawText("R: Reset | F1: Caixas", SCREEN_WIDTH - 175, SCREEN_HEIGHT - 30, 14, GRAY);
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
    DescarregarVisualPersonagem(&visualDio);

    CloseWindow();
    return 0;
}