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
- Código: a source da VM FreeBSD 14.4 está alinhada com o repositório GitHub MartySama-5.8-Source. As análises já realizadas no GitHub podem ser utilizadas como referência para continuar o desenvolvimento, sem solicitar novamente os mesmos arquivos. Se houver divergência ou atualização posterior, verificar o commit antes de prosseguir.
- Fluxo de desenvolvimento no ChatGPT: trabalhar normalmente no chat, sem exigir Work mode nem solicitar novamente arquivos já analisados. Consultar o GitHub diretamente para novas verificações e reutilizar os levantamentos anteriores enquanto forem compatíveis com a source. Entregar alterações como código sugerido, indicando arquivo, função, ponto de inserção, trecho a substituir e dependências. A aplicação, compilação e testes são realizados pelo usuário na VM FreeBSD; nunca afirmar que foram executados sem confirmação.
- As pesquisas serão feitas todas de uma só vez e seguida do codigo sugerido para compilação e um novo teste, sempre que houver duvida sugira a mudança caso ela ja tenha aconteciso antes eu não a farei.

## 2. Status por sistema

### Festival de Guildas

Em andamento: ## 3. Pendências globais

### Sistema de reinos
- (preencher status)

### Fila de Metins
- (preencher status)

## 3. Pendências globais (ordem de prioridade)
1-Atualizar o banco para suportar sequências e resgates individuais em lote.(feito)
2-Implementar o cálculo de recompensas pendentes e o sorteio persistente.(feito)
3-Implementar entrega segura, inclusive quando o inventário estiver cheio.
4-Criar o protocolo cliente-servidor para solicitar o resgate em lote.
5-Atualizar o indicador numérico do baú superior e o estado visual após o resgate.
6-Finalizar o resgate dos baús.

## 4. Problemas conhecidos / armadilhas

- Struct de pacote deve ser idêntica no servidor e no cliente (tamanho e ordem).
- (adicionar o que já deu errado e a causa)
- Não solicitar novamente arquivos já analisados no GitHub quando a VM estiver alinhada com o repositório. Reutilizar as verificações anteriores, conferindo o commit quando houver indício de divergência.
- No chat normal, não interromper a implementação para solicitar arquivos que podem ser consultados no GitHub. Se a implementação não puder ser confirmada na source, informar a limitação e pedir somente o trecho indispensável.


## Diário de sessões

### 2026-11-22 — Metodologia e encerramento do Festival
- Feito: definida a regra de continuidade pelo chat normal, com consulta direta ao GitHub e entrega de código com localização precisa.
- Confirmado na source: `festiv.cpp` já contém uma versão global de `FinalizeFestivalEvent()`; os três caminhos de encerramento estão integrados; `StartEvent()` já não reseta AUTO_INCREMENT na branch consultada.
- Proposta (não verificada/compilada): reforçar validação de guildas, conferência do arquivamento e preservação de datas em `FinalizeFestivalEvent()`.
- Decisões: não exigir Work mode nem reenvio de arquivos para continuar a análise; apresentar alterações sugeridas com instruções claras.
- Compilou/testou na VM: sim.
- Próximo passo: 