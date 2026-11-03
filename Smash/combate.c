#include "combate.h"

void InicializarPersonagens(Personagem *p1, Personagem *p2) {
    // Configuração Jogador 1
    p1->id = 1;
    p1->posicao = (Vector2){ 250.0f, CHAO_Y };
    p1->hp = HP_MAX;
    p1->estado = ESTADO_IDLE;
    p1->direcao = OLHANDO_DIREITA;
    p1->hitboxAtiva = false;
    p1->temporizador = 0;
    p1->cor = BLUE;
    p1->controlos = (MapeamentoTeclas){
        .esquerda = KEY_A,
        .direita = KEY_D,
        .soco = KEY_J,
        .chute = KEY_K
    };

    // Configuração Jogador 2
    p2->id = 2;
    p2->posicao = (Vector2){ 750.0f, CHAO_Y };
    p2->hp = HP_MAX;
    p2->estado = ESTADO_IDLE;
    p2->direcao = OLHANDO_ESQUERDA;
    p2->hitboxAtiva = false;
    p2->temporizador = 0;
    p2->cor = RED;
    p2->controlos = (MapeamentoTeclas){
        .esquerda = KEY_LEFT,
        .direita = KEY_RIGHT,
        .soco = KEY_KP_4,
        .chute = KEY_KP_5
    };

    AtualizarCaixas(p1);
    AtualizarCaixas(p2);
}

void AtualizarControles(Personagem *p) {
    if (p->estado == ESTADO_KO) return;

    // Se estiver em animação de dano (Hitstun)
    if (p->estado == ESTADO_HIT) {
        p->temporizador--;
        if (p->temporizador <= 0) {
            p->estado = ESTADO_IDLE;
        }
        return;
    }

    // Se estiver executando um ataque
    if (p->estado == ESTADO_ATACANDO_SOCO || p->estado == ESTADO_ATACANDO_CHUTE) {
        p->temporizador--;
        // Ativa a hitbox na janela central do golpe (frames 14 a 6)
        if (p->temporizador <= 14 && p->temporizador >= 6) {
            p->hitboxAtiva = true;
        } else {
            p->hitboxAtiva = false;
        }

        if (p->temporizador <= 0) {
            p->estado = ESTADO_IDLE;
            p->hitboxAtiva = false;
        }
        return;
    }

    // Prioridade para acionar novos ataques
    if (IsKeyPressed(p->controlos.soco)) {
        p->estado = ESTADO_ATACANDO_SOCO;
        p->temporizador = TEMPO_ATAQUE;
        return;
    }
    if (IsKeyPressed(p->controlos.chute)) {
        p->estado = ESTADO_ATACANDO_CHUTE;
        p->temporizador = TEMPO_ATAQUE;
        return;
    }

    // Movimentação horizontal
    bool movendo = false;
    if (IsKeyDown(p->controlos.esquerda)) {
        p->posicao.x -= VELOCIDADE_PADRAO;
        movendo = true;
    }
    if (IsKeyDown(p->controlos.direita)) {
        p->posicao.x += VELOCIDADE_PADRAO;
        movendo = true;
    }

    p->estado = movendo ? ESTADO_ANDANDO : ESTADO_IDLE;

    // Limites da tela
    float margem = LARGURA_CORPO / 2.0f;
    if (p->posicao.x - margem < 0.0f) p->posicao.x = margem;
    if (p->posicao.x + margem > SCREEN_WIDTH) p->posicao.x = SCREEN_WIDTH - margem;
}

void AtualizarOrientacao(Personagem *p1, Personagem *p2) {
    // Apenas atualiza a direção se não estiverem no meio de um ataque
    if (p1->estado != ESTADO_ATACANDO_SOCO && p1->estado != ESTADO_ATACANDO_CHUTE) {
        p1->direcao = (p1->posicao.x < p2->posicao.x) ? OLHANDO_DIREITA : OLHANDO_ESQUERDA;
    }
    if (p2->estado != ESTADO_ATACANDO_SOCO && p2->estado != ESTADO_ATACANDO_CHUTE) {
        p2->direcao = (p2->posicao.x < p1->posicao.x) ? OLHANDO_DIREITA : OLHANDO_ESQUERDA;
    }
}

void AtualizarCaixas(Personagem *p) {
    // Hurtbox: cobre o corpo do lutador
    p->hurtbox = (Rectangle){
        p->posicao.x - (LARGURA_CORPO / 2.0f),
        p->posicao.y - ALTURA_CORPO,
        LARGURA_CORPO,
        ALTURA_CORPO
    };

    // Hitbox: projetada à frente na direção em que o jogador está virado
    float ataqueX = (p->direcao == OLHANDO_DIREITA) 
        ? (p->posicao.x + (LARGURA_CORPO / 2.0f))
        : (p->posicao.x - (LARGURA_CORPO / 2.0f) - LARGURA_HITBOX);

    // Ajusta a altura da hitbox de acordo com o golpe (alto para soco, baixo para chute)
    float ataqueY = (p->estado == ESTADO_ATACANDO_SOCO) 
        ? (p->posicao.y - ALTURA_CORPO + 25.0f) 
        : (p->posicao.y - 55.0f);

    p->hitbox = (Rectangle){
        ataqueX,
        ataqueY,
        LARGURA_HITBOX,
        ALTURA_HITBOX
    };
}

void ProcessarColisoes(Personagem *p1, Personagem *p2) {
    // 1. Bloqueio de sobreposição de corpos
    if (CheckCollisionRecs(p1->hurtbox, p2->hurtbox)) {
        float sobreposicao = (p1->hurtbox.x + p1->hurtbox.width) - p2->hurtbox.x;
        if (p1->posicao.x < p2->posicao.x) {
            p1->posicao.x -= 2.0f;
            p2->posicao.x += 2.0f;
        } else {
            p1->posicao.x += 2.0f;
            p2->posicao.x -= 2.0f;
        }
        AtualizarCaixas(p1);
        AtualizarCaixas(p2);
    }

    // 2. Dano P1 -> P2
    if (p1->hitboxAtiva && CheckCollisionRecs(p1->hitbox, p2->hurtbox)) {
        int dano = (p1->estado == ESTADO_ATACANDO_SOCO) ? DANO_SOCO : DANO_CHUTE;
        p2->hp -= dano;
        p2->estado = ESTADO_HIT;
        p2->temporizador = TEMPO_HIT;
        p1->hitboxAtiva = false; // Desativa para não dar dano contínuo em frames subsequentes

        if (p2->hp <= 0) {
            p2->hp = 0;
            p2->estado = ESTADO_KO;
        }
    }

    // 3. Dano P2 -> P1
    if (p2->hitboxAtiva && CheckCollisionRecs(p2->hitbox, p1->hurtbox)) {
        int dano = (p2->estado == ESTADO_ATACANDO_SOCO) ? DANO_SOCO : DANO_CHUTE;
        p1->hp -= dano;
        p1->estado = ESTADO_HIT;
        p1->temporizador = TEMPO_HIT;
        p2->hitboxAtiva = false;

        if (p1->hp <= 0) {
            p1->hp = 0;
            p1->estado = ESTADO_KO;
        }
    }
}

void DesenharPersonagemDebug(const Personagem *p) {
    // Hurtbox (Corpo)
    Color corCorpo = p->cor;
    if (p->estado == ESTADO_HIT) corCorpo = ORANGE;
    if (p->estado == ESTADO_KO) corCorpo = DARKGRAY;
    DrawRectangleRec(p->hurtbox, corCorpo);
    DrawRectangleLinesEx(p->hurtbox, 2, BLACK);

    // Indicador da direção dos olhos/frente
    float olhoX = (p->direcao == OLHANDO_DIREITA) ? (p->posicao.x + 15.0f) : (p->posicao.x - 25.0f);
    DrawRectangle(olhoX, p->posicao.y - ALTURA_CORPO + 15.0f, 10, 10, WHITE);

    // Hitbox (se estiver ativa durante um golpe)
    if (p->hitboxAtiva) {
        DrawRectangleRec(p->hitbox, Fade(YELLOW, 0.6f));
        DrawRectangleLinesEx(p->hitbox, 2, RED);
    }
}
