#include "jogo.h"
#include <math.h>

static const char *NOMES_LUTADORES[NUM_PERSONAGENS] = {
    "DIO",
    "BAIKEN"
};

static const Color CORES_LUTADORES[NUM_PERSONAGENS] = {
    BLUE,
    RED
};

static Texture2D retratosLutadores[NUM_PERSONAGENS];
static bool retratosCarregados = false;

void InicializarEstadoJogo(EstadoJogo *estado) {
    if (!retratosCarregados) {
        retratosLutadores[0] = LoadTexture("Smash/animations/DioBrando/icon.png");
        retratosLutadores[1] = LoadTexture("Smash/animations/Baiken/icon.png");
        retratosCarregados = true;
    }

    estado->telaAtual = TELA_MENU;
    estado->opcaoMenu = 0; // 0 = NOVO JOGO, 1 = SAIR
    estado->selecaoP1 = 0;
    estado->selecaoP2 = 1;
    estado->prontoP1 = false;
    estado->prontoP2 = false;
    estado->roundsP1 = 0;
    estado->roundsP2 = 0;
    estado->roundAtual = 1;
    estado->tempoRound = TEMPO_MAXIMO_ROUND;
    estado->pausaRound = 0.0f;
    estado->encerrandoRound = false;
    estado->vencedorPartida = 0;
    estado->sairJogo = false;
}

void AtualizarMenu(EstadoJogo *estado) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) ||
        IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        estado->opcaoMenu = (estado->opcaoMenu == 0) ? 1 : 0;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (estado->opcaoMenu == 0) {
            estado->selecaoP1 = 0;
            estado->selecaoP2 = 1;
            estado->prontoP1 = false;
            estado->prontoP2 = false;
            estado->telaAtual = TELA_SELECAO;
        } else {
            estado->sairJogo = true;
        }
    }
}

void DesenharMenu(const EstadoJogo *estado) {
    ClearBackground((Color){ 18, 22, 36, 255 });

    DrawRectangle(0, 90, SCREEN_WIDTH, 130, (Color){ 180, 30, 30, 255 });
    DrawRectangle(0, 85, SCREEN_WIDTH, 5, GOLD);
    DrawRectangle(0, 220, SCREEN_WIDTH, 5, GOLD);

    const char *titulo = "MORTAL KOMBAT C";
    int larguraTitulo = MeasureText(titulo, 54);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2 + 4, 128 + 4, 54, BLACK);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2, 128, 54, GOLD);

    Rectangle btnNovo = { SCREEN_WIDTH / 2.0f - 150, 320, 300, 55 };
    Color corNovo = (estado->opcaoMenu == 0) ? GOLD : DARKGRAY;
    DrawRectangleRec(btnNovo, (Color){ 35, 40, 60, 255 });
    DrawRectangleLinesEx(btnNovo, (estado->opcaoMenu == 0) ? 4 : 2, corNovo);
    DrawText("NOVO JOGO", btnNovo.x + 65, btnNovo.y + 15, 28, (estado->opcaoMenu == 0) ? YELLOW : WHITE);

    Rectangle btnSair = { SCREEN_WIDTH / 2.0f - 150, 400, 300, 55 };
    Color corSair = (estado->opcaoMenu == 1) ? GOLD : DARKGRAY;
    DrawRectangleRec(btnSair, (Color){ 35, 40, 60, 255 });
    DrawRectangleLinesEx(btnSair, (estado->opcaoMenu == 1) ? 4 : 2, corSair);
    DrawText("SAIR", btnSair.x + 115, btnSair.y + 15, 28, (estado->opcaoMenu == 1) ? YELLOW : WHITE);

    const char *instrucao = "Use W/S ou SETAS para navegar e ENTER para confirmar";
    int larguraInst = MeasureText(instrucao, 18);
    DrawText(instrucao, (SCREEN_WIDTH - larguraInst) / 2, SCREEN_HEIGHT - 50, 18, LIGHTGRAY);
}

void IniciarNovoRound(EstadoJogo *estado, Personagem *p1, Personagem *p2) {
    InicializarPersonagens(p1, p2);
    p1->id = estado->selecaoP1;
    p1->cor = CORES_LUTADORES[estado->selecaoP1];

    p2->id = estado->selecaoP2;
    p2->cor = CORES_LUTADORES[estado->selecaoP2];

    estado->tempoRound = TEMPO_MAXIMO_ROUND;
    estado->pausaRound = 2.0f; // 2 segundos anunciando "ROUND X" -> "FIGHT!"
    estado->encerrandoRound = false;
}

void AtualizarSelecao(EstadoJogo *estado, Personagem *p1, Personagem *p2) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        estado->telaAtual = TELA_MENU;
        return;
    }

    if (!estado->prontoP1) {
        if (IsKeyPressed(KEY_A)) {
            estado->selecaoP1 = (estado->selecaoP1 - 1 + NUM_PERSONAGENS) % NUM_PERSONAGENS;
        }
        if (IsKeyPressed(KEY_D)) {
            estado->selecaoP1 = (estado->selecaoP1 + 1) % NUM_PERSONAGENS;
        }
        if (IsKeyPressed(KEY_J) || IsKeyPressed(KEY_ENTER)) {
            estado->prontoP1 = true;
        }
    }

    if (!estado->prontoP2) {
        if (IsKeyPressed(KEY_LEFT)) {
            estado->selecaoP2 = (estado->selecaoP2 - 1 + NUM_PERSONAGENS) % NUM_PERSONAGENS;
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            estado->selecaoP2 = (estado->selecaoP2 + 1) % NUM_PERSONAGENS;
        }
        if (IsKeyPressed(KEY_KP_4) || IsKeyPressed(KEY_ENTER)) {
            estado->prontoP2 = true;
        }
    }

    if (estado->prontoP1 && estado->prontoP2) {
        estado->roundsP1 = 0;
        estado->roundsP2 = 0;
        estado->roundAtual = 1;
        estado->vencedorPartida = 0;
        IniciarNovoRound(estado, p1, p2);
        estado->telaAtual = TELA_COMBATE;
    }
}

void DesenharSelecao(const EstadoJogo *estado) {
    ClearBackground((Color){ 24, 28, 44, 255 });

    const char *titulo = "ESCOLHA SEU LUTADOR!";
    int larguraTitulo = MeasureText(titulo, 36);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2, 45, 36, GOLD);

    int larguraCard = 180;
    int alturaCard = 220;
    int espacamento = 60;
    int totalLargura = (NUM_PERSONAGENS * larguraCard) + ((NUM_PERSONAGENS - 1) * espacamento);
    int inicioX = (SCREEN_WIDTH - totalLargura) / 2;
    int posY = 160;

    for (int i = 0; i < NUM_PERSONAGENS; i++) {
        Rectangle card = { inicioX + i * (larguraCard + espacamento), posY, larguraCard, alturaCard };
        DrawRectangleRec(card, (Color){ 45, 52, 75, 255 });

        Rectangle destinoRetrato = { card.x + 30, card.y + 25, 120, 120 };
        if (retratosCarregados && retratosLutadores[i].id > 0) {
            Rectangle origemRetrato = {
                0.0f,
                0.0f,
                (float)retratosLutadores[i].width,
                (float)retratosLutadores[i].height
            };
            DrawTexturePro(retratosLutadores[i], origemRetrato, destinoRetrato, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
        } else {
            DrawRectangleRec(destinoRetrato, CORES_LUTADORES[i]);
        }

        int larguraNome = MeasureText(NOMES_LUTADORES[i], 20);
        DrawText(NOMES_LUTADORES[i], card.x + (larguraCard - larguraNome) / 2, card.y + 170, 20, WHITE);

        if (estado->selecaoP1 == i) {
            Rectangle bordaP1 = { card.x - 6, card.y - 6, card.width + 12, card.height + 12 };
            DrawRectangleLinesEx(bordaP1, 4, GREEN);
            DrawText("1P", bordaP1.x, bordaP1.y - 25, 22, GREEN);
        }

        if (estado->selecaoP2 == i) {
            Rectangle bordaP2 = { card.x - 12, card.y - 12, card.width + 24, card.height + 24 };
            DrawRectangleLinesEx(bordaP2, 4, ORANGE);
            DrawText("2P", bordaP2.x + bordaP2.width - 30, bordaP2.y - 25, 22, ORANGE);
        }
    }

    const char *textoP1 = TextFormat("P1: %s %s", NOMES_LUTADORES[estado->selecaoP1], estado->prontoP1 ? "[PRONTO]" : "(A/D + J ou ENTER)");
    DrawText(textoP1, (SCREEN_WIDTH - MeasureText(textoP1, 18)) / 2, 435, 18, estado->prontoP1 ? GREEN : LIGHTGRAY);

    const char *textoP2 = TextFormat("P2: %s %s", NOMES_LUTADORES[estado->selecaoP2], estado->prontoP2 ? "[PRONTO]" : "(Setas + KP_4 ou ENTER)");
    DrawText(textoP2, (SCREEN_WIDTH - MeasureText(textoP2, 18)) / 2, 465, 18, estado->prontoP2 ? ORANGE : LIGHTGRAY);

    DrawText("ESC: Voltar ao Menu", 40, SCREEN_HEIGHT - 40, 18, GRAY);
}

bool AtualizarCombateRounds(EstadoJogo *estado, Personagem *p1, Personagem *p2, float dt) {
    // Pausa de início ou fim de round
    if (estado->pausaRound > 0.0f) {
        estado->pausaRound -= dt;
        if (estado->pausaRound <= 0.0f) {
            estado->pausaRound = 0.0f;
            if (estado->encerrandoRound) {
                if (estado->roundsP1 >= ROUNDS_PARA_VENCER || estado->roundsP2 >= ROUNDS_PARA_VENCER) {
                    if (estado->roundsP1 > estado->roundsP2) estado->vencedorPartida = 1;
                    else if (estado->roundsP2 > estado->roundsP1) estado->vencedorPartida = 2;
                    else estado->vencedorPartida = 0;

                    estado->telaAtual = TELA_VITORIA;
                } else {
                    estado->roundAtual++;
                    IniciarNovoRound(estado, p1, p2);
                    return true; // Sinaliza ao main.c para revincular os sprites
                }
            }
        }
        return false;
    }

    // Decrementa cronômetro de 67s
    estado->tempoRound -= dt;
    if (estado->tempoRound < 0.0f) {
        estado->tempoRound = 0.0f;
    }

    bool nocaute = (p1->estado == ESTADO_KO || p2->estado == ESTADO_KO || p1->hp <= 0 || p2->hp <= 0);
    bool tempoEsgotado = (estado->tempoRound <= 0.0f);

    if (nocaute || tempoEsgotado) {
        if (p1->hp > p2->hp) {
            estado->roundsP1++;
        } else if (p2->hp > p1->hp) {
            estado->roundsP2++;
        } else {
            // Empate no round: ambos marcam ponto (se ambos chegarem a 2, vai para TELA_VITORIA como empate)
            estado->roundsP1++;
            estado->roundsP2++;
        }

        estado->encerrandoRound = true;
        estado->pausaRound = 2.0f; // 2 segundos mostrando o resultado do round antes de trocar
    }

    return false;
}

void DesenharHUDRounds(const EstadoJogo *estado) {
    // 1. Relógio central (67 -> 00) no topo entre as duas barras de vida
    int segundos = (int)ceilf(estado->tempoRound);
    const char *textoTempo = TextFormat("%02d", segundos);
    Color corTempo = (segundos <= 10) ? RED : DARKBROWN;

    Rectangle caixaTempo = { SCREEN_WIDTH / 2.0f - 42, 24, 84, 54 };
    DrawRectangleRec(caixaTempo, (Color){ 25, 25, 35, 255 });
    DrawRectangleLinesEx(caixaTempo, 3, GOLD);

    int larguraTempo = MeasureText(textoTempo, 38);
    DrawText(textoTempo, (SCREEN_WIDTH - larguraTempo) / 2, 33, 38, (segundos <= 10) ? RED : GOLD);

    // 2. Medalhões de Rounds vencidos abaixo das barras de vida
    for (int i = 0; i < ROUNDS_PARA_VENCER; i++) {
        // Bolinhas P1 (canto esquerdo, abaixo do texto "P1 HP")
        int posXP1 = 230 + (i * 28);
        DrawCircle(posXP1, 80, 9, (i < estado->roundsP1) ? GOLD : LIGHTGRAY);
        DrawCircleLines(posXP1, 80, 9, BLACK);

        // Bolinhas P2 (canto direito, abaixo da barra do P2)
        int posXP2 = SCREEN_WIDTH - 230 - (i * 28);
        DrawCircle(posXP2, 80, 9, (i < estado->roundsP2) ? GOLD : LIGHTGRAY);
        DrawCircleLines(posXP2, 80, 9, BLACK);
    }

    // 3. Banner central nas transições ("ROUND X", "FIGHT!", "FIM DE ROUND")
    if (estado->pausaRound > 0.0f) {
        const char *banner;
        if (estado->encerrandoRound) {
            banner = (estado->tempoRound <= 0.0f) ? "TEMPO ESGOTADO!" : "K.O.!";
        } else if (estado->pausaRound > 0.7f) {
            bool roundDecisivo = (estado->roundsP1 == ROUNDS_PARA_VENCER - 1 && estado->roundsP2 == ROUNDS_PARA_VENCER - 1);
            banner = roundDecisivo ? "FINAL ROUND" : TextFormat("ROUND %d", estado->roundAtual);
        } else {
            banner = "FIGHT!";
        }

        int larguraBanner = MeasureText(banner, 56);
        int x = (SCREEN_WIDTH - larguraBanner) / 2;
        int y = SCREEN_HEIGHT / 2 - 40;
        DrawText(banner, x + 4, y + 4, 56, BLACK);
        DrawText(banner, x, y, 56, (estado->encerrandoRound) ? RED : GOLD);
    }
}

void AtualizarVitoria(EstadoJogo *estado) {
    if (IsKeyPressed(KEY_ENTER)) {
        estado->telaAtual = TELA_MENU;
    }
}

void DesenharVitoria(const EstadoJogo *estado) {
    ClearBackground((Color){ 18, 22, 36, 255 });

    const char *titulo;
    Color corTitulo;
    if (estado->vencedorPartida == 1) {
        titulo = TextFormat("P1 (%s) VENCEU A PARTIDA!", NOMES_LUTADORES[estado->selecaoP1]);
        corTitulo = GREEN;
    } else if (estado->vencedorPartida == 2) {
        titulo = TextFormat("P2 (%s) VENCEU A PARTIDA!", NOMES_LUTADORES[estado->selecaoP2]);
        corTitulo = ORANGE;
    } else {
        titulo = "EMPATE!";
        corTitulo = GOLD;
    }

    int larguraTitulo = MeasureText(titulo, 42);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2 + 3, 180 + 3, 42, BLACK);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2, 180, 42, corTitulo);

    const char *placar = TextFormat("Placar Final: %d x %d", estado->roundsP1, estado->roundsP2);
    int larguraPlacar = MeasureText(placar, 28);
    DrawText(placar, (SCREEN_WIDTH - larguraPlacar) / 2, 260, 28, WHITE);

    const char *instrucao = "Pressione ENTER para voltar ao Menu Principal";
    int larguraInst = MeasureText(instrucao, 20);
    DrawText(instrucao, (SCREEN_WIDTH - larguraInst) / 2, 380, 20, LIGHTGRAY);
}