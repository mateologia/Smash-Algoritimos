#ifndef AUDIO_H
#define AUDIO_H

#include "raylib.h"
#include <stdbool.h>

// Volumes iniciais (0.0f a 1.0f)
#define VOLUME_MUSICA_PADRAO 0.6f
#define VOLUME_EFEITOS_PADRAO 0.9f

// Faixas de trilha sonora, uma por tela do jogo
typedef enum
{
    MUSICA_NENHUMA = -1,
    MUSICA_MENU = 0,
    MUSICA_SELECAO,
    MUSICA_COMBATE,
    MUSICA_VITORIA,
    MUSICA_TOTAL
} FaixaMusica;

// Efeitos sonoros disparados pelos eventos do jogo
typedef enum
{
    SOM_MENU_MOVER = 0,
    SOM_MENU_CONFIRMAR,
    SOM_SOCO,
    SOM_CHUTE,
    SOM_ESPECIAL,
    SOM_BLOQUEIO,
    SOM_IMPACTO,
    SOM_KO,
    SOM_FINISH_HIM,
    SOM_FATALITY,
    SOM_ROUND_INICIO,
    SOM_VITORIA,
    SOM_TOTAL
} EfeitoSonoro;

typedef struct
{
    Music musicas[MUSICA_TOTAL];
    bool musicaCarregada[MUSICA_TOTAL];

    Sound efeitos[SOM_TOTAL];
    bool efeitoCarregado[SOM_TOTAL];

    int musicaAtual;      // MUSICA_NENHUMA ou a FaixaMusica em reproducao
    bool musicaTocando;
    bool audioDisponivel; // false quando o dispositivo de audio nao inicializa

    float volumeMusica;
    float volumeEfeitos;
} GerenciadorAudio;

// Inicializa o dispositivo de audio e carrega os arquivos encontrados.
// Arquivos ausentes sao ignorados (o jogo roda em silencio).
void InicializarAudio(GerenciadorAudio *audio);

// Deve ser chamada uma vez por frame: alimenta o stream da musica atual.
void AtualizarAudio(GerenciadorAudio *audio);

// Libera musicas, efeitos e o dispositivo de audio.
void DescarregarAudio(GerenciadorAudio *audio);

// Troca a trilha sonora. Chamar com a faixa ja tocando nao reinicia a musica.
void TocarMusica(GerenciadorAudio *audio, FaixaMusica faixa);
void PararMusica(GerenciadorAudio *audio);

// Dispara um efeito sonoro. Ignorado se o arquivo nao estiver carregado.
void TocarEfeito(GerenciadorAudio *audio, EfeitoSonoro efeito);

void DefinirVolumeMusica(GerenciadorAudio *audio, float volume);
void DefinirVolumeEfeitos(GerenciadorAudio *audio, float volume);

bool AudioDisponivel(const GerenciadorAudio *audio);

// Caminho relativo esperado do arquivo (util para depuracao/documentacao)
const char *ObterCaminhoMusica(FaixaMusica faixa);
const char *ObterCaminhoEfeito(EfeitoSonoro efeito);

#endif
