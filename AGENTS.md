# AGENTS.md — Metin2 MartySama 5.8

Instruções para agentes de código (Codex, etc.) neste repositório.
Leia este arquivo inteiro antes de qualquer tarefa.

Nenhuma resposta de desenvolvimento deve terminar somente com pesquisa, análise ou planejamento. 
O objetivo de cada etapa é entregar código concreto, baseado na source real da MartySama 5.8, 
para inserção e compilação na VM FreeBSD. Pesquise o necessário, implemente o próximo conjunto funcional
e só interrompa a entrega quando existir um bloqueio técnico comprovado.

## 0. Memória do projeto

- No início de cada tarefa, leia `MEMORIA_PROJETO.md` (decisões firmes, status e pendências).
- No final, atualize esse arquivo: adicione uma entrada no "Diário de sessões" e ajuste as seções 1 a 4 se algo mudou. Registre só o que foi realmente feito e verificado.
- Todos os comandos de terminal que você sugerir devem ser compatíveis com FreeBSD (`/bin/sh`, sem bashisms).

## 1. Como trabalhar (regras de ouro)

- **Não peça confirmação para decisões de implementação.** Escolha a opção mais coerente com o código existente, implemente e liste as suposições no final.
- **Só pergunte se algo for realmente bloqueante** (ex.: duas opções incompatíveis que mudam o banco ou o protocolo de rede e não há como inferir). Nesse caso, faça uma única pergunta objetiva e, enquanto isso, avance no que não depende da resposta.
- **Pesquise no próprio repositório antes de escrever código.** Use busca (grep/ripgrep) para encontrar declarações, definições, chamadas e dependências. Não presuma que algo existe: confirme.
- **Entregue a tarefa completa**, não só um plano. Termine com o código alterado, compilando, quando possível.
- Priorize mecanismos nativos da MartySama (DBManager, CAsyncSQL, sistema de comandos GM, packet_info etc.) em vez de soluções externas.
- Não presuma compatibilidade com outras sources de Metin2.
- Não faça refatorações fora do escopo da tarefa. Mudanças pequenas e focadas.

## 2. Formato da resposta final

Sempre termine com:

1. **O que foi feito** (arquivos e funções alterados).
2. **Suposições adotadas** (decisões que você tomou sem perguntar).
3. **Confirmado na source vs. proposta** (o que você verificou no código e o que é sugestão).
4. **Como compilar e testar** (comandos e passos de teste).
5. **Pendências** (o que ficou de fora e por quê).

O dono do projeto quer entender a lógica: explique de forma técnica e curta o *porquê* de cada alteração importante, sem despejar mudanças não verificadas.

## 3. Ambiente

- Servidor: FreeBSD 14.4 (VirtualBox). Compilação e testes acontecem na VM.
- Cliente: Windows 11 (C++ + interface em Python).
- Banco: MariaDB, banco `srv1_player` para as tabelas dos sistemas customizados.
- GitHub é a referência principal do código. A VM é o ambiente de alteração, compilação e teste.
- Se não for possível compilar no seu ambiente, diga isso explicitamente e indique o comando para o usuário rodar na VM. **Não afirme que compila sem ter compilado.**

## 4. Estrutura relevante (preencher/ajustar caminhos reais)

- `game/src/` — código do servidor game (ex.: `main.cpp`, `input_main.cpp`, `packet_info.cpp`, `cmd_gm.cpp`, `char_battle.cpp`, `char_item.cpp`, `db.cpp`, `festiv.cpp/.h`)
- `libsql/` — `AsyncSQL.cpp` (conexões e `DirectQuery`)
- Cliente: parser de pacotes em C++ e módulos de interface em Python (confirmar os caminhos reais no repositório antes de editar).

## 5. Regras de protocolo de rede

- Qualquer mudança em struct de pacote deve ser feita **no servidor e no cliente ao mesmo tempo**: mesmo tamanho, mesma ordem de campos, mesmo header.
- Ao alterar ou criar pacote, atualizar `packet_info.cpp` (header e tamanho) e o parser do cliente.
- Antes de alterar uma struct, localize no cliente o parser real e confira a serialização.

## 6. Regras de banco de dados

- Mudanças de schema devem vir como script SQL separado (`sql/` ou pasta equivalente), nunca só no código C++.
- Operações que alteram vários registros devem ser feitas em transação. `DBManager::DirectQuery()` usa a conexão persistente `m_hDB`, então comandos consecutivos usam a mesma conexão (confirmado na source).
- Operações de arquivamento/histórico devem ser **idempotentes** (usar `UNIQUE` + `ON DUPLICATE KEY UPDATE`).
- Não reiniciar `AUTO_INCREMENT` de tabelas de missões entre edições: os IDs devem ser monotônicos.

---

## 7. Projeto: Festival de Guildas

### Visão geral

Evento competitivo periódico em que as guildas recebem missões, acumulam pontos e disputam classificação durante uma edição de 7 dias.

- GM abre/fecha com `/festival open`, `/festival open [minutos]` e `/festival close` (permissão `GM_IMPLEMENTOR`).
- Ciclo padrão de 24h (1440 min, configurável). Até 5 missões por guilda por ciclo, renovadas ao fim do ciclo.
- Cada jogador aceita no máximo 2 missões por ciclo e só pode ter 1 missão `IN_PROGRESS` por vez.
- Missões concluídas, falhadas ou abandonadas continuam consumindo uma das 2 vagas no ciclo.
- Pontuação pertence à guilda; resgate dos baús é individual e não reduz a pontuação.

### Tipos de missão (`festival_mission_template`)

1 = Destruir Metins, 2 = PvP, 3 = Derrotar monstros, 4 = Coletar itens.
`target_vnum = 0` significa qualquer VNUM do tipo; diferente de zero exige alvo exato.
Progresso é individual, exceto coleta (drops legítimos de qualquer membro podem avançar missões compatíveis do recebedor).
Missões expiradas viram `FAILED`. Pontos são concedidos uma única vez.

### Baús

Marcos: 200 / 500 / 1.000 / 1.500 / 2.000 pontos. Campos: `total_points`, `reward_progress`, `pending_points`, `completed_missions`. Progresso limitado a 2.000 por sequência; pontuação total do evento sem limite. Os registros individuais de resgate são a fonte de verdade. `claimed_chests` em `festival_guild_state` deve ser removido quando o resgate individual estiver pronto. Valores de recompensa atuais são de teste.

### Arquitetura

- Arquivos: `festiv.h`, `festiv.cpp`, `input_main.cpp`, `packet_info.cpp`, `cmd_gm.cpp`, `main.cpp` (chama `Festival::ProcessCycle()` a cada 10 s no heartbeat), `char_battle.cpp`, `char_item.cpp`.
- Hooks: `Festival::OnMetinKilled`, `OnMonsterKilled`, `OnPlayerKilled`, `OnItemCollected`.
- Estados: `CLOSED`, `ACTIVE`, `PROCESSING`.
- **Decisão: evento único** (`FESTIVAL_EVENT_ID = 1`). Não criar gerenciador de múltiplas edições.
- Distribuição: `DistributeMissionsForAllGuilds(cycleID)` com `GET_LOCK`/`RELEASE_LOCK` para evitar distribuição duplicada.

### Tabelas (banco `srv1_player`)

`festival_event`, `festival_guild_state`, `festival_guild_mission`, `festival_mission_template`, `festival_player_state`, `festival_pvp_history`, `festival_config`, `festival_chests`, `festival_chest_rewards`, `festival_guild_history`.

`festival_guild_history`: `UNIQUE(edition_key, guild_id)`; `edition_key` = `started_at` da edição; todas as guildas de `festival_guild_state` são arquivadas, inclusive com 0 pontos.

### Encerramento (prioridade máxima)

Existem 3 caminhos em `festiv.cpp`: (1) manual `CloseEvent()`, (2) automático em `ACTIVE` quando `ends_at` expira, (3) automático em `PROCESSING` quando o prazo total termina durante a transição de ciclo.

Todos devem chamar uma função central `FinalizeFestivalEvent()`:

1. Iniciar transação SQL.
2. Bloquear o registro de `festival_event`.
3. Verificar os dados da edição.
4. Falhar as missões pendentes.
5. Registrar todas as guildas em `festival_guild_history` (idempotente).
6. Atualizar o evento para `CLOSED`.
7. Confirmar a transação (rollback em falha).

> `FinalizeFestivalEvent()` foi finalizado.

proxima etapa validar entrega das recompensas.

### Preservação de dados

| Tabela | No encerramento | Na nova edição |
|---|---|---|
| `festival_guild_history` | Preservar | Preservar |
| `festival_guild_state` | Preservar | Zerar pontos/progresso |
| `festival_guild_mission` | Preservar | Limpar registros antigos |
| `festival_pvp_history` | Preservar | Limpar registros antigos |
| `festival_player_state` | Preservar | Limpar registros antigos |

A limpeza só ocorre em `StartEvent()`, depois que a edição anterior estiver encerrada e arquivada. Não implementar o painel histórico agora.

### Interface com evento encerrado

Controlar pela informação explícita do servidor, sem apagar missões do banco. Estrutura **proposta** (conferir com o parser real do cliente antes de aplicar):

```cpp
enum EFestivalMissionListStatus
{
    FESTIVAL_LIST_ACTIVE = 0,
    FESTIVAL_LIST_UNAVAILABLE = 1
};

typedef struct SPacketGCFestivalMissionList
{
    BYTE status;
    WORD count;
} TPacketGCFestivalMissionList;
```

| Estado | Status enviado | Count | Interface |
|---|---|---|---|
| ACTIVE | ACTIVE | > 0 | Exibir missões |
| ACTIVE | ACTIVE | 0 | Lista vazia |
| PROCESSING | UNAVAILABLE | 0 | Mensagem indisponível |
| CLOSED | UNAVAILABLE | 0 | Mensagem indisponível |

Mensagem: "Festival de Guildas encerrado ou não está em execução." (complemento opcional: "Aguarde a próxima edição do evento."). Lista vazia **não** significa evento encerrado. No cliente: se `UNAVAILABLE`, limpar a lista antiga e mostrar a mensagem; se receber `ACTIVE`, remover a mensagem e mostrar as missões.

Conferir: `SendMissionPacket()`, `SendMission()`, `SendOpen()` em `festiv.cpp`; parser de `HEADER_GC_FESTIVAL` no cliente C++; função Python que cria os elementos visuais da lista.

### Estado e ordem de trabalho

Concluído: encerramento do evento e interface com mensagem de evento encerrado

Ordem de trabalho (não começar por funcionalidades novas):

1-Atualizar o banco para suportar sequências e resgates individuais em lote.(feito)
2-Implementar o cálculo de recompensas pendentes e o sorteio persistente.(feito)
3-Implementar entrega segura, inclusive quando o inventário estiver cheio.
4-Criar o protocolo cliente-servidor para solicitar o resgate em lote.
5-Atualizar o indicador numérico do baú superior e o estado visual após o resgate.
6-Finalizar o resgate dos baús.

## REGRA OBRIGATÓRIA — ENTREGA DE CÓDIGO EM TODA ETAPA DE DESENVOLVIMENTO

### 1. Princípio fundamental

Toda resposta que envolva desenvolvimento, correção, implementação ou evolução de sistemas da MartySama 5.8 deve produzir código concreto para ser inserido nos arquivos da source e submetido à compilação na VM FreeBSD.

**Pesquisar não é o resultado da tarefa. O resultado esperado é código implementado, acompanhado das instruções necessárias para compilar e testar.**

Não encerrar uma resposta apenas com:
- Análise da arquitetura.
- Explicação de como o sistema deveria funcionar.
- Lista de dependências a investigar.
- Proposta de implementação para uma próxima etapa.
- Promessa de que o código será desenvolvido posteriormente.

### 2. Pesquisa e implementação na mesma etapa

Antes de escrever código:
1. Consultar o repositório oficial MartySama 5.8.
2. Ler as implementações reais relacionadas ao problema.
3. Verificar declarações, definições, chamadas e dependências.
4. Identificar os mecanismos nativos que podem ser reutilizados.
5. Escolher a solução tecnicamente mais adequada.

Depois da pesquisa, implementar imediatamente o próximo conjunto funcional de alterações.

Não realizar pesquisas sucessivas sem transformar os resultados em código.

### 3. Entrega obrigatória de código

Toda resposta de desenvolvimento deve conter, sempre que tecnicamente possível:

- Caminho exato do arquivo.
- Nome da função, classe ou estrutura que será alterada.
- Ponto de inserção ou substituição.
- Código completo do trecho necessário, sem pseudocódigo.
- Includes, declarações e dependências necessárias.
- SQL separado, quando necessário.
- Alterações de protocolo nos dois lados, quando aplicável.
- Comandos de compilação compatíveis com FreeBSD 14.4.

Não utilizar expressões como "adicione a lógica aqui", "implemente o restante" ou "siga este exemplo" no lugar do código necessário.

Quando uma alteração depender de outra, entregar o conjunto coerente de arquivos e mudanças, não apenas uma função isolada que não possa ser integrada.

### 4. Proibição de pesquisa sem entrega

Cada etapa de investigação deve terminar com pelo menos um dos seguintes resultados:

**A. Implementação entregue:** código concreto, pronto para ser inserido na source e submetido à compilação.

**B. Bloqueio técnico comprovado:** informar exatamente qual dependência impede a implementação, apresentar as evidências encontradas na source e entregar todas as alterações independentes que já podem ser feitas com segurança.

O bloqueio deve ser específico e verificável. Não utilizar "precisamos pesquisar mais" como justificativa genérica para não entregar código.

A investigação deve continuar até eliminar o bloqueio, sem transformar a tarefa em uma sequência indefinida de análises.

### 5. Critério de compilação

O código deve ser escrito considerando a versão real da MartySama 5.8, e não exemplos genéricos de outras sources.

Distinguir obrigatoriamente:

- **Verificado na source:** mecanismo, função ou dependência encontrada no GitHub.
- **Código entregue:** alterações concretas propostas para os arquivos.
- **Compilação:** ainda não realizada ou resultado efetivamente obtido.
- **Testes:** ainda não realizados ou resultados efetivamente obtidos.

Nunca afirmar que o código compila sem executar a compilação.

O objetivo é entregar código com estrutura e dependências suficientes para que o usuário possa inseri-lo na VM e compilar. Se houver erro, corrigir com base nos logs apresentados.

### 6. Desenvolvimento incremental

Dividir sistemas grandes em etapas funcionais e compiláveis.

Cada etapa deve:
1. Ter um objetivo técnico delimitado.
2. Entregar código concreto.
3. Identificar todos os arquivos envolvidos.
4. Informar comandos de compilação.
5. Definir testes objetivos.
6. Registrar o que permanece pendente.

Não esperar a conclusão de todo o sistema para começar a entregar código.

### 7. Regra de encerramento da resposta

Antes de encerrar qualquer resposta de desenvolvimento, verificar:

- Entreguei código concreto?
- Identifiquei os arquivos e os pontos de alteração?
- Incluí as dependências necessárias?
- Informei como compilar na VM FreeBSD?
- Separei o que foi verificado do que ainda precisa ser testado?

Se não houver código, justificar com um bloqueio técnico específico e comprovado.

**Regra prioritária: o usuário não quer apenas aprender como implementar. Ele espera receber as alterações concretas para inserir na VM, compilar, testar e trazer os resultados para a próxima etapa.**