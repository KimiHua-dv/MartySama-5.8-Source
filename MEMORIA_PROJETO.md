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

- **Concluído:** distribuição de missões, hooks (Metin, monstro, PvP e coleta), progresso individual, expiração, pontuação coletiva, comando GM e `ProcessCycle()`.
- **Concluído:** histórico de edições, encerramento manual testado e sistema de claims individuais dos baús com entrega idempotente.
- **Concluído:** interface central — lista de missões, recompensas, cinco baús, contadores individuais, barra de pontos compartilhada, indicador de missões aceitas por membro e tooltips.
- **Concluído:**  ciclo completo de encerramento do evento, incluindo `FinalizeFestivalEvent()`, integração dos três caminhos e validação de `StartEvent()`.
- **Concluído:**  progresso e tempo restante das missões aceitas; exibir o nome do jogador responsável quando outro membro aceitar uma missão.
- **Concluído:** adicionar atualização automatica de progresso para cada missão.
- **Concluído:** painel Informações com contribuição individual, posição na guilda, total de renovações e contagem regressiva para a próxima renovação automática.
- **Concluído:** implementar renovação de missoes pelo lider da guilda custando 300.000 gold da guilda.
- **concluido:** validar protocolo de disponibilidade da lista (`FESTIVAL_LIST_ACTIVE/UNAVAILABLE`) e testar o ciclo completo entre encerramento, histórico e nova edição.
-**Prioridade:** informaçoes sobre o evento na parte superior do painel como: Nome do evento, frase abaixo do nome: "conclua missões,somem pontos e leve sua guilda a vitoria.Trabalho em equipe faz a diferença", tempo restante para fim do evento com timer.
- **Futuro:** painel histórico de edições.

### Sistema de reinos
- (preencher status)

### Fila de Metins
- (preencher status)

## 3. Pendências globais (ordem de prioridade)
1. Festival — exibir progresso e tempo restante das missões aceitas e identificar o jogador responsável pelas missões ocupadas.
2. Festival — implementar o painel Informações: contribuição, posição na guilda, renovações e próxima renovação automática.


## 4. Problemas conhecidos / armadilhas

- Struct de pacote deve ser idêntica no servidor e no cliente (tamanho e ordem).
- (adicionar o que já deu errado e a causa)
- Não solicitar novamente arquivos já analisados no GitHub quando a VM estiver alinhada com o repositório. Reutilizar as verificações anteriores, conferindo o commit quando houver indício de divergência.
- No chat normal, não interromper a implementação para solicitar arquivos que podem ser consultados no GitHub. Se a implementação não puder ser confirmada na source, informar a limitação e pedir somente o trecho indispensável.
- A interface central do Festival está concluída. Não duplicar mecanismos de atualização já existentes.
- A barra de pontos é compartilhada pela guilda e deriva da pontuação acumulada; não deve depender do resgate individual dos baús.
- Os contadores dos baús são individuais por jogador. Não utilizar o estado de resgate de um membro para alterar o estado dos demais.
- O limite de 2 missões aceitas por jogador por ciclo é validado pelo servidor. A interface deve apenas apresentar o estado recebido, sem implementar uma segunda validação local.
- Progresso e tempo restante das missões devem refletir dados autoritativos do servidor; não tratar contadores locais como fonte da verdade.
- O nome exibido para uma missão ocupada deve identificar o jogador que efetivamente a aceitou, não o líder da guilda nem o último jogador que abriu o painel.

## Diário de sessões

### 2026-11-22 — Metodologia e encerramento do Festival
- Feito: definida a regra de continuidade pelo chat normal, com consulta direta ao GitHub e entrega de código com localização precisa.
- Confirmado na source: `festiv.cpp` já contém uma versão global de `FinalizeFestivalEvent()`; os três caminhos de encerramento estão integrados; `StartEvent()` já não reseta AUTO_INCREMENT na branch consultada.
- Proposta (não verificada/compilada): reforçar validação de guildas, conferência do arquivamento e preservação de datas em `FinalizeFestivalEvent()`.
- Decisões: não exigir Work mode nem reenvio de arquivos para continuar a análise; apresentar alterações sugeridas com instruções claras.
- Compilou/testou na VM: sim.
- Próximo passo: 

### 2026-11-25 — Festival de Guildas: interface central

- Feito: conclusão da interface central do Festival, incluindo lista de missões, painel de recompensas, cinco baús com contadores individuais, tooltips, barra de pontos compartilhada e indicador de missões aceitas por membro.
- Confirmado na source: mecanismos de estado do Festival, pontuação coletiva, claims individuais e limite de duas missões aceitas por jogador por ciclo.
- Proposta (não verificada/compilada): progresso e tempo restante das missões aceitas; identificação do jogador responsável por missões ocupadas; painel Informações.
- Decisões: barra de progresso compartilhada, calculada pelos pontos da guilda; resgates e contadores dos baús continuam individuais.
- Compilou/testou na VM: interface central testada visualmente pelo usuário; novas funcionalidades ainda não implementadas.
- Próximo passo: 1. Implementar detalhes das missões aceitas. 2. Implementar o painel Informações.

### 26/11/2026 — Festival de Guildas
- Feito: cronômetro local nos cartões de missão, com OnUpdate() e app.GetTime(), em uiFestival.py.
- Confirmado na source: padrão de contagem regressiva em root/uiautoban.py e root/uicommon.py.
- Proposta (não verificada/compilada): atualização incremental do progresso das missões via servidor.
- Decisões: cronômetro visual local; servidor continua sendo a autoridade sobre expiração.
- Compilou/testou na VM: não aplicável ao Python do cliente; sintaxe validada, teste no cliente pendente.
- Próximo passo: testar a contagem regressiva no cliente e depois implementar atualização automática do progresso.

### Atualização da memória — 2026-11-26
- Feito: corrigido o envio da atualização de progresso após o COMMIT em OnMetinKilled() e ajustada a consulta de SendMissionProgress() para distinguir mission.id de mission.template_id.
- Confirmado na source: o cliente já possuía o caminho de recebimento e atualização visual.
- Proposta (não verificada/compilada): nenhuma alteração adicional nesta etapa.
- Decisões: manter a atualização incremental, sem reconstruir toda a janela.
- Compilou/testou na VM: teste funcional confirmado pelo usuário; compilação na VM.
- confimado: conclusão da missão, atribuição de pontos e atualização do estado dos baús.

###2026-11-27 — Festival de Guildas: revisão das renovações
- Feito: identificados dois erros na implementação de renovação: ausência de reset de manual_renewals em ClearEvent() e vírgula ausente no SQL de ProcessCycle().
- Confirmado na source: RenewGuildMissions() não modifica next_renewal_at; esse campo representa o fim do ciclo automático.
- Proposta (não verificada/compilada): incluir reset de manual_renewals, corrigir SQL e atualizar o schema de referência.
- Decisões: renovação manual não altera o prazo automático de 120 minutos.
- Compilou/testou na VM: não.
- Próximo passo: aplicar as correções, compilar e testar abertura de edição, duas renovações manuais e transição automática.