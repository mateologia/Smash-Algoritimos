#include "combate.h"

void AtualizarOrientacao(Personagem *p1, Personagem *p2) {
    if (p1->posicao.x < p2->posicao.x) {
        p1->direcao = OLHANDO_DIREITA;
        p2->direcao = OLHANDO_ESQUERDA;
    } else {
        p1->direcao = OLHANDO_ESQUERDA;
        p2->direcao = OLHANDO_DIREITA;
    }
}

void ProcessarColisoes(Personagem *p1, Personagem *p2) {
    // 1. Bloqueio de sobreposição (Pushbox/Hurtbox)
    if (CheckCollisionRecs(p1->hurtbox, p2->hurtbox)) {
        if (p1->posicao.x < p2->posicao.x) {
            p1->posicao.x -= 2.0f;
            p2->posicao.x += 2.0f;
        } else {
            p1->posicao.x += 2.0f;
            p2->posicao.x -= 2.0f;
        }
    }

    // 2. Deteção de acerto: Hitbox de P1 contra Hurtbox de P2
    if (p1->hitboxAtiva && CheckCollisionRecs(p1->hitbox, p2->hurtbox)) {
        p2->hp -= (p1->estado == ESTADO_ATACANDO_SOCO) ? DANO_SOCO : DANO_CHUTE;
        p2->estado = ESTADO_HIT;
        p2->temporizadorEstado = 15; // Duração do atordoamento em frames
        p1->hitboxAtiva = false;     // Evita aplicar dano repetido no mesmo golpe
        if (p2->hp <= 0) {
            p2->hp = 0;
            p2->estado = ESTADO_KO;
        }
    }

    // 3. Deteção de acerto: Hitbox de P2 contra Hurtbox de P1
    if (p2->hitboxAtiva && CheckCollisionRecs(p2->hitbox, p1->hurtbox)) {
        p1->hp -= (p2->estado == ESTADO_ATACANDO_SOCO) ? DANO_SOCO : DANO_CHUTE;
        p1->estado = ESTADO_HIT;
        p1->temporizadorEstado = 15;
        p2->hitboxAtiva = false;
        if (p1->hp <= 0) {
            p1->hp = 0;
            p1->estado = ESTADO_KO;
        }
    }
}
