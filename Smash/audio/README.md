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

| Arquivo           | Quando toca                                              |
| ----------------- | -------------------------------------------------------- |
| `pillar-men.*`    | Desde o inicio do programa, em loop, ate o jogo fechar   |

## Efeitos (`audio/efeitos/`)

| Arquivo             | Evento no jogo                                            |
| ------------------- | --------------------------------------------------------- |
| `menu-mover.*`      | Navegar no menu / trocar personagem                       |
| `menu-confirmar.*`  | Confirmar uma opcao / voltar / sair                       |
| `soco.*`            | Inicio de um soco (baixo ou alto)                         |
| `chute.*`           | Inicio de um chute (baixo ou alto)                        |
| `especial.*`        | Golpe especial (Baixo -> Frente + Soco)                   |
| `bloqueio.*`        | Golpe defendido                                           |
| `ko.*`              | Nocaute ou fim do round por tempo                         |
| `finish-him.*`      | Inicio do "FINISH HIM!"                                   |
| `fatality.*`        | Execucao do Fatality                                      |
| `round-inicio.*`    | Anuncio de "ROUND X" / "FIGHT!" nos rounds seguintes      |
| `game-start.*`      | Primeiro round: partida comeca apos a selecao             |
| `game-end.*`        | Fim da partida, apos a animacao de derrota                |
| `death-dio.*`       | Golpe fatal no DIO (substitui o som normal de hit)        |
| `death-blanka.*`    | Golpe fatal no BLANKA (substitui o som normal de hit)     |
| `hit-1.*` a `hit-6.*`| Golpe conectado em qualquer lutador (sorteado a cada hit) |

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

Exemplo: `musicas/pillar-men` vira `Smash/audio/musicas/pillar-men.mp3`.
