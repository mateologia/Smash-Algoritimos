#ifndef JOGO_H
#define JOGO_H

#include "definicoes.h"
#include "combate.h"
#include "audio.h"

void InicializarEstadoJogo(EstadoJogo *estado);
void AtualizarMenu(EstadoJogo *estado, GerenciadorAudio *audio);
void DesenharMenu(const EstadoJogo *estado);
void AtualizarSelecao(EstadoJogo *estado, Personagem *p1, Personagem *p2, GerenciadorAudio *audio);
void DesenharSelecao(const EstadoJogo *estado);

void IniciarNovoRound(EstadoJogo *estado, Personagem *p1, Personagem *p2, GerenciadorAudio *audio);
bool AtualizarCombateRounds(EstadoJogo *estado, Personagem *p1, Personagem *p2, float dt, GerenciadorAudio *audio);
void DesenharHUDRounds(const EstadoJogo *estado);
void AtualizarVitoria(EstadoJogo *estado, GerenciadorAudio *audio);
void DesenharVitoria(const EstadoJogo *estado);
void DescarregarRecursosJogo(EstadoJogo *estado);
const char *ObterNomeLutador(int id);

#endif
