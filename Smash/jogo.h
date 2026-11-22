#ifndef JOGO_H
#define JOGO_H

#include "definicoes.h"
#include "combate.h"

void InicializarEstadoJogo(EstadoJogo *estado);
void AtualizarMenu(EstadoJogo *estado);
void DesenharMenu(const EstadoJogo *estado);
void AtualizarSelecao(EstadoJogo *estado, Personagem *p1, Personagem *p2);
void DesenharSelecao(const EstadoJogo *estado);

#endif