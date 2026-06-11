# Sistema de Manutenção de Contas Bancárias em C

Atividade acadêmica desenvolvida em linguagem C com base nos conceitos de subprogramas, arquivos binários e recursão.

## O que foi utilizado

- `struct` com `typedef` para representar os dados do cliente
- Funções separadas por responsabilidade, com protótipos declarados antes do `main`
- Arquivo binário (`contas.dat`) com registros de tamanho fixo
- `fopen`, `fclose`, `fread`, `fwrite` para leitura e escrita de registros
- `fseek` para acesso direto a uma posição específica do arquivo
- `rewind` para releitura do arquivo desde o início
- `ferror` e `feof` para tratamento de erros e fim de arquivo
- Função recursiva para contagem de clientes ativos
- Guard `#ifndef` para proteção de definições
