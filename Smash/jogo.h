#ifndef JOGO_H
#define JOGO_H

#include "definicoes.h"
#include "combate.h"

void InicializarEstadoJogo(EstadoJogo *estado);
void AtualizarMenu(EstadoJogo *estado);
void DesenharMenu(const EstadoJogo *estado);
void AtualizarSelecao(EstadoJogo *estado, Personagem *p1, Personagem *p2);
void DesenharSelecao(const EstadoJogo *estado);

void IniciarNovoRound(EstadoJogo *estado, Personagem *p1, Personagem *p2);
bool AtualizarCombateRounds(EstadoJogo *estado, Personagem *p1, Personagem *p2, float dt);
void DesenharHUDRounds(const EstadoJogo *estado);
void AtualizarVitoria(EstadoJogo *estado);
void DesenharVitoria(const EstadoJogo *estado);

#endif