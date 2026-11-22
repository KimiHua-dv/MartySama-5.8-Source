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
- Não solicitar novamente arquivos já analisados no GitHub quando a VM estiver alinhada com o repositório. Reutilizar as verificações anteriores, conferindo o commit quando houver indício de divergência.
- No chat normal, não interromper a implementação para solicitar arquivos que podem ser consultados no GitHub. Se a implementação não puder ser confirmada na source, informar a limitação e pedir somente o trecho indispensável.


### 2026-11-22 — Metodologia do projeto
- Feito: definida a equivalência entre o código da VM FreeBSD 14.4 e o repositório GitHub como referência operacional para continuidade das análises.
- Confirmado na source: mantidos como referência os levantamentos anteriores do repositório MartySama 5.8.
- Proposta (não verificada/compilada): nenhuma alteração de código.
- Decisões: reutilizar análises anteriores sem solicitar novamente os mesmos arquivos, verificando o commit em caso de possível divergência.
- Compilou/testou na VM: não se aplica.
- Próximo passo: continuar a revisão de `FinalizeFestivalEvent()` e dos três caminhos de encerramento.

---

## Diário de sessões

### 2026-11-22 — Metodologia e encerramento do Festival
- Feito: definida a regra de continuidade pelo chat normal, com consulta direta ao GitHub e entrega de código com localização precisa.
- Confirmado na source: `festiv.cpp` já contém uma versão global de `FinalizeFestivalEvent()`; os três caminhos de encerramento estão integrados; `StartEvent()` já não reseta AUTO_INCREMENT na branch consultada.
- Proposta (não verificada/compilada): reforçar validação de guildas, conferência do arquivamento e preservação de datas em `FinalizeFestivalEvent()`.
- Decisões: não exigir Work mode nem reenvio de arquivos para continuar a análise; apresentar alterações sugeridas com instruções claras.
- Compilou/testou na VM: sim.
- Próximo passo: 
Encontrei dois pontos importantes na implementação atual:
1. StartEvent() não é atômico. Ela marca o evento como CLOSED, atualiza as datas e o ciclo e, em seguida, executa várias operações de limpeza e distribuição separadamente. Se uma delas falhar, a função retorna false, mas as operações anteriores já podem ter sido gravadas.
2. O protocolo atual não consegue representar uma lista indisponível. TPacketGCFestivalMissionList contém apenas WORD count. Além disso, SendMissionPacket() retorna sem enviar nada quando a lista está vazia. Portanto, o cliente não recebe uma informação explícita de encerramento.
Vou tratar as duas mudanças em conjunto, mas sem misturar suas responsabilidades: a abertura precisa ser consistente no banco; a interface precisa receber um estado explícito do servidor. A mensagem será exibida quando o estado do evento for CLOSED ou PROCESSING, e não simplesmente quando a lista tiver zero missões.
1. Abertura de uma nova edição — StartEvent()
A implementação atual confirma que a preparação ocorre em várias consultas independentes:
- Atualiza festival_event para CLOSED, já alterando started_at e current_cycle.
- Apaga festival_pvp_history.
- Apaga festival_player_state.
- Zera a pontuação em festival_guild_state.
- Apaga festival_guild_mission.
- Distribui as missões do ciclo 1.
- Só então altera o status para ACTIVE.
A correção será colocar essa sequência em uma única transação, com o registro de festival_event bloqueado por SELECT ... FOR UPDATE.

A sequência segura será:
1. Iniciar transação.
2. Bloquear o registro do evento.
3. Confirmar que o evento está CLOSED.
4. Se já existe uma edição anterior, conferir se todas as guildas foram arquivadas em festival_guild_history.
5. Atualizar as datas e preparar o ciclo 1.
6. Limpar os dados operacionais da edição anterior.
7. Distribuir as missões do ciclo 1.
8. Alterar o status para ACTIVE.
9. Executar COMMIT.
10. Somente após o commit, ativar festival_guild.
Se qualquer etapa falhar, executar ROLLBACK.

Um detalhe importante: DistributeMissionsForAllGuilds() já utiliza GET_LOCK() e RELEASE_LOCK(). Não vamos criar outro mecanismo de distribuição. Precisamos preservar esse mecanismo e confirmar que as consultas transacionais permanecem na mesma conexão durante toda a execução.

Também precisamos confirmar que as tabelas envolvidas utilizam InnoDB. Sem isso, a transação não garante a atomicidade das operações.
2. Protocolo para informar o encerramento
A estrutura atual, tanto no servidor quanto no cliente, é:
typedef struct SPacketGCFestivalMissionList
{
    WORD count;
} TPacketGCFestivalMissionList;


Minha proposta é substituir por:
enum EFestivalMissionListStatus
{
    FESTIVAL_LIST_ACTIVE = 0,
    FESTIVAL_LIST_UNAVAILABLE = 1
};

#pragma pack(push, 1)
typedef struct SPacketGCFestivalMissionList
{
    BYTE status;
    WORD count;
} TPacketGCFestivalMissionList;
#pragma pack(pop)


O tamanho do payload será de 3 bytes. O #pragma pack é importante para garantir que servidor e cliente utilizem exatamente o mesmo layout.

Arquivos que precisam receber a alteração:
Arquivo	Alteração
Server/game/src/festiv.h	Enum e estrutura do pacote
Client/UserInterface/Festiv.h	Mesma enum e estrutura
Server/game/src/festiv.cpp	Envio do status e da lista
Client/UserInterface/PythonNetworkStreamPhaseGame.cpp	Leitura do novo payload
root/uifestival.py	Exibição da mensagem e controle da lista

packet_info.cpp não precisa de alteração no tamanho do pacote principal: ele já registra HEADER_GC_FESTIVAL com sizeof(TPacketGCFestival), e o pacote é dinâmico.
3. Comportamento esperado
Estado no banco	Status enviado	Count	Cliente
ACTIVE	ACTIVE	Maior que zero	Exibe missões
ACTIVE	ACTIVE	Zero	Lista vazia
PROCESSING	UNAVAILABLE	Zero	Evento indisponível
CLOSED	UNAVAILABLE	Zero	Evento finalizado

No cliente, o fluxo será:
- Recebe UNAVAILABLE: limpa as missões antigas e exibe "Evento finalizado".
- Recebe ACTIVE: remove a mensagem e atualiza a lista.
- Recebe ACTIVE com zero missões: mantém a lista vazia, sem afirmar que o evento terminou.
Isso evita um erro importante da implementação atual: SendMissionPacket() simplesmente retorna quando missions.empty(), deixando o cliente sem saber que a lista antiga não é mais válida.
4. Um ponto que ainda precisa ser conferido
O arquivo root/uifestival.py contém a classe Festival, com ClearFestivalMissions(), AppendFestivalMission() e RefreshMissionList(). Porém, a busca na source não encontrou uma implementação Python correspondente ao callback OpenFestval chamado pelo C++.

Não vou presumir que esse callback está corretamente conectado à janela. Precisamos conferir a ponte entre PHASE_WINDOW_GAME e uifestival.Festival antes de definir onde chamar a nova função de exibição.

Estado desta etapa: auditoria da implementação atual realizada; nenhuma alteração foi aplicada ou compilada nesta etapa.

O próximo passo é fechar a implementação do protocolo e a integração Python, incluindo a verificação da ponte da interface. Depois disso, entrego a substituição completa de StartEvent() e o protocolo, com os pontos exatos de inserção e os testes para executar na VM.
