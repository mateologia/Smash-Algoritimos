#include "combate.h"
#include <string.h>

static bool EstaAtacando(EstadoPersonagem estado)
{
    return (estado == ESTADO_ATACANDO_SOCO_BAIXO ||
            estado == ESTADO_ATACANDO_SOCO_ALTO ||
            estado == ESTADO_ATACANDO_CHUTE_BAIXO ||
            estado == ESTADO_ATACANDO_CHUTE_ALTO);
}

static int ObterDanoAtaque(EstadoPersonagem estado)
{
    switch (estado)
    {
        case ESTADO_ATACANDO_SOCO_BAIXO:  return DANO_SOCO_BAIXO;
        case ESTADO_ATACANDO_SOCO_ALTO:   return DANO_SOCO_ALTO;
        case ESTADO_ATACANDO_CHUTE_BAIXO: return DANO_CHUTE_BAIXO;
        case ESTADO_ATACANDO_CHUTE_ALTO:  return DANO_CHUTE_ALTO;
        default:                          return 0;
    }
}

void InicializarPersonagens(Personagem *p1, Personagem *p2)
{
    memset(p1, 0, sizeof(*p1));
    memset(p2, 0, sizeof(*p2));

    // Configuração Jogador 1
    p1->id = 1;
    p1->posicao = (Vector2){250.0f, CHAO_Y};
    p1->hp = HP_MAX;
    p1->estado = ESTADO_IDLE;
    p1->direcao = OLHANDO_DIREITA;
    p1->hitboxAtiva = false;
    p1->temporizador = 0;
    p1->cor = BLUE;
    p1->controles = (MapeamentoTeclas){
        .cima = KEY_W,
        .baixo = KEY_S,
        .esquerda = KEY_A,
        .direita = KEY_D,
        .socoBaixo = KEY_C,
        .socoAlto = KEY_F,
        .chuteBaixo = KEY_V,
        .chuteAlto = KEY_G};

    // Configuração Jogador 2
    p2->id = 2;
    p2->posicao = (Vector2){750.0f, CHAO_Y};
    p2->hp = HP_MAX;
    p2->estado = ESTADO_IDLE;
    p2->direcao = OLHANDO_ESQUERDA;
    p2->hitboxAtiva = false;
    p2->temporizador = 0;
    p2->cor = RED;
    p2->controles = (MapeamentoTeclas){
        .cima = KEY_UP,
        .baixo = KEY_DOWN,
        .esquerda = KEY_LEFT,
        .direita = KEY_RIGHT,
        .socoBaixo = KEY_N,
        .socoAlto = KEY_J,
        .chuteBaixo = KEY_M,
        .chuteAlto = KEY_K};

    AtualizarCaixas(p1);
    AtualizarCaixas(p2);
}

void AtualizarControles(Personagem *p)
{
    p->direcaoMovimento = 0;

    if (p->estado == ESTADO_KO)
        return;

    // Se estiver em animação de dano (Hitstun)
    if (p->estado == ESTADO_HIT)
    {
        p->temporizador--;
        if (p->temporizador <= 0)
        {
            p->estado = ESTADO_IDLE;
        }
        return;
    }

    // Se estiver executando qualquer um dos 4 ataques
    if (EstaAtacando(p->estado))
    {
        p->temporizador--;
        // Ativa a hitbox na janela central do golpe (frames 14 a 6)
        if (p->temporizador <= 14 && p->temporizador >= 6)
        {
            p->hitboxAtiva = true;
        }
        else
        {
            p->hitboxAtiva = false;
        }

        if (p->temporizador <= 0)
        {
            p->estado = ESTADO_IDLE;
            p->hitboxAtiva = false;
        }
        return;
    }

    // Prioridade para acionar novos ataques (cada jogador isolado nos seus botões)
    bool ehJogador1 = (p->controles.esquerda == KEY_A);
    bool acionarSocoBaixo = IsKeyPressed(p->controles.socoBaixo) || (!ehJogador1 && IsKeyPressed(KEY_KP_4));
    bool acionarSocoAlto = IsKeyPressed(p->controles.socoAlto) || (!ehJogador1 && IsKeyPressed(KEY_KP_7));
    bool acionarChuteBaixo = IsKeyPressed(p->controles.chuteBaixo) || (!ehJogador1 && IsKeyPressed(KEY_KP_5));
    bool acionarChuteAlto = IsKeyPressed(p->controles.chuteAlto) || (!ehJogador1 && IsKeyPressed(KEY_KP_8));

    if (acionarSocoBaixo)
    {
        p->estado = ESTADO_ATACANDO_SOCO_BAIXO;
        p->temporizador = TEMPO_ATAQUE;
        return;
    }
    if (acionarSocoAlto)
    {
        p->estado = ESTADO_ATACANDO_SOCO_ALTO;
        p->temporizador = TEMPO_ATAQUE;
        return;
    }
    if (acionarChuteBaixo)
    {
        p->estado = ESTADO_ATACANDO_CHUTE_BAIXO;
        p->temporizador = TEMPO_ATAQUE;
        return;
    }
    if (acionarChuteAlto)
    {
        p->estado = ESTADO_ATACANDO_CHUTE_ALTO;
        p->temporizador = TEMPO_ATAQUE;
        return;
    }

    // Movimentação horizontal
    int direcaoHorizontal = 0;
    if (IsKeyDown(p->controles.esquerda))
        direcaoHorizontal -= 1;
    if (IsKeyDown(p->controles.direita))
        direcaoHorizontal += 1;

    if (direcaoHorizontal != 0)
    {
        p->posicao.x += VELOCIDADE_PADRAO * (float)direcaoHorizontal;
        p->direcaoMovimento = direcaoHorizontal;
    }

    p->estado = (direcaoHorizontal != 0) ? ESTADO_ANDANDO : ESTADO_IDLE;

    // Limites da tela
    float margem = LARGURA_CORPO / 2.0f;
    if (p->posicao.x - margem < 0.0f)
        p->posicao.x = margem;
    if (p->posicao.x + margem > SCREEN_WIDTH)
        p->posicao.x = SCREEN_WIDTH - margem;
}

void AtualizarOrientacao(Personagem *p1, Personagem *p2)
{
    if (!EstaAtacando(p1->estado))
    {
        p1->direcao = (p1->posicao.x < p2->posicao.x) ? OLHANDO_DIREITA : OLHANDO_ESQUERDA;
    }
    if (!EstaAtacando(p2->estado))
    {
        p2->direcao = (p2->posicao.x < p1->posicao.x) ? OLHANDO_DIREITA : OLHANDO_ESQUERDA;
    }
}

void AtualizarCaixas(Personagem *p)
{
    // Hurtbox: cobre o corpo do lutador
    p->hurtbox = (Rectangle){
        p->posicao.x - (LARGURA_CORPO / 2.0f),
        p->posicao.y - ALTURA_CORPO,
        LARGURA_CORPO,
        ALTURA_CORPO};

    // Hitbox: projetada à frente na direção em que o jogador está virado
    float ataqueX = (p->direcao == OLHANDO_DIREITA)
                        ? (p->posicao.x + (LARGURA_CORPO / 2.0f))
                        : (p->posicao.x - (LARGURA_CORPO / 2.0f) - LARGURA_HITBOX);

    // Ajusta a altura da hitbox de acordo com golpe alto ou baixo
    bool golpeAlto = (p->estado == ESTADO_ATACANDO_SOCO_ALTO || p->estado == ESTADO_ATACANDO_CHUTE_ALTO);
    float ataqueY = golpeAlto
                        ? (p->posicao.y - ALTURA_CORPO + 20.0f)
                        : (p->posicao.y - 55.0f);

    p->hitbox = (Rectangle){
        ataqueX,
        ataqueY,
        LARGURA_HITBOX,
        ALTURA_HITBOX};
}

void ProcessarColisoes(Personagem *p1, Personagem *p2)
{
    // 1. Bloqueio de sobreposição de corpos
    if (CheckCollisionRecs(p1->hurtbox, p2->hurtbox))
    {
        if (p1->posicao.x < p2->posicao.x)
        {
            p1->posicao.x -= 2.0f;
            p2->posicao.x += 2.0f;
        }
        else
        {
            p1->posicao.x += 2.0f;
            p2->posicao.x -= 2.0f;
        }
        AtualizarCaixas(p1);
        AtualizarCaixas(p2);
    }

    // 2. Dano P1 -> P2
    if (p1->hitboxAtiva && CheckCollisionRecs(p1->hitbox, p2->hurtbox))
    {
        p2->hp -= ObterDanoAtaque(p1->estado);
        p2->estado = ESTADO_HIT;
        p2->temporizador = TEMPO_HIT;
        p1->hitboxAtiva = false;

        if (p2->hp <= 0)
        {
            p2->hp = 0;
            p2->estado = ESTADO_KO;
        }
    }

    // 3. Dano P2 -> P1
    if (p2->hitboxAtiva && CheckCollisionRecs(p2->hitbox, p1->hurtbox))
    {
        p1->hp -= ObterDanoAtaque(p2->estado);
        p1->estado = ESTADO_HIT;
        p1->temporizador = TEMPO_HIT;
        p2->hitboxAtiva = false;

        if (p1->hp <= 0)
        {
            p1->hp = 0;
            p1->estado = ESTADO_KO;
        }
    }
}