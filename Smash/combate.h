#ifndef COMBATE_H
#define COMBATE_H

#include "definicoes.h"
#include "audio.h"

void InicializarPersonagens(Personagem *p1, Personagem *p2);
void AtualizarControles(Personagem *p, GerenciadorAudio *audio, float deltaTime);
void AtualizarOrientacao(Personagem *p1, Personagem *p2);
void AtualizarCaixas(Personagem *p);
void ProcessarColisoes(Personagem *p1, Personagem *p2, GerenciadorAudio *audio);

#endif
