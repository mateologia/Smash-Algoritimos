#include "raylib.h"
#include "definicoes.h"
#include "combate.h"
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

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jogo de Luta 2D - Teste de Comandos");
    SetTargetFPS(TARGET_FPS);

    Personagem p1 = {0}, p2 = {0};
    VisualPersonagem visualDio = {0};

    InicializarPersonagens(&p1, &p2);

    if (CarregarVisualPersonagem(&visualDio, &VISUAL_DIO))
    {
        VincularVisualPersonagem(&p1, &visualDio);
    }
    else
    {
        TraceLog(LOG_WARNING, "P1 vai usar o retangulo de debug (sprite nao carregado)");
    }

    bool mostrarCaixas = true;

    while (!WindowShouldClose())
    {
        // --- ATUALIZAÇÃO ---
        AtualizarControles(&p1);
        AtualizarControles(&p2);

        AtualizarOrientacao(&p1, &p2);

        AtualizarCaixas(&p1);
        AtualizarCaixas(&p2);

        ProcessarColisoes(&p1, &p2);

        float deltaTime = GetFrameTime();
        AtualizarVisualPersonagem(&p1, deltaTime);
        AtualizarVisualPersonagem(&p2, deltaTime);

        if (IsKeyPressed(KEY_F1))
        {
            mostrarCaixas = !mostrarCaixas;
        }

        // Reiniciar se apertar 'R'
        if (IsKeyPressed(KEY_R))
        {
            InicializarPersonagens(&p1, &p2);
            if (visualDio.carregado)
            {
                VincularVisualPersonagem(&p1, &visualDio);
            }
        }

        // --- DESENHO ---
        BeginDrawing();
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
            DrawText("P2 VENCEU!", SCREEN_WIDTH / 2 - 80, 200, 30, RED);
        if (p2.estado == ESTADO_KO)
            DrawText("P1 VENCEU!", SCREEN_WIDTH / 2 - 80, 200, 30, BLUE);

        EndDrawing();
    }

    DescarregarVisualPersonagem(&visualDio);

    CloseWindow();
    return 0;
}
