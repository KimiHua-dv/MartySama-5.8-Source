AGENTS.md — Metin2 MartySama 5.8

Instruções para agentes de código (Codex, chat de Projeto, etc.) neste repositório. Leia este arquivo inteiro antes de qualquer tarefa.

REGRA PRIORITÁRIA — TODA RESPOSTA DE DESENVOLVIMENTO ENTREGA CÓDIGO

O usuário não quer apenas aprender como implementar. Ele quer receber as alterações concretas para inserir na VM, compilar, testar e trazer os resultados para a próxima etapa.

Nenhuma resposta de desenvolvimento pode terminar somente com pesquisa, análise, explicação ou planejamento. Pesquisar não é o resultado da tarefa: o resultado é código implementado, com instruções para compilar e testar.

Proibido encerrar uma resposta com
Análise da arquitetura ou explicação de como o sistema "deveria" funcionar.
Lista de dependências a investigar.
Proposta para a próxima etapa ou promessa de código posterior.
Pergunta de confirmação sobre algo que você pode decidir sozinho.
"Vou pesquisar mais", "preciso ver o arquivo X antes" ou equivalentes, quando você pode ler o arquivo agora.
Pseudocódigo, ou frases como "adicione a lógica aqui", "implemente o restante", "siga este exemplo".
Fluxo obrigatório em cada etapa (tudo na mesma resposta)
Pesquisar uma vez, com foco: ler no repositório as implementações reais relacionadas, declarações, definições, chamadas e dependências, e identificar os mecanismos nativos reutilizáveis.
Decidir: escolher a solução mais coerente com o código existente. Não apresentar alternativas para o usuário escolher.
Implementar imediatamente o próximo conjunto funcional e compilável.
Entregar o código e as instruções de compilação e teste.

Não fazer pesquisas sucessivas sem transformá-las em código.

Todo o resultado deve ser A ou B

A. Implementação entregue: código concreto, pronto para inserir na source e compilar.

B. Bloqueio técnico comprovado: informar exatamente qual dependência impede a implementação, mostrar as evidências encontradas na source (arquivo, função, linha) e entregar mesmo assim todas as alterações independentes que já podem ser feitas com segurança. O bloqueio deve ser específico e verificável. "Precisamos pesquisar mais" não é bloqueio.

Se faltar uma informação não crítica (nome de arquivo do cliente, valor de constante, etc.), implemente com a melhor suposição verificável, marque como SUPOSIÇÃO no final e siga em frente. Só pergunte quando duas opções forem incompatíveis, mudarem banco ou protocolo de rede e não houver como inferir. Nesse caso, faça uma pergunta objetiva e entregue no mesmo turno tudo o que não depende da resposta.

O que a entrega deve conter (sempre que tecnicamente possível)
Caminho exato de cada arquivo.
Nome da função, classe ou estrutura alterada.
Ponto de inserção ou substituição (função vizinha, linha âncora ou trecho antigo a ser trocado).
Código completo do trecho, sem pseudocódigo: arquivo inteiro se for pequeno; para arquivos grandes, trecho completo da função alterada ou diff unificado, nunca resumos.
Includes, declarações (.h) e dependências necessárias.
SQL em bloco separado, quando houver mudança de schema.
Alterações de protocolo nos dois lados (servidor e cliente) quando aplicável.
Comandos de compilação compatíveis com FreeBSD 14.4.
Testes objetivos, com o que observar no log ou no jogo.

Se uma alteração depender de outra, entregue o conjunto coerente de arquivos e mudanças, não uma função isolada que não possa ser integrada. Se a resposta ficar longa, divida em partes numeradas na ordem de aplicação e continue na mensagem seguinte sem cortar código no meio.

Honestidade sobre o estado do código

Distinguir sempre:

Verificado na source: mecanismo, função ou dependência efetivamente encontrada no GitHub.
Código entregue: alterações concretas propostas para os arquivos.
Compilação: ainda não realizada ou resultado efetivamente obtido.
Testes: ainda não realizados ou resultados efetivamente obtidos.

Nunca afirmar que o código compila ou funciona sem ter executado. O usuário compila na VM e traz os logs; se houver erro, corrija com base neles.

Checklist antes de enviar a resposta
Entreguei código concreto (e não só texto)?
Identifiquei os arquivos e os pontos de alteração?
Incluí includes, declarações e dependências?
Alterei servidor e cliente juntos, se houve mudança de protocolo?
Informei como compilar na VM FreeBSD?
Separei o que foi verificado do que ainda precisa ser testado?

Se alguma resposta for "não", corrija antes de enviar. Se não houver código, justifique com um bloqueio técnico específico e comprovado.

0. Memória do projeto
No início de cada tarefa, leia MEMORIA_PROJETO.md (decisões firmes, status e pendências).
No final, atualize esse arquivo: adicione uma entrada no "Diário de sessões" e ajuste as seções 1 a 4 se algo mudou. Registre só o que foi realmente feito e verificado.
Todos os comandos de terminal que você sugerir devem ser compatíveis com FreeBSD (/bin/sh, sem bashisms).
1. Como trabalhar
Priorize mecanismos nativos da MartySama (DBManager, CAsyncSQL, sistema de comandos GM, packet_info etc.) em vez de soluções externas.
Evite duplicar entidades, sistemas e funcionalidades que já existem.
Não presuma compatibilidade com outras sources de Metin2 nem com outras versões. Não apresente código como compatível com a MartySama sem conferir a source.
Não faça refatorações fora do escopo da tarefa. Mudanças pequenas e focadas.
Trabalhe de forma incremental: cada etapa tem objetivo técnico delimitado, entrega código, lista os arquivos envolvidos, informa como compilar, define testes objetivos e registra o que permanece pendente. Não espere o sistema inteiro ficar pronto para começar a entregar código.
2. Formato da resposta final

Código primeiro, explicação depois. Termine sempre com:

O que foi feito (arquivos e funções alterados).
Código (conforme a seção "O que a entrega deve conter").
Suposições adotadas (decisões tomadas sem perguntar).
Verificado na source vs. proposta.
Como compilar e testar.
Pendências (o que ficou de fora e por quê).

O dono do projeto quer entender a lógica: explique de forma técnica e curta o porquê das alterações importantes, sem substituir o código por explicação.

3. Ambiente
Servidor: FreeBSD 14.4 (VirtualBox). Compilação e testes acontecem na VM.
Cliente: Windows 11 (C++ + interface em Python).
Banco: MariaDB, banco srv1_player para as tabelas dos sistemas customizados.
GitHub é a referência do código. A VM é a referência do funcionamento real.
Se não for possível compilar no seu ambiente, diga isso e entregue o comando para o usuário rodar na VM.
4. Estrutura relevante (ajustar caminhos reais)
game/src/ — servidor game (ex.: main.cpp, input_main.cpp, packet_info.cpp, cmd_gm.cpp, char_battle.cpp, char_item.cpp, db.cpp, festiv.cpp/.h)
libsql/ — AsyncSQL.cpp (conexões e DirectQuery)
Cliente: parser de pacotes em C++ e módulos de interface em Python (confirmar os caminhos reais no repositório antes de editar).
5. Regras de protocolo de rede
Qualquer mudança em struct de pacote deve ser feita no servidor e no cliente ao mesmo tempo: mesmo tamanho, mesma ordem de campos, mesmo header.
Ao alterar ou criar pacote, atualizar packet_info.cpp (header e tamanho) e o parser do cliente.
Antes de alterar uma struct, localize no cliente o parser real e confira a serialização.
6. Regras de banco de dados
Mudanças de schema vêm como script SQL separado (sql/ ou pasta equivalente), nunca só no código C++.
Operações que alteram vários registros devem ser feitas em transação. DBManager::DirectQuery() usa a conexão persistente m_hDB, então comandos consecutivos usam a mesma conexão (confirmado na source).
Operações de arquivamento/histórico devem ser idempotentes (UNIQUE + ON DUPLICATE KEY UPDATE).
Não reiniciar AUTO_INCREMENT de tabelas de missões entre edições: os IDs devem ser monotônicos.
7. Projeto: Festival de Guildas
Visão geral

Evento competitivo periódico em que as guildas recebem missões, acumulam pontos e disputam classificação durante uma edição de 7 dias.

GM abre/fecha com /festival open, /festival open [minutos] e /festival close (permissão GM_IMPLEMENTOR).
Ciclo padrão de 24h (1440 min, configurável). Até 5 missões por guilda por ciclo, renovadas ao fim do ciclo.
Cada jogador aceita no máximo 2 missões por ciclo e só pode ter 1 missão IN_PROGRESS por vez.
Missões concluídas, falhadas ou abandonadas continuam consumindo uma das 2 vagas no ciclo.
Pontuação pertence à guilda; resgate dos baús é individual e não reduz a pontuação.
Tipos de missão (festival_mission_template)

1 = Destruir Metins, 2 = PvP, 3 = Derrotar monstros, 4 = Coletar itens. target_vnum = 0 significa qualquer VNUM do tipo; diferente de zero exige alvo exato. Progresso é individual, exceto coleta (drops legítimos de qualquer membro podem avançar missões compatíveis do recebedor). Missões expiradas viram FAILED. Pontos são concedidos uma única vez.

Baús

Marcos: 200 / 500 / 1.000 / 1.500 / 2.000 pontos. Campos: total_points, reward_progress, pending_points, completed_missions. Progresso limitado a 2.000 por sequência; pontuação total do evento sem limite. Os registros individuais de resgate são a fonte de verdade. claimed_chests em festival_guild_state deve ser removido quando o resgate individual estiver pronto. Valores de recompensa atuais são de teste.

Arquitetura
Arquivos: festiv.h, festiv.cpp, input_main.cpp, packet_info.cpp, cmd_gm.cpp, main.cpp (chama Festival::ProcessCycle() a cada 10 s no heartbeat), char_battle.cpp, char_item.cpp.
Hooks: Festival::OnMetinKilled, OnMonsterKilled, OnPlayerKilled, OnItemCollected.
Estados: CLOSED, ACTIVE, PROCESSING.
Decisão: evento único (FESTIVAL_EVENT_ID = 1). Não criar gerenciador de múltiplas edições.
Distribuição: DistributeMissionsForAllGuilds(cycleID) com GET_LOCK/RELEASE_LOCK para evitar distribuição duplicada.
Tabelas (banco srv1_player)

festival_event, festival_guild_state, festival_guild_mission, festival_mission_template, festival_player_state, festival_pvp_history, festival_config, festival_chests, festival_chest_rewards, festival_guild_history.

festival_guild_history: UNIQUE(edition_key, guild_id); edition_key = started_at da edição; todas as guildas de festival_guild_state são arquivadas, inclusive com 0 pontos.

Encerramento (concluído)

Os 3 caminhos em festiv.cpp (manual CloseEvent(), automático em ACTIVE quando ends_at expira, automático em PROCESSING quando o prazo total termina) chamam a função central FinalizeFestivalEvent(), que: inicia transação, bloqueia o registro de festival_event, falha as missões pendentes, registra todas as guildas em festival_guild_history (idempotente), marca o evento como CLOSED e confirma (rollback em falha). Está finalizada; não reescrever sem motivo técnico.

Preservação de dados
Tabela	No encerramento	Na nova edição
festival_guild_history	Preservar	Preservar
festival_guild_state	Preservar	Zerar pontos/progresso
festival_guild_mission	Preservar	Limpar registros antigos
festival_pvp_history	Preservar	Limpar registros antigos
festival_player_state	Preservar	Limpar registros antigos

A limpeza só ocorre em StartEvent(), depois que a edição anterior estiver encerrada e arquivada. Não implementar o painel histórico agora.

Interface com evento encerrado (concluído)

Controlada pela informação explícita do servidor, sem apagar missões do banco: status (FESTIVAL_LIST_ACTIVE = 0, FESTIVAL_LIST_UNAVAILABLE = 1) + count. PROCESSING e CLOSED enviam UNAVAILABLE com count 0 e o cliente mostra "Festival de Guildas encerrado ou não está em execução." Lista vazia com ACTIVE não significa evento encerrado. Consulte SendMissionPacket(), SendMission(), SendOpen() e o parser de HEADER_GC_FESTIVAL no cliente antes de mexer no protocolo.

Estado e ordem de trabalho (prioridade máxima)

Concluído — núcleo do sistema:

Distribuição de missões, hooks, progresso individual, expiração e pontuação coletiva.

Controle de até 2 missões aceitas por jogador em cada ciclo e limite de 1 missão IN_PROGRESS simultânea.

Ciclos, renovação de missões e comando GM.

Histórico de edições em festival_guild_history.

Encerramento manual testado.

Sistema de claims individuais dos baús, preparação de recompensas, sorteio persistente e entrega idempotente.

Protocolo e interface dos baús: cinco marcos, contadores individuais de recompensas pendentes e atualização após resgate.

Interface central do Festival: painel de missões, recompensas, barra de pontos, progresso dos marcos, indicador de missões por membro e tooltips dos baús.

Barra de progresso compartilhada da guilda, calculada a partir dos pontos acumulados, independente dos resgates individuais.

Interface central concluída. Não reabrir essa etapa sem identificar um problema funcional concreto.

Próximas prioridades de desenvolvimento:

-Informações detalhadas das missões aceitas:
Exibir o progresso atual e o tempo restante em cada missão aceita.(concluido)
Quando a missão estiver ocupada por outro jogador, substituir o texto genérico "Em curso" pelo nome do jogador responsável.
Para o próprio jogador, apresentar seu progresso e tempo restante.(concluido)
Manter a validação e a autoridade do estado no servidor.(concluido)
-Painel Informações(prioridade):
Exibir a contribuição individual do jogador.
Exibir sua posição na guilda.
Exibir o total de renovações.
Exibir o tempo restante até a próxima renovação automática.
Atualizar os dados conforme o estado real do servidor.

-Renovação de Missoes:
somente lider da guilda é autorizado
custo: 300.000 gold retirado do banco da guilda.
limite de 2 renovaçoes por ciclo.

Pendências posteriores à interface:
Revisar FinalizeFestivalEvent() e confirmar sua implementação.(concluido)
Integrar e validar os três caminhos de encerramento: manual, automático por ends_at e recuperação durante PROCESSING.(concluido)
Revisar StartEvent() e confirmar que a limpeza só ocorre depois do encerramento e arquivamento da edição anterior.(concluido)
Remover resets desnecessários de cycle_id e de AUTO_INCREMENT.(concluido)
Validar o protocolo de disponibilidade da lista de missões (ACTIVE / UNAVAILABLE) em todos os estados do evento.(concluido)
Testar o ciclo completo de encerramento, histórico e abertura de nova edição.(concluido)


Ao receber este arquivo, comece pelo item 3 e entregue o código completo (servidor, SQL se houver, comandos de compilação e testes), sem etapa prévia de análise.