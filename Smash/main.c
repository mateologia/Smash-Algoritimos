#include "raylib.h"
#include "definicoes.h"
#include "combate.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jogo de Luta 2D - Teste de Comandos");
    SetTargetFPS(TARGET_FPS);

    Personagem p1, p2;
    InicializarPersonagens(&p1, &p2);

    while (!WindowShouldClose()) {
        // --- ATUALIZA��O ---
        AtualizarControles(&p1);
        AtualizarControles(&p2);

        AtualizarOrientacao(&p1, &p2);

        AtualizarCaixas(&p1);
        AtualizarCaixas(&p2);

        ProcessarColisoes(&p1, &p2);

        // Reiniciar se apertar 'R'
        if (IsKeyPressed(KEY_R)) {
            InicializarPersonagens(&p1, &p2);
        }

        // --- DESENHO ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Cen�rio / Ch�o
        DrawLine(0, CHAO_Y, SCREEN_WIDTH, CHAO_Y, DARKGRAY);

        // Renderiza��o dos lutadores em modo debug
        DesenharPersonagemDebug(&p1);
        DesenharPersonagemDebug(&p2);

        // HUD simples: Barras de Vida
        DrawRectangle(50, 40, 300, 25, LIGHTGRAY);
        DrawRectangle(50, 40, 3 * p1.hp, 25, GREEN);
        DrawRectangleLines(50, 40, 300, 25, BLACK);
        DrawText(TextFormat("P1 HP: %d", p1.hp), 50, 70, 20, BLUE);

        DrawRectangle(SCREEN_WIDTH - 350, 40, 300, 25, LIGHTGRAY);
        DrawRectangle(SCREEN_WIDTH - 50 - (3 * p2.hp), 40, 3 * p2.hp, 25, GREEN);
        DrawRectangleLines(SCREEN_WIDTH - 350, 40, 300, 25, BLACK);
        DrawText(TextFormat("P2 HP: %d", p2.hp), SCREEN_WIDTH - 150, 70, 20, RED);

        // Instru��es na tela
        DrawText("P1: A/D (mover), C (soco), V (chute)", 50, SCREEN_HEIGHT - 40, 16, DARKGRAY);
        DrawText("P2: Setas (mover), N (soco), M (chute) | R: Reset", SCREEN_WIDTH - 460, SCREEN_HEIGHT - 40, 16, DARKGRAY);

        if (p1.estado == ESTADO_KO) DrawText("P2 VENCEU!", SCREEN_WIDTH / 2 - 80, 200, 30, RED);
        if (p2.estado == ESTADO_KO) DrawText("P1 VENCEU!", SCREEN_WIDTH / 2 - 80, 200, 30, BLUE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
