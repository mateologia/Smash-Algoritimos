#include "audio.h"

#include <stddef.h>
#include <stdio.h>

// O carregador procura os arquivos em cada uma destas raizes, nesta ordem.
// Isso cobre tanto a execucao pela raiz do repositorio (run.sh) quanto
// pela pasta Smash/ (Code::Blocks).
static const char *const RAIZES_AUDIO[] = {
    "Smash/audio",
    "audio",
    "../Smash/audio",
};

// A primeira extensao existente e usada, permitindo trocar o formato
// dos arquivos sem alterar o codigo.
static const char *const EXTENSOES_AUDIO[] = {
    ".ogg",
    ".wav",
    ".mp3",
    ".flac",
};

// Caminhos relativos as raizes acima, sem extensao
static const char *const ARQUIVOS_MUSICA[MUSICA_TOTAL] = {
    [MUSICA_MENU] = "musicas/menu",
    [MUSICA_SELECAO] = "musicas/selecao",
    [MUSICA_COMBATE] = "musicas/combate",
    [MUSICA_VITORIA] = "musicas/vitoria",
};

static const char *const ARQUIVOS_EFEITO[SOM_TOTAL] = {
    [SOM_MENU_MOVER] = "efeitos/menu-mover",
    [SOM_MENU_CONFIRMAR] = "efeitos/menu-confirmar",
    [SOM_SOCO] = "efeitos/soco",
    [SOM_CHUTE] = "efeitos/chute",
    [SOM_ESPECIAL] = "efeitos/especial",
    [SOM_BLOQUEIO] = "efeitos/bloqueio",
    [SOM_IMPACTO] = "efeitos/impacto",
    [SOM_KO] = "efeitos/ko",
    [SOM_FINISH_HIM] = "efeitos/finish-him",
    [SOM_FATALITY] = "efeitos/fatality",
    [SOM_ROUND_INICIO] = "efeitos/round-inicio",
    [SOM_VITORIA] = "efeitos/vitoria",
};

static float LimitarVolume(float volume)
{
    if (volume < 0.0f)
        return 0.0f;
    if (volume > 1.0f)
        return 1.0f;
    return volume;
}

// Procura o arquivo em todas as raizes e extensoes conhecidas.
static bool ResolverCaminhoAudio(const char *relativo, char *destino, size_t tamanho)
{
    if (relativo == NULL || destino == NULL || tamanho == 0)
        return false;

    size_t totalRaizes = sizeof(RAIZES_AUDIO) / sizeof(RAIZES_AUDIO[0]);
    size_t totalExtensoes = sizeof(EXTENSOES_AUDIO) / sizeof(EXTENSOES_AUDIO[0]);

    for (size_t r = 0; r < totalRaizes; r++)
    {
        for (size_t e = 0; e < totalExtensoes; e++)
        {
            snprintf(destino, tamanho, "%s/%s%s", RAIZES_AUDIO[r], relativo, EXTENSOES_AUDIO[e]);
            if (FileExists(destino))
                return true;
        }
    }

    destino[0] = '\0';
    return false;
}

static bool CarregarMusica(GerenciadorAudio *audio, FaixaMusica faixa)
{
    char caminho[512];
    if (!ResolverCaminhoAudio(ARQUIVOS_MUSICA[faixa], caminho, sizeof(caminho)))
    {
        TraceLog(LOG_DEBUG, "AUDIO: musica ausente: %s.ogg", ARQUIVOS_MUSICA[faixa]);
        return false;
    }

    Music musica = LoadMusicStream(caminho);
    if (musica.frameCount == 0)
    {
        TraceLog(LOG_WARNING, "AUDIO: falha ao carregar musica: %s", caminho);
        return false;
    }

    musica.looping = true;
    audio->musicas[faixa] = musica;
    audio->musicaCarregada[faixa] = true;
    SetMusicVolume(audio->musicas[faixa], audio->volumeMusica);
    TraceLog(LOG_INFO, "AUDIO: musica carregada: %s", caminho);
    return true;
}

static bool CarregarEfeito(GerenciadorAudio *audio, EfeitoSonoro efeito)
{
    char caminho[512];
    if (!ResolverCaminhoAudio(ARQUIVOS_EFEITO[efeito], caminho, sizeof(caminho)))
    {
        TraceLog(LOG_DEBUG, "AUDIO: efeito ausente: %s.ogg", ARQUIVOS_EFEITO[efeito]);
        return false;
    }

    Sound som = LoadSound(caminho);
    if (som.frameCount == 0)
    {
        TraceLog(LOG_WARNING, "AUDIO: falha ao carregar efeito: %s", caminho);
        return false;
    }

    audio->efeitos[efeito] = som;
    audio->efeitoCarregado[efeito] = true;
    SetSoundVolume(audio->efeitos[efeito], audio->volumeEfeitos);
    TraceLog(LOG_INFO, "AUDIO: efeito carregado: %s", caminho);
    return true;
}

void InicializarAudio(GerenciadorAudio *audio)
{
    if (audio == NULL)
        return;

    *audio = (GerenciadorAudio){0};
    audio->musicaAtual = MUSICA_NENHUMA;
    audio->volumeMusica = VOLUME_MUSICA_PADRAO;
    audio->volumeEfeitos = VOLUME_EFEITOS_PADRAO;

    if (!IsAudioDeviceReady())
        InitAudioDevice();

    audio->audioDisponivel = IsAudioDeviceReady();
    if (!audio->audioDisponivel)
    {
        TraceLog(LOG_WARNING, "AUDIO: dispositivo de audio indisponivel - jogo seguira em silencio");
        return;
    }

    int musicasCarregadas = 0;
    for (int i = 0; i < MUSICA_TOTAL; i++)
    {
        if (CarregarMusica(audio, (FaixaMusica)i))
            musicasCarregadas++;
    }

    int efeitosCarregados = 0;
    for (int i = 0; i < SOM_TOTAL; i++)
    {
        if (CarregarEfeito(audio, (EfeitoSonoro)i))
            efeitosCarregados++;
    }

    TraceLog(LOG_INFO, "AUDIO: %d/%d musicas e %d/%d efeitos carregados",
             musicasCarregadas, MUSICA_TOTAL, efeitosCarregados, SOM_TOTAL);

    if (musicasCarregadas < MUSICA_TOTAL || efeitosCarregados < SOM_TOTAL)
    {
        TraceLog(LOG_INFO, "AUDIO: adicione os arquivos em Smash/audio (veja o README.md da pasta)");
    }
}

void AtualizarAudio(GerenciadorAudio *audio)
{
    if (audio == NULL || !audio->audioDisponivel)
        return;

    if (audio->musicaTocando && audio->musicaAtual >= 0 &&
        audio->musicaCarregada[audio->musicaAtual])
    {
        UpdateMusicStream(audio->musicas[audio->musicaAtual]);
    }
}

void DescarregarAudio(GerenciadorAudio *audio)
{
    if (audio == NULL)
        return;

    for (int i = 0; i < MUSICA_TOTAL; i++)
    {
        if (audio->musicaCarregada[i])
        {
            UnloadMusicStream(audio->musicas[i]);
            audio->musicaCarregada[i] = false;
        }
    }

    for (int i = 0; i < SOM_TOTAL; i++)
    {
        if (audio->efeitoCarregado[i])
        {
            UnloadSound(audio->efeitos[i]);
            audio->efeitoCarregado[i] = false;
        }
    }

    audio->musicaAtual = MUSICA_NENHUMA;
    audio->musicaTocando = false;

    if (audio->audioDisponivel)
    {
        CloseAudioDevice();
        audio->audioDisponivel = false;
    }
}

void TocarMusica(GerenciadorAudio *audio, FaixaMusica faixa)
{
    if (audio == NULL || faixa <= MUSICA_NENHUMA || faixa >= MUSICA_TOTAL)
        return;

    // Nao reinicia a faixa se ela ja esta selecionada e tocando
    if (audio->musicaAtual == (int)faixa &&
        (audio->musicaTocando || !audio->musicaCarregada[faixa]))
        return;

    if (audio->musicaAtual >= 0 && audio->musicaAtual < MUSICA_TOTAL &&
        audio->musicaCarregada[audio->musicaAtual])
    {
        StopMusicStream(audio->musicas[audio->musicaAtual]);
    }

    audio->musicaAtual = (int)faixa;
    audio->musicaTocando = false;

    if (!audio->audioDisponivel || !audio->musicaCarregada[faixa])
        return;

    SetMusicVolume(audio->musicas[faixa], audio->volumeMusica);
    PlayMusicStream(audio->musicas[faixa]);
    audio->musicaTocando = true;
}

void PararMusica(GerenciadorAudio *audio)
{
    if (audio == NULL)
        return;

    if (audio->musicaAtual >= 0 && audio->musicaAtual < MUSICA_TOTAL &&
        audio->musicaCarregada[audio->musicaAtual])
    {
        StopMusicStream(audio->musicas[audio->musicaAtual]);
    }

    audio->musicaAtual = MUSICA_NENHUMA;
    audio->musicaTocando = false;
}

void TocarEfeito(GerenciadorAudio *audio, EfeitoSonoro efeito)
{
    if (audio == NULL || !audio->audioDisponivel)
        return;
    if (efeito < 0 || efeito >= SOM_TOTAL || !audio->efeitoCarregado[efeito])
        return;

    PlaySound(audio->efeitos[efeito]);
}

void DefinirVolumeMusica(GerenciadorAudio *audio, float volume)
{
    if (audio == NULL)
        return;

    audio->volumeMusica = LimitarVolume(volume);

    for (int i = 0; i < MUSICA_TOTAL; i++)
    {
        if (audio->musicaCarregada[i])
            SetMusicVolume(audio->musicas[i], audio->volumeMusica);
    }
}

void DefinirVolumeEfeitos(GerenciadorAudio *audio, float volume)
{
    if (audio == NULL)
        return;

    audio->volumeEfeitos = LimitarVolume(volume);

    for (int i = 0; i < SOM_TOTAL; i++)
    {
        if (audio->efeitoCarregado[i])
            SetSoundVolume(audio->efeitos[i], audio->volumeEfeitos);
    }
}

bool AudioDisponivel(const GerenciadorAudio *audio)
{
    return audio != NULL && audio->audioDisponivel;
}

const char *ObterCaminhoMusica(FaixaMusica faixa)
{
    static const char *const CAMINHOS[MUSICA_TOTAL] = {
        [MUSICA_MENU] = "audio/musicas/menu.ogg",
        [MUSICA_SELECAO] = "audio/musicas/selecao.ogg",
        [MUSICA_COMBATE] = "audio/musicas/combate.ogg",
        [MUSICA_VITORIA] = "audio/musicas/vitoria.ogg",
    };

    if (faixa < 0 || faixa >= MUSICA_TOTAL)
        return NULL;

    return CAMINHOS[faixa];
}

const char *ObterCaminhoEfeito(EfeitoSonoro efeito)
{
    if (efeito < 0 || efeito >= SOM_TOTAL)
        return NULL;

    static const char *const CAMINHOS[SOM_TOTAL] = {
        [SOM_MENU_MOVER] = "audio/efeitos/menu-mover.ogg",
        [SOM_MENU_CONFIRMAR] = "audio/efeitos/menu-confirmar.ogg",
        [SOM_SOCO] = "audio/efeitos/soco.ogg",
        [SOM_CHUTE] = "audio/efeitos/chute.ogg",
        [SOM_ESPECIAL] = "audio/efeitos/especial.ogg",
        [SOM_BLOQUEIO] = "audio/efeitos/bloqueio.ogg",
        [SOM_IMPACTO] = "audio/efeitos/impacto.ogg",
        [SOM_KO] = "audio/efeitos/ko.ogg",
        [SOM_FINISH_HIM] = "audio/efeitos/finish-him.ogg",
        [SOM_FATALITY] = "audio/efeitos/fatality.ogg",
        [SOM_ROUND_INICIO] = "audio/efeitos/round-inicio.ogg",
        [SOM_VITORIA] = "audio/efeitos/vitoria.ogg",
    };

    return CAMINHOS[efeito];
}
