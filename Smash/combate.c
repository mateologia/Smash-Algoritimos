#include "combate.h"
#include <string.h>

static bool EstaAtacando(EstadoPersonagem estado)
{
    return (estado == ESTADO_ATACANDO_SOCO_BAIXO ||
            estado == ESTADO_ATACANDO_SOCO_ALTO ||
            estado == ESTADO_ATACANDO_CHUTE_BAIXO ||
            estado == ESTADO_ATACANDO_CHUTE_ALTO ||
            estado == ESTADO_ESPECIAL ||
            estado == ESTADO_FATALITY);
}

static int ObterDanoAtaque(EstadoPersonagem estado)
{
    switch (estado)
    {
        case ESTADO_ATACANDO_SOCO_BAIXO:  return DANO_SOCO_BAIXO;
        case ESTADO_ATACANDO_SOCO_ALTO:   return DANO_SOCO_ALTO;
        case ESTADO_ATACANDO_CHUTE_BAIXO: return DANO_CHUTE_BAIXO;
        case ESTADO_ATACANDO_CHUTE_ALTO:  return DANO_CHUTE_ALTO;
        case ESTADO_ESPECIAL:             return DANO_ESPECIAL;
        default:                          return 0;
    }
}

void InicializarPersonagens(Personagem *p1, Personagem *p2)
{
    memset(p1, 0, sizeof(*p1));
    memset(p2, 0, sizeof(*p2));

    // Configuração Jogador 1 (centralizado na arena de 1600px - BONUS-06)
    p1->id = 1;
    p1->posicao = (Vector2){550.0f, CHAO_Y};
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
    p2->posicao = (Vector2){1050.0f, CHAO_Y};
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

void AtualizarControles(Personagem *p, GerenciadorAudio *audio)
{
    p->direcaoMovimento = 0;
    p->bloqueando = false;
    p->agachado = false;

    if (p->tempoCombo > 0)
    {
        p->tempoCombo--;
        if (p->tempoCombo <= 0)
        {
            p->contadorCombo = 0;
        }
    }

    // Física vertical (gravidade sempre atua, mesmo em hitstun ou atacando no ar)
    if (p->posicao.y < CHAO_Y || p->velocidadeY < 0.0f)
    {
        p->posicao.y += p->velocidadeY;
        p->velocidadeY += GRAVIDADE;
        if (p->posicao.y >= CHAO_Y)
        {
            p->posicao.y = CHAO_Y;
            p->velocidadeY = 0.0f;
        }
    }

    bool noChao = (p->posicao.y >= CHAO_Y);

    if (p->estado == ESTADO_KO || p->estado == ESTADO_TONTO)
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

    // Se estiver executando qualquer ataque (normal, especial ou fatality)
    if (EstaAtacando(p->estado))
    {
        p->temporizador--;
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

    // Atualiza buffer de sequência para Golpe Especial (BONUS-04: Baixo -> Frente + Soco)
    if (p->timerEspecial > 0)
    {
        p->timerEspecial--;
        if (p->timerEspecial <= 0)
        {
            p->etapaEspecial = 0;
        }
    }

    // Atualiza buffer de sequência para Fatality (BONUS-05: Baixo -> Baixo + Chute Alto)
    if (p->timerFatality > 0)
    {
        p->timerFatality--;
        if (p->timerFatality <= 0)
        {
            p->etapaFatality = 0;
        }
    }

    KeyboardKey teclaFrente = (p->direcao == OLHANDO_DIREITA) ? p->controles.direita : p->controles.esquerda;
    if (IsKeyPressed(p->controles.baixo))
    {
        p->etapaEspecial = 1;
        p->timerEspecial = JANELA_COMANDO_ESPECIAL;

        if (p->etapaFatality == 1 && p->timerFatality > 0)
        {
            p->etapaFatality = 2;
            p->timerFatality = JANELA_COMANDO_FATALITY;
        }
        else
        {
            p->etapaFatality = 1;
            p->timerFatality = JANELA_COMANDO_FATALITY;
        }
    }
    else if (p->etapaEspecial == 1 && IsKeyPressed(teclaFrente))
    {
        p->etapaEspecial = 2;
        p->timerEspecial = JANELA_COMANDO_ESPECIAL;
    }

    // Prioridade para acionar novos ataques (cada jogador isolado nos seus botões)
    bool ehJogador1 = (p->controles.esquerda == KEY_A);
    bool acionarSocoBaixo = IsKeyPressed(p->controles.socoBaixo) || (!ehJogador1 && IsKeyPressed(KEY_KP_4));
    bool acionarSocoAlto = IsKeyPressed(p->controles.socoAlto) || (!ehJogador1 && IsKeyPressed(KEY_KP_7));
    bool acionarChuteBaixo = IsKeyPressed(p->controles.chuteBaixo) || (!ehJogador1 && IsKeyPressed(KEY_KP_5));
    bool acionarChuteAlto = IsKeyPressed(p->controles.chuteAlto) || (!ehJogador1 && IsKeyPressed(KEY_KP_8));

    if ((acionarSocoBaixo || acionarSocoAlto) && p->etapaEspecial == 2)
    {
        p->estado = ESTADO_ESPECIAL;
        p->temporizador = TEMPO_ATAQUE + 4;
        p->etapaEspecial = 0;
        p->timerEspecial = 0;
        p->agachado = false;
        TocarEfeito(audio, SOM_ESPECIAL);
        return;
    }

    if (acionarSocoBaixo)
    {
        p->estado = ESTADO_ATACANDO_SOCO_BAIXO;
        p->temporizador = TEMPO_ATAQUE;
        TocarEfeito(audio, SOM_SOCO);
        return;
    }
    if (acionarSocoAlto)
    {
        p->estado = ESTADO_ATACANDO_SOCO_ALTO;
        p->temporizador = TEMPO_ATAQUE;
        TocarEfeito(audio, SOM_SOCO);
        return;
    }
    if (acionarChuteBaixo)
    {
        p->estado = ESTADO_ATACANDO_CHUTE_BAIXO;
        p->temporizador = TEMPO_ATAQUE;
        TocarEfeito(audio, SOM_CHUTE);
        return;
    }
    if (acionarChuteAlto)
    {
        p->estado = ESTADO_ATACANDO_CHUTE_ALTO;
        p->temporizador = TEMPO_ATAQUE;
        TocarEfeito(audio, SOM_CHUTE);
        return;
    }

    // Pulo (W / Seta Cima) e Agachar (S / Seta Baixo) quando no chão
    if (noChao && IsKeyPressed(p->controles.cima))
    {
        p->velocidadeY = FORCA_PULO;
        noChao = false;
    }
    else if (noChao && IsKeyDown(p->controles.baixo))
    {
        p->agachado = true;
    }

    // Bloqueio (BONUS-01): segurar para trás em relação ao oponente
    bool segurandoTras = (p->direcao == OLHANDO_DIREITA && IsKeyDown(p->controles.esquerda)) ||
                         (p->direcao == OLHANDO_ESQUERDA && IsKeyDown(p->controles.direita));
    if (segurandoTras && noChao)
    {
        p->bloqueando = true;
    }

    // Movimentação horizontal (não anda enquanto está agachado)
    int direcaoHorizontal = 0;
    if (!p->agachado)
    {
        if (IsKeyDown(p->controles.esquerda))
            direcaoHorizontal -= 1;
        if (IsKeyDown(p->controles.direita))
            direcaoHorizontal += 1;
    }

    if (direcaoHorizontal != 0)
    {
        float velocidade = p->bloqueando ? (VELOCIDADE_PADRAO * 0.5f) : VELOCIDADE_PADRAO;
        p->posicao.x += velocidade * (float)direcaoHorizontal;
        p->direcaoMovimento = direcaoHorizontal;
    }

    p->estado = (direcaoHorizontal != 0) ? ESTADO_ANDANDO : ESTADO_IDLE;

    // Limites da arena larga (BONUS-06)
    float margem = LARGURA_CORPO / 2.0f;
    if (p->posicao.x - margem < 0.0f)
        p->posicao.x = margem;
    if (p->posicao.x + margem > LARGURA_ARENA)
        p->posicao.x = LARGURA_ARENA - margem;
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
    float alturaAtual = p->agachado ? ALTURA_CORPO_AGACHADO : ALTURA_CORPO;

    // Hurtbox: cobre o corpo do lutador (reduzida quando agachado)
    p->hurtbox = (Rectangle){
        p->posicao.x - (LARGURA_CORPO / 2.0f),
        p->posicao.y - alturaAtual,
        LARGURA_CORPO,
        alturaAtual};

    bool ehEspecial = (p->estado == ESTADO_ESPECIAL);
    float larguraHitbox = ehEspecial ? LARGURA_HITBOX_ESPECIAL : LARGURA_HITBOX;
    float alturaHitbox = ehEspecial ? ALTURA_HITBOX_ESPECIAL : ALTURA_HITBOX;

    // Hitbox: projetada à frente na direção em que o jogador está virado
    float ataqueX = (p->direcao == OLHANDO_DIREITA)
                        ? (p->posicao.x + (LARGURA_CORPO / 2.0f))
                        : (p->posicao.x - (LARGURA_CORPO / 2.0f) - larguraHitbox);

    // Ajusta a altura da hitbox de acordo com golpe especial, alto ou baixo
    bool golpeAlto = (p->estado == ESTADO_ATACANDO_SOCO_ALTO || p->estado == ESTADO_ATACANDO_CHUTE_ALTO);
    float ataqueY = ehEspecial
                        ? (p->posicao.y - ALTURA_CORPO + 35.0f)
                        : (golpeAlto ? (p->posicao.y - ALTURA_CORPO + 20.0f) : (p->posicao.y - 55.0f));

    p->hitbox = (Rectangle){
        ataqueX,
        ataqueY,
        larguraHitbox,
        alturaHitbox};
}

static void AplicarImpacto(Personagem *atacante, Personagem *defensor, GerenciadorAudio *audio)
{
    int danoBase = ObterDanoAtaque(atacante->estado);

    if (defensor->bloqueando)
    {
        int danoBloqueado = (int)((float)danoBase * FATOR_DANO_BLOQUEIO);
        if (danoBloqueado < 1)
        {
            danoBloqueado = 1;
        }
        defensor->hp -= danoBloqueado;
        defensor->estado = ESTADO_HIT;
        defensor->temporizador = TEMPO_BLOQUEIO;
        atacante->contadorCombo = 0;
        atacante->tempoCombo = 0;
        TocarEfeito(audio, SOM_BLOQUEIO);
    }
    else
    {
        // BONUS-03: Encadeamento de combo se o oponente ainda estiver em hitstun ou na janela
        if (defensor->estado == ESTADO_HIT || atacante->tempoCombo > 0)
        {
            atacante->contadorCombo++;
        }
        else
        {
            atacante->contadorCombo = 1;
        }
        atacante->tempoCombo = JANELA_COMBO;

        float multiplicador = 1.0f + ((float)(atacante->contadorCombo - 1) * MULTIPLICADOR_COMBO);
        int danoFinal = (int)((float)danoBase * multiplicador);

        defensor->hp -= danoFinal;
        defensor->estado = ESTADO_HIT;
        defensor->temporizador = TEMPO_HIT;
        TocarEfeito(audio, SOM_IMPACTO);
    }

    atacante->hitboxAtiva = false;

    if (defensor->hp <= 0)
    {
        defensor->hp = 0;
        defensor->estado = ESTADO_KO;
    }
}

void ProcessarColisoes(Personagem *p1, Personagem *p2, GerenciadorAudio *audio)
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

    // 2. Mantém ambos os lutadores dentro do campo de visão da câmera (BONUS-06)
    float distanciaMax = SCREEN_WIDTH - LARGURA_CORPO;
    float diferencaX = p2->posicao.x - p1->posicao.x;
    if (diferencaX > distanciaMax)
    {
        float excesso = (diferencaX - distanciaMax) / 2.0f;
        p1->posicao.x += excesso;
        p2->posicao.x -= excesso;
        AtualizarCaixas(p1);
        AtualizarCaixas(p2);
    }
    else if (diferencaX < -distanciaMax)
    {
        float excesso = (-diferencaX - distanciaMax) / 2.0f;
        p1->posicao.x -= excesso;
        p2->posicao.x += excesso;
        AtualizarCaixas(p1);
        AtualizarCaixas(p2);
    }

    // 3. Dano P1 -> P2
    if (p1->hitboxAtiva && CheckCollisionRecs(p1->hitbox, p2->hurtbox))
    {
        AplicarImpacto(p1, p2, audio);
    }

    // 4. Dano P2 -> P1
    if (p2->hitboxAtiva && CheckCollisionRecs(p2->hitbox, p1->hurtbox))
    {
        AplicarImpacto(p2, p1, audio);
    }
}