# Tarefa: Roteiro de FreeRTOS #1 - EmbarcaTech 2025

Autor: **Arthur Damacena Silva**

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Brasília, 12 de junho de 2025

---

## 🎯 Objetivo do projeto

Desenvolver um sistema multitarefa embarcado utilizando a placa BitDogLab (baseada no RP2040) e o sistema operacional de tempo real FreeRTOS. O sistema deve controlar três periféricos de forma concorrente: um LED RGB, um buzzer e dois botões para interação do usuário, suspendendo e retomando as tarefas dos periféricos.

## 🔧 Componentes usados

*   Placa de desenvolvimento BitDogLab (Raspberry Pi Pico RP2040)
*   LED RGB
*   Buzzer 
*   2x Botões

## 💾 Como compilar e executar o código

1.  **Clonar o Repositório:**
    *   Abra um terminal ou prompt de comando.
    *   Clone o repositório utilizando o comando:
        ```bash
        git clone --recurse-submodules https://github.com/EmbarcaTech-2025/tarefa-freertos-1-R2Damacena.git
        ```
    *   A flag `--recurse-submodules` garante que o FreeRTOS (que é um submódulo) seja clonado junto.
    *   Se você já clonou o repositório sem `--recurse-submodules`, navegue até a pasta do projeto e execute:
        ```bash
        git submodule update --init --recursive
        ```

2.  **Pré-requisitos:**
    *   VS Code com a extensão para o Raspberry Pi Pico.
    *   SDK do Raspberry Pi Pico instalado.
    *   Toolchain ARM GCC.
    *   (O FreeRTOS-Kernel já deve ter sido baixado com o passo anterior se o submódulo estiver configurado corretamente no repositório).

3.  **Compilar:**
    *   No VS Code, use o comando de build fornecido pela extensão.
    *   Isso gerará um arquivo `.uf2` no diretório `build` (ou similar, dependendo da configuração da extensão).

5.  **Executar (Carregar na Placa):**
    *   Coloque o Raspberry Pi Pico em modo BOOTSEL (pressione e segure o botão BOOTSEL enquanto conecta o cabo USB ao computador).
    *   O Pico aparecerá como um dispositivo de armazenamento em massa no seu computador.

## ⚡ Pinagem dos dispositivos utilizados

*   **LED RGB:**
    *   Componente Vermelho (R): GPIO 13
    *   Componente Verde (G): GPIO 11
    *   Componente Azul (B): GPIO 12
*   **Buzzer:**
    *   Pino de Controle: GPIO 21
*   **Botões:**
    *   Botão A (Controle LED): GPIO 5
    *   Botão B (Controle Buzzer): GPIO 6

## 🖼️ Vídeo e imagem do projeto em funcionamento

https://github.com/user-attachments/assets/a9f709e6-5e79-48f4-8a21-646cab4b11cc

![Monitor serial exibindo o status de botões sendo pressionados e soltos.](https://github.com/user-attachments/assets/92ed8e58-6d63-415f-ae75-d4021f169b2b)

## 📈 Resultados

**Resultados obtidos:**

*   O LED RGB alterna ciclicamente entre as cores vermelho, verde e azul, com cada cor permanecendo acesa por 500ms.
*   O buzzer emite um bipe curto a cada 1 segundo.
*   Ao pressionar o Botão A, a tarefa do LED RGB é suspensa (o LED apaga). Ao soltar o Botão A, a tarefa do LED RGB é retomada do ponto onde parou.
*   Ao pressionar o Botão B, a tarefa do buzzer é suspensa (o buzzer para de bipar). Ao soltar o Botão B, a tarefa do buzzer é retomada.
*   As três funcionalidades (LED, buzzer, botões) operam de forma concorrente e responsiva.
*   Mensagens de status sobre a suspensão e retomada das tarefas são exibidas no monitor serial.

## 🤔 Reflexão 

### O que acontece se todas as tarefas tiverem a mesma prioridade?

Se todas as tarefas têm a mesma prioridade, o FreeRTOS geralmente as reveza. Cada uma usa o processador por um pequeno período e depois passa a vez para a próxima. Isso cria a sensação de que todas estão rodando ao mesmo tempo.

### Qual tarefa consome mais tempo da CPU neste projeto?

A tarefa que verifica os botões (`button_task`) provavelmente usa mais a CPU. Isso acontece porque ela "acorda" mais vezes (a cada 50 milissegundos) para ver se algum botão foi pressionado, enquanto as tarefas do LED e do buzzer "dormem" por mais tempo. Mesmo que ela faça pouca coisa cada vez que acorda, a frequência maior faz com que ela use mais o processador ao longo do tempo.

### Quais seriam os riscos de usar polling (verificação constante) sem gerenciar bem as prioridades?

Usar "polling" (ficar verificando algo repetidamente) sem cuidado pode trazer problemas:

*   **Desperdício de Processamento:** O processador gasta tempo verificando, mesmo que nada tenha mudado.
*   **Lentidão em Outras Tarefas:**
    *   Se uma tarefa de polling tem a mesma prioridade que outras, ela pode "roubar" tempo delas, deixando o sistema lento.
    *   Se ela tem prioridade alta, pode impedir que tarefas menos importantes (mas ainda necessárias) rodem.
    *   Se tem prioridade baixa, pode demorar para perceber uma mudança (como um botão pressionado) se outras tarefas estiverem ocupando o processador.
*   **Dificuldade para Economizar Energia:** O processador fica sempre ativo, gastando mais bateria.

## 📜 Licença
GNU GPL-3.0.
