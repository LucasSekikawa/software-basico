#include <stdio.h>
#include <stdlib.h>

void setup_brk();
void dismiss_brk();
void* memory_alloc(unsigned long int bytes, unsigned char strategy);
int memory_free(void *pointer);						

extern void *original_brk;	/* Você precisa ter a variável global que armazena o valor de brk como um extern aqui.
				No código de teste estou chamandando de original_brk, utilizem esse nome também. */

int main() { 

	printf("============================== ROTINAS DE TESTE ==============================\n");
	
	setup_brk();
	void *initial_brk = original_brk;
	void *f_pnt, *s_pnt, *t_pnt, *fo_pnt;

	f_pnt = memory_alloc(100, 0);
	printf("==>> ALOCANDO UM ESPAÇO DE 100 BYTES WORST FIT:\n");
	printf("\tLOCAL: %s\n", f_pnt-9 == initial_brk ? "CORRETO!" : "INCORRETO!");
	printf("\tIND. DE USO: %s\n", *((char*) (f_pnt-9)) == 1 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (f_pnt-8)) == 100 ? "CORRETO!" : "INCORRETO!");

	printf("==>> DESALOCANDO UM ESPAÇO DE 100 BYTES:\n");
	memory_free(f_pnt);
	printf("\tIND. DE USO: %s\n", *((char*) (f_pnt-9)) == 0 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (f_pnt-8)) == 100 ? "CORRETO!" : "INCORRETO!");

	s_pnt = memory_alloc(50, 0);
	t_pnt = memory_alloc(100, 0);
	printf("==>> ALOCANDO UM ESPAÇO DE 50 BYTES WORST FIT:\n");
	printf("\tLOCAL: %s\n", s_pnt-9 == initial_brk ? "CORRETO!" : "INCORRETO!");
	printf("\tIND. DE USO: %s\n", *((char*) (s_pnt-9)) == 1 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (s_pnt-8)) == 50 ? "CORRETO!" : "INCORRETO!");
	printf("==>> ALOCANDO UM ESPAÇO DE 100 BYTES WORST FIT:\n");
	printf("\tLOCAL: %s\n", s_pnt+100 == t_pnt-9 ? "CORRETO!" : "INCORRETO!");
	printf("\tIND. DE USO: %s\n", *((char*) (t_pnt-9)) == 1 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (t_pnt-8)) == 100 ? "CORRETO!" : "INCORRETO!");
	printf("==> VERIFICANDO A FRAGMENTAÇÃO DE MEMÓRIA:\n");
	printf("\tIND. DE USO: %s\n", *((char*) (s_pnt+50)) == 0 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (s_pnt+51)) == 41 ? "CORRETO!" : "INCORRETO!");
	
	printf("==>> DESALOCANDO TUDO:\n");
	memory_free(s_pnt);
	memory_free(t_pnt);
	printf("\tIND. DE USO: %s\n", *((char*) (s_pnt-9)) == 0 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (s_pnt-8)) == 50 ? "CORRETO!" : "INCORRETO!");
	printf("\tIND. DE USO: %s\n", *((char*) (t_pnt-9)) == 0 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (t_pnt-8)) == 100 ? "CORRETO!" : "INCORRETO!");
	
	printf("==>> ALOCANDO UM ESPAÇO DE 40 BYTES BEST FIT:\n");
	fo_pnt = memory_alloc(40, 1);
	printf("\tLOCAL: %s\n", s_pnt+59 == fo_pnt ? "CORRETO!" : "INCORRETO!");
	printf("\tIND. DE USO: %s\n", *((char*) (fo_pnt-9)) == 1 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (fo_pnt-8)) == 41 ? "CORRETO!" : "INCORRETO!");

	printf("==>> DESALOCANDO UM ESPAÇO DE 40 BYTES:\n");
	memory_free(fo_pnt);
	printf("\tIND. DE USO: %s\n", *((char*) (fo_pnt-9)) == 0 ? "CORRETO!" : "INCORRETO!");
	printf("\tTAMANHO: %s\n", *((long long*) (fo_pnt-8)) == 41 ? "CORRETO!" : "INCORRETO!");

	printf("==>> DESALOCANDO A PILHA (ILEGAL):\n");
	unsigned long long stack_var = 0;
	unsigned int alloc_return = memory_free((void*) &stack_var);
	if (!alloc_return) printf("\tO RETORNO DA LIBERAÇÃO FOI NULL!\n");
	else printf("\tO RETORNO DA LIBERAÇÃO NÃO FOI NULL!\n");

	printf("\n============================== TESTES ADICIONAIS ==============================\n");

	printf("==>> DESALOCANDO UM PONTEIRO NULO (ILEGAL):\n");
	if (memory_free(NULL) == 0) printf("\tRETORNO: CORRETO!\n");
	else printf("\tRETORNO: INCORRETO!\n");

	printf("==>> ALOCANDO 50 BYTES WORST FIT COM VÁRIOS BLOCOS LIVRES:\n");
	void *p = memory_alloc(50, 0);
	if (p == t_pnt) printf("\tLOCAL: CORRETO!\n");
	else printf("\tLOCAL: INCORRETO!\n");
	if (p == NULL) return 1;

	printf("==>> DESALOCANDO O MEIO DO BLOCO (ILEGAL):\n");
	if (memory_free(p + 25) == 0) printf("\tRETORNO: CORRETO!\n");
	else printf("\tRETORNO: INCORRETO!\n");
	if (*((char*) (p-9)) == 1) printf("\tIND. DE USO: CORRETO!\n");
	else printf("\tIND. DE USO: INCORRETO!\n");

	printf("==>> DESALOCANDO O CABEÇALHO DO BLOCO (ILEGAL):\n");
	if (memory_free(p-9) == 0) printf("\tRETORNO: CORRETO!\n");
	else printf("\tRETORNO: INCORRETO!\n");

	printf("==>> DESALOCANDO O MESMO BLOCO DUAS VEZES:\n");
	if (memory_free(p) == 1) printf("\tPRIMEIRA DESALOCAÇÃO: CORRETO!\n");
	else printf("\tPRIMEIRA DESALOCAÇÃO: INCORRETO!\n");
	if (memory_free(p) == 0) printf("\tSEGUNDA DESALOCAÇÃO: CORRETO!\n");
	else printf("\tSEGUNDA DESALOCAÇÃO: INCORRETO!\n");

	printf("==>> ALOCANDO ZERO BYTES:\n");
	if (memory_alloc(0, 0) == NULL) printf("\tRETORNO: CORRETO!\n");
	else printf("\tRETORNO: INCORRETO!\n");

	printf("==>> ALOCANDO COM ESTRATÉGIA INVÁLIDA:\n");
	if (memory_alloc(20, 2) == NULL) printf("\tRETORNO: CORRETO!\n");
	else printf("\tRETORNO: INCORRETO!\n");

	printf("==>> ALOCANDO UM BLOCO DE TAMANHO EXATO BEST FIT:\n");
	p = memory_alloc(41, 1);
	if (p == fo_pnt) printf("\tLOCAL: CORRETO!\n");
	else printf("\tLOCAL: INCORRETO!\n");
	if (p == NULL) return 1;
	if (*((long long*) (p-8)) == 41) printf("\tTAMANHO: CORRETO!\n");
	else printf("\tTAMANHO: INCORRETO!\n");

	printf("==>> DIVIDINDO UM BLOCO COM SOBRA DE 10 BYTES:\n");
	void *q = memory_alloc(40, 0);
	if (q == NULL) return 1;
	if (*((long long*) (q-8)) == 40 && *((char*) (q+40)) == 0 && *((long long*) (q+41)) == 1)
		printf("\tFRAGMENTAÇÃO: CORRETO!\n");
	else printf("\tFRAGMENTAÇÃO: INCORRETO!\n");

	printf("==>> ALOCANDO UM BLOCO MAIOR QUE OS BLOCOS LIVRES:\n");
	void *r = memory_alloc(200, 0);
	if (r == NULL) return 1;
	if (r == t_pnt+109) printf("\tLOCAL: CORRETO!\n");
	else printf("\tLOCAL: INCORRETO!\n");
	*((char*) p) = 'A';
	*((char*) q) = 'B';
	*((char*) r) = 'C';
	memory_free(q);
	if (*((char*) p) == 'A' && *((char*) r) == 'C') printf("\tDADOS PRESERVADOS: CORRETO!\n");
	else printf("\tDADOS PRESERVADOS: INCORRETO!\n");

	printf("==>> RESTAURANDO A HEAP:\n");
	dismiss_brk();
	setup_brk();
	if (original_brk == initial_brk) printf("\tENDEREÇO INICIAL: CORRETO!\n");
	else printf("\tENDEREÇO INICIAL: INCORRETO!\n");

	return 0;
}
