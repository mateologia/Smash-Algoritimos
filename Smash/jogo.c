#include "jogo.h"

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
    estado->sairJogo = false;
}

void AtualizarMenu(EstadoJogo *estado) {
    // Alternar entre NOVO JOGO e SAIR
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) ||
        IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        estado->opcaoMenu = (estado->opcaoMenu == 0) ? 1 : 0;
    }

    // Confirmar seleção com ENTER
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
    ClearBackground((Color){ 18, 22, 36, 255 }); // Fundo escuro estilo arcade

    // Faixa decorativa estilo Mortal Kombat
    DrawRectangle(0, 90, SCREEN_WIDTH, 130, (Color){ 180, 30, 30, 255 });
    DrawRectangle(0, 85, SCREEN_WIDTH, 5, GOLD);
    DrawRectangle(0, 220, SCREEN_WIDTH, 5, GOLD);

    // Título Principal
    const char *titulo = "MORTAL KOMBAT C";
    int larguraTitulo = MeasureText(titulo, 54);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2 + 4, 128 + 4, 54, BLACK);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2, 128, 54, GOLD);

    // Botão 1: NOVO JOGO
    Rectangle btnNovo = { SCREEN_WIDTH / 2.0f - 150, 320, 300, 55 };
    Color corNovo = (estado->opcaoMenu == 0) ? GOLD : DARKGRAY;
    DrawRectangleRec(btnNovo, (Color){ 35, 40, 60, 255 });
    DrawRectangleLinesEx(btnNovo, (estado->opcaoMenu == 0) ? 4 : 2, corNovo);
    DrawText("NOVO JOGO", btnNovo.x + 65, btnNovo.y + 15, 28, (estado->opcaoMenu == 0) ? YELLOW : WHITE);

    // Botão 2: SAIR
    Rectangle btnSair = { SCREEN_WIDTH / 2.0f - 150, 400, 300, 55 };
    Color corSair = (estado->opcaoMenu == 1) ? GOLD : DARKGRAY;
    DrawRectangleRec(btnSair, (Color){ 35, 40, 60, 255 });
    DrawRectangleLinesEx(btnSair, (estado->opcaoMenu == 1) ? 4 : 2, corSair);
    DrawText("SAIR", btnSair.x + 115, btnSair.y + 15, 28, (estado->opcaoMenu == 1) ? YELLOW : WHITE);

    // Rodapé de instrução
    const char *instrucao = "Use W/S ou SETAS para navegar e ENTER para confirmar";
    int larguraInst = MeasureText(instrucao, 18);
    DrawText(instrucao, (SCREEN_WIDTH - larguraInst) / 2, SCREEN_HEIGHT - 50, 18, LIGHTGRAY);
}

void AtualizarSelecao(EstadoJogo *estado, Personagem *p1, Personagem *p2) {
    // Voltar ao menu principal com ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        estado->telaAtual = TELA_MENU;
        return;
    }

    // Navegação Jogador 1 (A / D)
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

    // Navegação Jogador 2 (Setas Esquerda / Direita)
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

    // Quando ambos confirmarem, inicia a luta
    if (estado->prontoP1 && estado->prontoP2) {
        InicializarPersonagens(p1, p2);
        p1->id = estado->selecaoP1;
        p1->cor = CORES_LUTADORES[estado->selecaoP1];

        p2->id = estado->selecaoP2;
        p2->cor = CORES_LUTADORES[estado->selecaoP2];

        estado->roundsP1 = 0;
        estado->roundsP2 = 0;
        estado->telaAtual = TELA_COMBATE;
    }
}

void DesenharSelecao(const EstadoJogo *estado) {
    ClearBackground((Color){ 24, 28, 44, 255 });

    const char *titulo = "ESCOLHA SEU LUTADOR!";
    int larguraTitulo = MeasureText(titulo, 36);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2, 45, 36, GOLD);

    // Desenha os quadros (retratos) dos personagens disponíveis no centro
    int larguraCard = 180;
    int alturaCard = 220;
    int espacamento = 60;
    int totalLargura = (NUM_PERSONAGENS * larguraCard) + ((NUM_PERSONAGENS - 1) * espacamento);
    int inicioX = (SCREEN_WIDTH - totalLargura) / 2;
    int posY = 160;

    for (int i = 0; i < NUM_PERSONAGENS; i++) {
        Rectangle card = { inicioX + i * (larguraCard + espacamento), posY, larguraCard, alturaCard };
        DrawRectangleRec(card, (Color){ 45, 52, 75, 255 });

        // Área interna do retrato (120x120 centralizado horizontalmente no card)
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

        // Nome do lutador centralizado dentro do card
        int larguraNome = MeasureText(NOMES_LUTADORES[i], 20);
        DrawText(NOMES_LUTADORES[i], card.x + (larguraCard - larguraNome) / 2, card.y + 170, 20, WHITE);

        // Borda de seleção P1 (Verde)
        if (estado->selecaoP1 == i) {
            Rectangle bordaP1 = { card.x - 6, card.y - 6, card.width + 12, card.height + 12 };
            DrawRectangleLinesEx(bordaP1, 4, GREEN);
            DrawText("1P", bordaP1.x, bordaP1.y - 25, 22, GREEN);
        }

        // Borda de seleção P2 (Laranja)
        if (estado->selecaoP2 == i) {
            Rectangle bordaP2 = { card.x - 12, card.y - 12, card.width + 24, card.height + 24 };
            DrawRectangleLinesEx(bordaP2, 4, ORANGE);
            DrawText("2P", bordaP2.x + bordaP2.width - 30, bordaP2.y - 25, 22, ORANGE);
        }
    }

    // Status de confirmação abaixo dos retratos
    const char *textoP1 = TextFormat("P1: %s %s", NOMES_LUTADORES[estado->selecaoP1], estado->prontoP1 ? "[PRONTO]" : "(A/D + J ou ENTER)");
    DrawText(textoP1, (SCREEN_WIDTH - MeasureText(textoP1, 18)) / 2, 435, 18, estado->prontoP1 ? GREEN : LIGHTGRAY);

    const char *textoP2 = TextFormat("P2: %s %s", NOMES_LUTADORES[estado->selecaoP2], estado->prontoP2 ? "[PRONTO]" : "(Setas + KP_4 ou ENTER)");
    DrawText(textoP2, (SCREEN_WIDTH - MeasureText(textoP2, 18)) / 2, 465, 18, estado->prontoP2 ? ORANGE : LIGHTGRAY);

    DrawText("ESC: Voltar ao Menu", 40, SCREEN_HEIGHT - 40, 18, GRAY);
}