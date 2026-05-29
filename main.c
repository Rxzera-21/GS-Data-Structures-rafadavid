#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* --- Constantes de limites --- */
#define TEMP_CRITICA     80.0f
#define ENERGIA_CRITICA  20.0f
#define MAX_HISTORICO    10

/* --- Codigos de cor ANSI (funcionam no Windows com ANSI habilitado) --- */
#define COR_RESET    "\033[0m"
#define COR_VERDE    "\033[1;32m"
#define COR_AMARELO  "\033[1;33m"
#define COR_VERMELHO "\033[1;31m"
#define COR_CIANO    "\033[1;36m"
#define COR_BRANCO   "\033[1;37m"
#define COR_AZUL     "\033[1;34m"

/* --- Estrutura de uma leitura --- */
typedef struct {
    int    id;
    float  temperatura;
    float  energia;
    int    comunicacao;   /* 1 = OK, 0 = falha */
} Leitura;

/* --- Variaveis globais --- */
Leitura historico[MAX_HISTORICO];
int     totalLeituras = 0;
int     proximoId     = 1;

/* ================================================
   Prototipos
   ================================================ */
void exibirCabecalho(void);
void exibirMenu(void);
void inserirDados(void);
void visualizarStatus(void);
void executarAnalise(void);
void exibirHistorico(void);
void limparBuffer(void);
void pausar(void);

/* ================================================
   MAIN
   ================================================ */
int main(void) {
    int opcao;
    int executando = 1;

    exibirCabecalho();

    while (executando) {
        exibirMenu();
        printf(COR_CIANO "  Opcao: " COR_RESET);

        if (scanf("%d", &opcao) != 1) {
            limparBuffer();
            printf(COR_VERMELHO "  [ERRO] Entrada invalida.\n" COR_RESET);
            pausar();
            continue;
        }
        limparBuffer();

        switch (opcao) {
            case 1:
                inserirDados();
                break;
            case 2:
                visualizarStatus();
                break;
            case 3:
                executarAnalise();
                break;
            case 4:
                exibirHistorico();
                break;
            case 0:
                printf(COR_VERDE "\n  Encerrando o sistema... Ate a proxima missao!\n\n" COR_RESET);
                executando = 0;
                break;
            default:
                printf(COR_AMARELO "  [AVISO] Opcao invalida. Tente novamente.\n" COR_RESET);
                pausar();
        }
    }

    return 0;
}

/* ================================================
   Exibe cabecalho da aplicacao
   ================================================ */
void exibirCabecalho(void) {
    printf("\n");
    printf(COR_CIANO "  +----------------------------------------------+\n" COR_RESET);
    printf(COR_CIANO "  |   ** SISTEMA DE MONITORAMENTO ESPACIAL **    |\n" COR_RESET);
    printf(COR_CIANO "  |          GS2026.1 - Missao Alpha             |\n" COR_RESET);
    printf(COR_CIANO "  +----------------------------------------------+\n" COR_RESET);
    printf("\n");
}

/* ================================================
   Exibe o menu principal
   ================================================ */
void exibirMenu(void) {
    printf("\n");
    printf(COR_BRANCO "  +-----------------------------------+\n" COR_RESET);
    printf(COR_BRANCO "  |         MENU PRINCIPAL            |\n" COR_RESET);
    printf(COR_BRANCO "  +-----------------------------------+\n" COR_RESET);
    printf(COR_BRANCO "  |  1. Inserir dados dos sensores    |\n" COR_RESET);
    printf(COR_BRANCO "  |  2. Visualizar status atual       |\n" COR_RESET);
    printf(COR_BRANCO "  |  3. Executar analise completa     |\n" COR_RESET);
    printf(COR_BRANCO "  |  4. Historico de leituras         |\n" COR_RESET);
    printf(COR_BRANCO "  |  0. Encerrar sistema              |\n" COR_RESET);
    printf(COR_BRANCO "  +-----------------------------------+\n" COR_RESET);
}

/* ================================================
   Insercao de dados dos sensores
   ================================================ */
void inserirDados(void) {
    Leitura nova;
    int comStatus;

    printf(COR_AZUL "\n  -- INSERCAO DE DADOS --\n\n" COR_RESET);

    /* Temperatura */
    printf("  Temperatura da nave (graus C): ");
    while (scanf("%f", &nova.temperatura) != 1) {
        limparBuffer();
        printf(COR_VERMELHO "  Valor invalido. Tente novamente: " COR_RESET);
    }
    limparBuffer();

    /* Energia */
    printf("  Nivel de energia (%%): ");
    while (scanf("%f", &nova.energia) != 1 ||
           nova.energia < 0.0f || nova.energia > 100.0f) {
        limparBuffer();
        printf(COR_VERMELHO "  Valor invalido (0-100). Tente novamente: " COR_RESET);
    }
    limparBuffer();

    /* Comunicacao */
    printf("  Comunicacao (1 = OK  |  0 = Falha): ");
    while (scanf("%d", &comStatus) != 1 ||
           (comStatus != 0 && comStatus != 1)) {
        limparBuffer();
        printf(COR_VERMELHO "  Digite apenas 0 ou 1: " COR_RESET);
    }
    limparBuffer();

    nova.comunicacao = comStatus;
    nova.id          = proximoId++;

    /* Armazena no historico (janela deslizante) */
    int pos = (totalLeituras < MAX_HISTORICO)
              ? totalLeituras
              : MAX_HISTORICO - 1;

    if (totalLeituras >= MAX_HISTORICO) {
        for (int i = 0; i < MAX_HISTORICO - 1; i++) {
            historico[i] = historico[i + 1];
        }
    }

    historico[pos] = nova;
    if (totalLeituras < MAX_HISTORICO) totalLeituras++;

    printf(COR_VERDE "\n  [OK] Leitura #%d registrada com sucesso!\n" COR_RESET, nova.id);
    pausar();
}

/* ================================================
   Visualiza a ultima leitura registrada
   ================================================ */
void visualizarStatus(void) {
    printf(COR_AZUL "\n  -- STATUS ATUAL DA MISSAO --\n\n" COR_RESET);

    if (totalLeituras == 0) {
        printf(COR_AMARELO "  Nenhuma leitura registrada ainda.\n" COR_RESET);
        pausar();
        return;
    }

    Leitura *ultima = &historico[totalLeituras - 1];

    /* Temperatura */
    const char *corTemp = (ultima->temperatura > TEMP_CRITICA)
                          ? COR_VERMELHO : COR_VERDE;
    printf("  [TEMP]  Temperatura : ");
    printf("%s%.1f graus C%s\n", corTemp, ultima->temperatura, COR_RESET);

    /* Energia */
    const char *corEner = (ultima->energia < ENERGIA_CRITICA)
                          ? COR_VERMELHO : COR_VERDE;
    printf("  [ENER]  Energia     : ");
    printf("%s%.1f%%%s\n", corEner, ultima->energia, COR_RESET);

    /* Comunicacao */
    if (ultima->comunicacao == 1) {
        printf("  [COM]   Comunicacao : " COR_VERDE "ONLINE\n" COR_RESET);
    } else {
        printf("  [COM]   Comunicacao : " COR_VERMELHO "OFFLINE\n" COR_RESET);
    }

    printf("\n  (Leitura #%d)\n", ultima->id);
    pausar();
}

/* ================================================
   Analise completa com alertas
   ================================================ */
void executarAnalise(void) {
    printf(COR_AZUL "\n  -- ANALISE OPERACIONAL --\n\n" COR_RESET);

    if (totalLeituras == 0) {
        printf(COR_AMARELO "  Nenhuma leitura disponivel para analise.\n" COR_RESET);
        pausar();
        return;
    }

    Leitura *u = &historico[totalLeituras - 1];
    int alertas = 0;

    /* Verificacao de temperatura */
    if (u->temperatura > TEMP_CRITICA) {
        printf(COR_VERMELHO
               "  [!] ALERTA: Superaquecimento detectado! (%.1f graus > %.0f graus)\n"
               COR_RESET, u->temperatura, TEMP_CRITICA);
        alertas++;
    } else {
        printf(COR_VERDE "  [OK] Temperatura dentro do limite normal.\n" COR_RESET);
    }

    /* Verificacao de energia */
    if (u->energia < ENERGIA_CRITICA) {
        printf(COR_VERMELHO
               "  [!] ALERTA: Modo de economia de energia ativado! (%.1f%% < %.0f%%)\n"
               COR_RESET, u->energia, ENERGIA_CRITICA);
        alertas++;
    } else {
        printf(COR_VERDE "  [OK] Nivel de energia satisfatorio.\n" COR_RESET);
    }

    /* Verificacao de comunicacao */
    if (u->comunicacao == 0) {
        printf(COR_VERMELHO
               "  [!] ALERTA: Falha de comunicacao! Sinal perdido.\n"
               COR_RESET);
        alertas++;
    } else {
        printf(COR_VERDE "  [OK] Canal de comunicacao operacional.\n" COR_RESET);
    }

    /* Resumo */
    printf("\n");
    if (alertas == 0) {
        printf(COR_VERDE "  == MISSAO: STATUS NOMINAL ==\n" COR_RESET);
    } else {
        printf(COR_VERMELHO
               "  == MISSAO: %d ALERTA(S) CRITICO(S) ==\n"
               COR_RESET, alertas);
    }

    /* Medias do historico */
    if (totalLeituras > 1) {
        float somaTemp = 0.0f, somaEner = 0.0f;
        int   falhasCom = 0;

        for (int i = 0; i < totalLeituras; i++) {
            somaTemp  += historico[i].temperatura;
            somaEner  += historico[i].energia;
            if (historico[i].comunicacao == 0) falhasCom++;
        }

        printf("\n  Medias do historico (%d leituras):\n", totalLeituras);
        printf("    Temperatura media : %.1f graus C\n", somaTemp / totalLeituras);
        printf("    Energia media     : %.1f%%\n",  somaEner / totalLeituras);
        printf("    Falhas de comun.  : %d\n", falhasCom);
    }

    pausar();
}

/* ================================================
   Exibe o historico de leituras
   ================================================ */
void exibirHistorico(void) {
    printf(COR_AZUL "\n  -- HISTORICO DE LEITURAS --\n\n" COR_RESET);

    if (totalLeituras == 0) {
        printf(COR_AMARELO "  Nenhuma leitura registrada.\n" COR_RESET);
        pausar();
        return;
    }

    printf(COR_BRANCO "  %-5s  %-14s  %-12s  %-12s\n" COR_RESET,
           "ID", "Temp (graus C)", "Energia (%)", "Comunicacao");
    printf("  %-5s  %-14s  %-12s  %-12s\n",
           "-----", "--------------", "------------", "------------");

    for (int i = 0; i < totalLeituras; i++) {
        Leitura *l = &historico[i];
        const char *corT   = (l->temperatura > TEMP_CRITICA)    ? COR_VERMELHO : COR_VERDE;
        const char *corE   = (l->energia     < ENERGIA_CRITICA) ? COR_VERMELHO : COR_VERDE;
        const char *corC   = (l->comunicacao == 0)              ? COR_VERMELHO : COR_VERDE;
        const char *comStr = (l->comunicacao == 1) ? "OK" : "FALHA";

        printf("  %-5d  %s%-14.1f%s  %s%-12.1f%s  %s%-12s%s\n",
               l->id,
               corT, l->temperatura, COR_RESET,
               corE, l->energia,     COR_RESET,
               corC, comStr,         COR_RESET);
    }

    pausar();
}

/* ================================================
   Utilitarios
   ================================================ */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pausar(void) {
    printf(COR_CIANO "\n  Pressione ENTER para continuar..." COR_RESET);
    getchar();
}
