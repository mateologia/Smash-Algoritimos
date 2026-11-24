# Audio do jogo

Esta pasta guarda a trilha sonora e os efeitos sonoros gerenciados por
`audio.c` / `audio.h` (`GerenciadorAudio`).

Basta criar os arquivos com os nomes abaixo. Nenhum codigo precisa ser alterado
para adicionar o audio: o gerenciador carrega o que existir e ignora o resto
(o jogo continua funcionando em silencio).

## Formatos aceitos

O carregador procura, nesta ordem: `.ogg`, `.wav`, `.mp3`, `.flac`.
Recomendado: `.ogg` para musicas (streaming) e `.wav` para efeitos (baixa latencia).

## Musicas (`audio/musicas/`)

| Arquivo          | Quando toca                     |
| ---------------- | ------------------------------- |
| `menu.ogg`       | Tela de menu principal          |
| `selecao.ogg`    | Tela de selecao de lutadores    |
| `combate.ogg`    | Durante as lutas                |
| `vitoria.ogg`    | Tela de resultado da partida    |

## Efeitos (`audio/efeitos/`)

| Arquivo             | Evento no jogo                                   |
| ------------------- | ------------------------------------------------ |
| `menu-mover.ogg`    | Navegar no menu / trocar personagem              |
| `menu-confirmar.ogg`| Confirmar uma opcao / voltar / sair              |
| `soco.ogg`          | Inicio de um soco (baixo ou alto)                |
| `chute.ogg`         | Inicio de um chute (baixo ou alto)               |
| `especial.ogg`      | Golpe especial (Baixo -> Frente + Soco)          |
| `bloqueio.ogg`      | Golpe defendido                                  |
| `impacto.ogg`       | Golpe conectado em cheio                        |
| `ko.ogg`            | Nocaute ou fim do round por tempo                |
| `finish-him.ogg`    | Inicio do "FINISH HIM!"                          |
| `fatality.ogg`      | Execucao do Fatality                             |
| `round-inicio.ogg`  | Anuncio de "ROUND X" / "FIGHT!"                  |
| `vitoria.ogg`       | Ao entrar na tela de vitoria                     |

## Volumes

Os volumes iniciais ficam em `audio.h`:

- `VOLUME_MUSICA_PADRAO` (0.6)
- `VOLUME_EFEITOS_PADRAO` (0.9)

Em tempo de execucao use `DefinirVolumeMusica()` e `DefinirVolumeEfeitos()`.

## Onde os arquivos sao procurados

Como o jogo pode ser executado pela raiz do repositorio (`run.sh`) ou pela pasta
`Smash/` (Code::Blocks), o carregador tenta:

1. `Smash/audio/...`
2. `audio/...`
3. `../Smash/audio/...`

Exemplo: `musicas/menu` vira `Smash/audio/musicas/menu.ogg`.
