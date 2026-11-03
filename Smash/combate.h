#ifndef COMBATE_H
#define COMBATE_H

#include "definicoes.h"

void InicializarPersonagens(Personagem *p1, Personagem *p2);
void AtualizarControles(Personagem *p);
void AtualizarOrientacao(Personagem *p1, Personagem *p2);
void AtualizarCaixas(Personagem *p);
void ProcessarColisoes(Personagem *p1, Personagem *p2);
void DesenharPersonagemDebug(const Personagem *p);

#endif
