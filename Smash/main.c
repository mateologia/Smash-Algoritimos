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

// Função auxiliar para vincular os sprites carregados aos personagens após inicializá-los
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

    EstadoJogo estado;
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
                // Assim que a seleção confirmar e mudar para a tela de combate, vincula o sprite
                if (estado.telaAtual == TELA_COMBATE)
                {
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio);
                }
                break;

            case TELA_COMBATE:
                AtualizarControles(&p1);
                AtualizarControles(&p2);

                AtualizarOrientacao(&p1, &p2);

                AtualizarCaixas(&p1);
                AtualizarCaixas(&p2);

                ProcessarColisoes(&p1, &p2);

                AtualizarVisualPersonagem(&p1, deltaTime);
                AtualizarVisualPersonagem(&p2, deltaTime);

                if (IsKeyPressed(KEY_F1))
                {
                    mostrarCaixas = !mostrarCaixas;
                }

                // Reiniciar luta atual se apertar 'R'
                if (IsKeyPressed(KEY_R))
                {
                    InicializarPersonagens(&p1, &p2);
                    AplicarVisuaisSelecionados(&estado, &p1, &p2, &visualDio);
                }

                // Retorna ao menu principal se alguém vencer e apertar ENTER
                if ((p1.estado == ESTADO_KO || p2.estado == ESTADO_KO) && IsKeyPressed(KEY_ENTER))
                {
                    estado.telaAtual = TELA_MENU;
                }
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

                // Cenário / Chão
                DrawLine(0, CHAO_Y, SCREEN_WIDTH, CHAO_Y, DARKGRAY);

                // Renderização dos lutadores
                DesenharPersonagem(&p1);
                DesenharPersonagem(&p2);

                if (mostrarCaixas)
                {
                    DesenharPersonagemDebug(&p1);
                    DesenharPersonagemDebug(&p2);
                }

                // HUD simples: Barras de Vida
                DrawRectangle(50, 40, 300, 25, LIGHTGRAY);
                DrawRectangle(50, 40, 3 * p1.hp, 25, GREEN);
                DrawRectangleLines(50, 40, 300, 25, BLACK);
                DrawText(TextFormat("P1 HP: %d", p1.hp), 50, 70, 20, BLUE);

                DrawRectangle(SCREEN_WIDTH - 350, 40, 300, 25, LIGHTGRAY);
                DrawRectangle(SCREEN_WIDTH - 50 - (3 * p2.hp), 40, 3 * p2.hp, 25, GREEN);
                DrawRectangleLines(SCREEN_WIDTH - 350, 40, 300, 25, BLACK);
                DrawText(TextFormat("P2 HP: %d", p2.hp), SCREEN_WIDTH - 150, 70, 20, RED);

                // Instruções na tela
                DrawText("P1: A/D (mover), C (soco), V (chute)", 50, SCREEN_HEIGHT - 40, 16, DARKGRAY);
                DrawText("P2: Setas (mover), N (soco), M (chute) | R: Reset | F1: caixas", SCREEN_WIDTH - 520, SCREEN_HEIGHT - 40, 16, DARKGRAY);

                if (p1.estado == ESTADO_KO)
                    DrawText("P2 VENCEU! ENTER: Menu | R: Revanche", SCREEN_WIDTH / 2 - 220, 200, 24, RED);
                if (p2.estado == ESTADO_KO)
                    DrawText("P1 VENCEU! ENTER: Menu | R: Revanche", SCREEN_WIDTH / 2 - 220, 200, 24, BLUE);
                break;

            default:
                break;
        }

        EndDrawing();
    }

    DescarregarVisualPersonagem(&visualDio);

    CloseWindow();
    return 0;
}