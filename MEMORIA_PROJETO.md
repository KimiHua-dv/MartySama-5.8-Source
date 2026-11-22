# MEMORIA_PROJETO.md — Metin2 MartySama 5.8

Memória viva do projeto. A IA deve **ler este arquivo no início de cada sessão** e **atualizá-lo ao final**.
Mantenha curto: só o que muda decisões futuras. Detalhe de código fica no repositório.

Última atualização: (preencher)

---

## 1. Decisões firmes (não reabrir sem motivo)

- Base: MartySama 5.8. Repositório: https://github.com/KimiHua-dv/MartySama-5.8-Source
- Ambiente: servidor FreeBSD 14.4 (VirtualBox), cliente Windows 11, MariaDB. Comandos de terminal sempre compatíveis com FreeBSD.
- Festival de Guildas: evento único (`FESTIVAL_EVENT_ID = 1`), sem gerenciador de múltiplas edições.
- Festival: dados operacionais preservados no encerramento; limpeza só na abertura da próxima edição; histórico em `festival_guild_history` (`UNIQUE(edition_key, guild_id)`).
- Festival: não reiniciar `AUTO_INCREMENT` de `festival_guild_mission`.
- Sistema de reinos: todos começam neutros; máximo de 3 reinos por jogador; imperador é quem derrota o Boss do reino sozinho.

## 2. Status por sistema

### Festival de Guildas
- Pronto: distribuição de missões, hooks (Metin, monstro, PvP, coleta), progresso individual, expiração, pontuação coletiva, comando GM, `ProcessCycle()`, tabela de histórico, encerramento manual testado.
- Em andamento: `FinalizeFestivalEvent()` (proposta, não compilada), integração dos 3 caminhos de encerramento.
- Pendente: protocolo da lista (`FESTIVAL_LIST_ACTIVE/UNAVAILABLE`), parser C++ e Python do cliente, resgate dos baús, painel histórico (futuro).

### Sistema de reinos
- (preencher status)

### Fila de Metins
- (preencher status)

## 3. Pendências globais (ordem de prioridade)

1. Finalizar o ciclo de encerramento do Festival.
2. (adicionar)

## 4. Problemas conhecidos / armadilhas

- Struct de pacote deve ser idêntica no servidor e no cliente (tamanho e ordem).
- (adicionar o que já deu errado e a causa)

---

## Diário de sessões

(Entradas novas são adicionadas no fim deste arquivo pelo `atualizar_memoria.sh` ou pela IA no final da sessão.)
