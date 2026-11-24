#include "jogo.h"
#include <math.h>
#include <stdio.h>

static const char *NOMES_LUTADORES[NUM_PERSONAGENS] = {
    "DIO",
    "BAIKEN"
};

static const Color CORES_LUTADORES[NUM_PERSONAGENS] = {
    { 0, 121, 241, 255 }, // BLUE
    { 230, 41, 55, 255 }  // RED
};

static void CarregarHistoricoPartidas(EstadoJogo *estado) {
    estado->historicoVitoriasP1 = 0;
    estado->historicoVitoriasP2 = 0;
    estado->historicoEmpates = 0;

    FILE *arquivo = fopen(ARQUIVO_HISTORICO, "r");
    if (arquivo != NULL) {
        if (fscanf(arquivo, "P1=%d\nP2=%d\nEMPATES=%d",
                   &estado->historicoVitoriasP1,
                   &estado->historicoVitoriasP2,
                   &estado->historicoEmpates) != 3) {
            estado->historicoVitoriasP1 = 0;
            estado->historicoVitoriasP2 = 0;
            estado->historicoEmpates = 0;
        }
        fclose(arquivo);
    }
}

static void RegistrarResultadoHistorico(EstadoJogo *estado) {
    if (estado->vencedorPartida == 1) {
        estado->historicoVitoriasP1++;
    } else if (estado->vencedorPartida == 2) {
        estado->historicoVitoriasP2++;
    } else {
        estado->historicoEmpates++;
    }

    FILE *arquivo = fopen(ARQUIVO_HISTORICO, "w");
    if (arquivo != NULL) {
        fprintf(arquivo, "P1=%d\nP2=%d\nEMPATES=%d\n",
                estado->historicoVitoriasP1,
                estado->historicoVitoriasP2,
                estado->historicoEmpates);
        fclose(arquivo);
    }
}

void InicializarEstadoJogo(EstadoJogo *estado) {
    CarregarHistoricoPartidas(estado);

    if (!estado->retratosCarregados) {
        estado->retratosLutadores[0] = LoadTexture("Smash/animations/DioBrando/icon.png");
        estado->retratosLutadores[1] = LoadTexture("Smash/animations/Baiken/icon.png");
        estado->retratosCarregados = true;
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
    estado->emFinishHim = false;
    estado->timerFinishHim = 0.0f;
    estado->fatalityExecutado = false;
    estado->timerEfeitoFatality = 0.0f;
}

void AtualizarMenu(EstadoJogo *estado, GerenciadorAudio *audio) {
    if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) ||
        IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN)) {
        estado->opcaoMenu = (estado->opcaoMenu == 0) ? 1 : 0;
        TocarEfeito(audio, SOM_MENU_MOVER);
    }

    if (IsKeyPressed(KEY_ENTER)) {
        TocarEfeito(audio, SOM_MENU_CONFIRMAR);
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

void IniciarNovoRound(EstadoJogo *estado, Personagem *p1, Personagem *p2, GerenciadorAudio *audio) {
    InicializarPersonagens(p1, p2);
    p1->id = estado->selecaoP1;
    p1->cor = CORES_LUTADORES[estado->selecaoP1];

    p2->id = estado->selecaoP2;
    p2->cor = CORES_LUTADORES[estado->selecaoP2];

    estado->tempoRound = TEMPO_MAXIMO_ROUND;
    estado->pausaRound = 2.0f; // 2 segundos anunciando "ROUND X" -> "FIGHT!"
    estado->encerrandoRound = false;
    estado->emFinishHim = false;
    estado->timerFinishHim = 0.0f;
    estado->fatalityExecutado = false;
    estado->timerEfeitoFatality = 0.0f;
    TocarEfeito(audio, SOM_ROUND_INICIO);
}

void AtualizarSelecao(EstadoJogo *estado, Personagem *p1, Personagem *p2, GerenciadorAudio *audio) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        TocarEfeito(audio, SOM_MENU_MOVER);
        estado->telaAtual = TELA_MENU;
        return;
    }

    bool enterPressionado = IsKeyPressed(KEY_ENTER);

    if (!estado->prontoP1) {
        if (IsKeyPressed(KEY_A)) {
            estado->selecaoP1 = (estado->selecaoP1 - 1 + NUM_PERSONAGENS) % NUM_PERSONAGENS;
            TocarEfeito(audio, SOM_MENU_MOVER);
        }
        if (IsKeyPressed(KEY_D)) {
            estado->selecaoP1 = (estado->selecaoP1 + 1) % NUM_PERSONAGENS;
            TocarEfeito(audio, SOM_MENU_MOVER);
        }
        if (IsKeyPressed(KEY_C) || enterPressionado) {
            estado->prontoP1 = true;
            enterPressionado = false; // Consome o ENTER deste frame para não confirmar o P2 junto
            TocarEfeito(audio, SOM_MENU_CONFIRMAR);
        }
    }

    if (!estado->prontoP2) {
        if (IsKeyPressed(KEY_LEFT)) {
            estado->selecaoP2 = (estado->selecaoP2 - 1 + NUM_PERSONAGENS) % NUM_PERSONAGENS;
            TocarEfeito(audio, SOM_MENU_MOVER);
        }
        if (IsKeyPressed(KEY_RIGHT)) {
            estado->selecaoP2 = (estado->selecaoP2 + 1) % NUM_PERSONAGENS;
            TocarEfeito(audio, SOM_MENU_MOVER);
        }
        if (IsKeyPressed(KEY_N) || enterPressionado) {
            estado->prontoP2 = true;
            TocarEfeito(audio, SOM_MENU_CONFIRMAR);
        }
    }

    if (estado->prontoP1 && estado->prontoP2) {
        estado->roundsP1 = 0;
        estado->roundsP2 = 0;
        estado->roundAtual = 1;
        estado->vencedorPartida = 0;
        IniciarNovoRound(estado, p1, p2, audio);
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
        if (estado->retratosCarregados && estado->retratosLutadores[i].id > 0) {
            Rectangle origemRetrato = {
                0.0f,
                0.0f,
                (float)estado->retratosLutadores[i].width,
                (float)estado->retratosLutadores[i].height
            };
            DrawTexturePro(estado->retratosLutadores[i], origemRetrato, destinoRetrato, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
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

    const char *textoP1 = TextFormat("P1: %s %s", NOMES_LUTADORES[estado->selecaoP1], estado->prontoP1 ? "[PRONTO]" : "(A/D para escolher | C ou ENTER)");
    DrawText(textoP1, (SCREEN_WIDTH - MeasureText(textoP1, 18)) / 2, 435, 18, estado->prontoP1 ? GREEN : LIGHTGRAY);

    const char *textoP2 = TextFormat("P2: %s %s", NOMES_LUTADORES[estado->selecaoP2], estado->prontoP2 ? "[PRONTO]" : "(Setas para escolher | N ou ENTER)");
    DrawText(textoP2, (SCREEN_WIDTH - MeasureText(textoP2, 18)) / 2, 465, 18, estado->prontoP2 ? ORANGE : LIGHTGRAY);

    DrawText("ESC: Voltar ao Menu", 40, SCREEN_HEIGHT - 40, 18, GRAY);
}

bool AtualizarCombateRounds(EstadoJogo *estado, Personagem *p1, Personagem *p2, float dt, GerenciadorAudio *audio) {
    // 1. Cena de Fatality executado (BONUS-05)
    if (estado->fatalityExecutado) {
        estado->timerEfeitoFatality -= dt;
        if (estado->timerEfeitoFatality <= 0.0f) {
            estado->fatalityExecutado = false;
            estado->vencedorPartida = (p1->estado == ESTADO_FATALITY) ? 1 : 2;
            if (estado->vencedorPartida == 1) estado->roundsP1++;
            else estado->roundsP2++;

            RegistrarResultadoHistorico(estado);
            estado->telaAtual = TELA_VITORIA;
        }
        return false;
    }

    // 2. Estado "FINISH HIM!" no round decisivo (BONUS-05)
    if (estado->emFinishHim) {
        estado->timerFinishHim -= dt;

        Personagem *vencedor = (p2->estado == ESTADO_TONTO) ? p1 : p2;
        Personagem *perdedor = (p2->estado == ESTADO_TONTO) ? p2 : p1;

        bool ehJogador1 = (vencedor->controles.esquerda == KEY_A);
        bool acionarChuteAlto = IsKeyPressed(vencedor->controles.chuteAlto) || (!ehJogador1 && IsKeyPressed(KEY_KP_8));
        float distancia = fabsf(vencedor->posicao.x - perdedor->posicao.x);

        // Executa Fatality se completar Baixo -> Baixo + Chute Alto perto do oponente
        if (vencedor->etapaFatality == 2 && acionarChuteAlto && distancia <= ALCANCE_FATALITY) {
            vencedor->estado = ESTADO_FATALITY;
            vencedor->temporizador = 45;
            perdedor->estado = ESTADO_KO;
            perdedor->hp = 0;
            estado->emFinishHim = false;
            estado->fatalityExecutado = true;
            estado->timerEfeitoFatality = 2.6f;
            estado->posicaoFatality = perdedor->posicao;
            TocarEfeito(audio, SOM_FATALITY);
            return false;
        }

        // Se o tempo do Finish Him acabar ou o perdedor já tiver caído em KO
        if (estado->timerFinishHim <= 0.0f || perdedor->estado == ESTADO_KO) {
            perdedor->hp = 0;
            perdedor->estado = ESTADO_KO;
            estado->emFinishHim = false;
            if (vencedor == p1) estado->roundsP1++;
            else estado->roundsP2++;

            estado->encerrandoRound = true;
            estado->pausaRound = 2.0f;
            TocarEfeito(audio, SOM_KO);
        }
        return false;
    }

    // 3. Pausa de início ou fim de round
    if (estado->pausaRound > 0.0f) {
        estado->pausaRound -= dt;
        if (estado->pausaRound <= 0.0f) {
            estado->pausaRound = 0.0f;
            if (estado->encerrandoRound) {
                if (estado->roundsP1 >= ROUNDS_PARA_VENCER || estado->roundsP2 >= ROUNDS_PARA_VENCER) {
                    if (estado->roundsP1 > estado->roundsP2) estado->vencedorPartida = 1;
                    else if (estado->roundsP2 > estado->roundsP1) estado->vencedorPartida = 2;
                    else estado->vencedorPartida = 0;

                    RegistrarResultadoHistorico(estado);
                    estado->telaAtual = TELA_VITORIA;
                } else {
                    estado->roundAtual++;
                    IniciarNovoRound(estado, p1, p2, audio);
                    return true; // Sinaliza ao main.c para revincular os sprites
                }
            }
        }
        return false;
    }

    // 4. Decrementa cronômetro de 67s
    estado->tempoRound -= dt;
    if (estado->tempoRound < 0.0f) {
        estado->tempoRound = 0.0f;
    }

    bool nocaute = (p1->estado == ESTADO_KO || p2->estado == ESTADO_KO || p1->hp <= 0 || p2->hp <= 0);
    bool tempoEsgotado = (estado->tempoRound <= 0.0f);

    if (nocaute || tempoEsgotado) {
        // Intercepta nocaute no round decisivo para ativar "FINISH HIM!" (BONUS-05)
        bool p1VenceDecisivo = (p1->hp > p2->hp) && (estado->roundsP1 == ROUNDS_PARA_VENCER - 1) && nocaute;
        bool p2VenceDecisivo = (p2->hp > p1->hp) && (estado->roundsP2 == ROUNDS_PARA_VENCER - 1) && nocaute;

        if (p1VenceDecisivo) {
            p2->hp = 1;
            p2->estado = ESTADO_TONTO;
            estado->emFinishHim = true;
            estado->timerFinishHim = TEMPO_FINISH_HIM;
            TocarEfeito(audio, SOM_FINISH_HIM);
            return false;
        }
        if (p2VenceDecisivo) {
            p1->hp = 1;
            p1->estado = ESTADO_TONTO;
            estado->emFinishHim = true;
            estado->timerFinishHim = TEMPO_FINISH_HIM;
            TocarEfeito(audio, SOM_FINISH_HIM);
            return false;
        }

        if (p1->hp > p2->hp) {
            estado->roundsP1++;
        } else if (p2->hp > p1->hp) {
            estado->roundsP2++;
        } else {
            // Empate no round: ambos marcam ponto
            estado->roundsP1++;
            estado->roundsP2++;
        }

        estado->encerrandoRound = true;
        estado->pausaRound = 2.0f; // 2 segundos mostrando o resultado do round antes de trocar
        TocarEfeito(audio, SOM_KO);
    }

    return false;
}

void DesenharHUDRounds(const EstadoJogo *estado) {
    // 1. Relógio central (67 -> 00) no topo entre as duas barras de vida
    int segundos = (int)ceilf(estado->tempoRound);
    const char *textoTempo = TextFormat("%02d", segundos);

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

    // 3. Banner "FINISH HIM!" durante o round decisivo (BONUS-05)
    if (estado->emFinishHim) {
        const char *bannerFinish = "FINISH HIM!";
        int larguraFinish = MeasureText(bannerFinish, 54);
        int x = (SCREEN_WIDTH - larguraFinish) / 2;
        int y = SCREEN_HEIGHT / 2 - 75;
        DrawText(bannerFinish, x + 4, y + 4, 54, BLACK);
        DrawText(bannerFinish, x, y, 54, RED);

        const char *dica = "Fatality (perto): Baixo -> Baixo + Chute Alto (G / K)";
        int larguraDica = MeasureText(dica, 18);
        DrawText(dica, (SCREEN_WIDTH - larguraDica) / 2, y + 62, 18, GOLD);
    }

    // 4. Banner central nas transições ("ROUND X", "FIGHT!", "K.O.!")
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

void AtualizarVitoria(EstadoJogo *estado, GerenciadorAudio *audio) {
    if (IsKeyPressed(KEY_ENTER)) {
        TocarEfeito(audio, SOM_MENU_CONFIRMAR);
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
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2 + 3, 150 + 3, 42, BLACK);
    DrawText(titulo, (SCREEN_WIDTH - larguraTitulo) / 2, 150, 42, corTitulo);

    const char *placar = TextFormat("Placar da Partida: %d x %d", estado->roundsP1, estado->roundsP2);
    int larguraPlacar = MeasureText(placar, 28);
    DrawText(placar, (SCREEN_WIDTH - larguraPlacar) / 2, 225, 28, WHITE);

    // Placar Histórico Persistido (.txt) - BONUS-09
    DrawRectangle(SCREEN_WIDTH / 2 - 230, 285, 460, 75, (Color){ 30, 36, 58, 255 });
    DrawRectangleLines(SCREEN_WIDTH / 2 - 230, 285, 460, 75, GOLD);

    const char *tituloHist = "HISTORICO TOTAL DE PARTIDAS";
    DrawText(tituloHist, (SCREEN_WIDTH - MeasureText(tituloHist, 16)) / 2, 297, 16, GOLD);

    const char *textoHist = TextFormat("P1: %d Vitorias   |   P2: %d Vitorias   |   Empates: %d",
                                       estado->historicoVitoriasP1,
                                       estado->historicoVitoriasP2,
                                       estado->historicoEmpates);
    DrawText(textoHist, (SCREEN_WIDTH - MeasureText(textoHist, 18)) / 2, 326, 18, WHITE);

    const char *instrucao = "Pressione ENTER para voltar ao Menu Principal";
    int larguraInst = MeasureText(instrucao, 20);
    DrawText(instrucao, (SCREEN_WIDTH - larguraInst) / 2, 420, 20, LIGHTGRAY);
}

void DescarregarRecursosJogo(EstadoJogo *estado) {
    if (estado->retratosCarregados) {
        for (int i = 0; i < NUM_PERSONAGENS; i++) {
            if (estado->retratosLutadores[i].id > 0) {
                UnloadTexture(estado->retratosLutadores[i]);
            }
        }
        estado->retratosCarregados = false;
    }
}

const char *ObterNomeLutador(int id) {
    if (id >= 0 && id < NUM_PERSONAGENS) {
        return NOMES_LUTADORES[id];
    }
    return "LUTADOR";
}