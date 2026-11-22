# Instruções do Projeto (cole no campo de instruções do Projeto do ChatGPT)

# Projeto: Desenvolvimento Metin2 – MartySama 5.8

## Source de referência
Repositório oficial de trabalho:
https://github.com/KimiHua-dv/MartySama-5.8-Source

## Memória do projeto
- No início de cada conversa, leia `MEMORIA_PROJETO.md` e `AGENTS.md` na raiz do repositório (ou o texto colado por mim). Eles têm as decisões firmes, o status de cada sistema e as pendências.
- Não reabra decisões marcadas como firmes sem um motivo técnico concreto.
- No final da tarefa, gere o bloco de atualização da memória no formato abaixo.

## Regras de desenvolvimento
- Consultar o GitHub antes de investigar ou modificar mecanismos existentes.
- Analisar a implementação real, incluindo declarações, definições e chamadas relacionadas.
- Não assumir compatibilidade com outras versões ou sources sem verificar.
- Priorizar mecanismos nativos já existentes.
- Evitar duplicação de entidades, sistemas e funcionalidades.
- Explicar a lógica das alterações e trabalhar incrementalmente.
- Não apresentar código como compatível com a MartySama sem conferir a source.
- Não pedir confirmação para decisões de implementação: escolha a opção mais coerente com o código existente, implemente e liste as suposições no final. Só pergunte se algo for realmente bloqueante.

## Ambiente
- Servidor: FreeBSD 14.4 em VirtualBox.
- Cliente: Windows 11.
- Compilação e testes realizados na VM.
- Todos os comandos de terminal devem ser compatíveis com FreeBSD (`/bin/sh`, sem bashisms, sem `sed -i` no estilo GNU).

## Metodologia
Pesquisar → compreender → planejar → implementar → compilar → testar.

O GitHub é a referência do código. A máquina virtual é a referência do funcionamento real.

---

# Prompt de fim de sessão (cole no chat quando terminar uma tarefa)

```
Encerrando a sessão. Gere o bloco de atualização da memória do projeto, curto e factual, neste formato:

### AAAA-MM-DD — <sistema>
- Feito: <o que foi implementado/alterado, com arquivos e funções>
- Confirmado na source: <o que foi verificado no código>
- Proposta (não verificada/compilada): <o que ainda é só sugestão>
- Decisões: <decisões tomadas e o motivo>
- Compilou/testou na VM: <sim/não/parcial e o que falta>
- Próximo passo: <1 a 3 itens>

Depois, liste só as linhas das seções 1 a 4 do MEMORIA_PROJETO.md que mudaram (decisões firmes, status por sistema, pendências, armadilhas), em formato "substituir X por Y".
Não repita o que já está no arquivo e não invente nada que não foi feito nesta conversa.
```

