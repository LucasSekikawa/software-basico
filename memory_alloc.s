.text
.globl memory_alloc

memory_alloc:

    # verifica se a alocacao eh de 0 bytes
    cmp $0, %rdi
    je erro_alocacao

    # verifica se a estrategia solicitada eh valida (0 ou 1)
    cmp $0, %sil
    je inicio

    cmp $1, %sil
    jne erro_alocacao


inicio:

    # r8 guarda o endereco do bloco candidato
    movq $0, %r8

    # r9 guarda o endereco do bloco atual, comecando pelo inicio da heap
    movq heap_start(%rip), %r9


ini_loop_busca_blocos:

    # percorre a heap procurando blocos livres compativeis

    # verifica se chegou ao final da heap
    cmpq %r9, heap_end(%rip)
    jbe fim_loop_busca_blocos

    # r10 = indicador de ocupacao do bloco
    movzbq (%r9), %r10

    # r11 = tamanho dos dados do bloco, sem o cabecalho
    movq 1(%r9), %r11

    # se o bloco esta ocupado, continua procurando
    cmpq $0, %r10
    jne pula_bloco

    # se o tamanho solicitado for maior que o tamanho do bloco, continua procurando
    cmpq %r11, %rdi
    ja pula_bloco

    # verifica se ja existe algum candidato a alocacao
    cmpq $0, %r8
    jne possui_candidato

    # guarda o primeiro candidato
    movq %r9, %r8
    jmp pula_bloco


possui_candidato:

    # obtem o tamanho do candidato
    movq 1(%r8), %rax

    # verifica a estrategia de alocacao solicitada
    cmpb $1, %sil
    je best_fit


worst_fit:

    # se o tamanho do bloco atual for maior que o do candidato, atualiza
    cmpq %rax, %r11
    ja atualiza_candidato
    jmp pula_bloco


best_fit:

    # se o tamanho do bloco atual for menor que o do candidato, atualiza
    cmpq %rax, %r11
    jb atualiza_candidato
    jmp pula_bloco


atualiza_candidato:

    # substitui o candidato pelo bloco atual
    movq %r9, %r8
    jmp pula_bloco


pula_bloco:

    # avanca para o proximo bloco, considerando os dados e os 9 bytes do cabecalho
    addq %r11, %r9
    addq $9, %r9
    jmp ini_loop_busca_blocos


fim_loop_busca_blocos:

    # verifica se sera necessario criar um novo bloco ou reutilizar um existente
    cmpq $0, %r8
    je novo_bloco
    jmp reutiliza_bloco


reutiliza_bloco:

    # calcula a quantidade de bytes que sobrara no bloco
    movq 1(%r8), %rax
    subq %rdi, %rax

    # verifica se sobram pelo menos 10 bytes para dividir o bloco
    cmp $10, %rax
    jb ocupa_bloco


divide_bloco:

    # atualiza o tamanho do bloco original
    movq %rdi, 1(%r8)

    # calcula o endereco do novo cabecalho
    movq %r8, %r9
    addq $9, %r9
    addq %rdi, %r9

    # preenche o cabecalho do novo bloco livre
    movb $0, (%r9)
    movq %rax, 1(%r9)

    # desconta os 9 bytes do cabecalho do tamanho restante
    subq $9, 1(%r9)


ocupa_bloco:

    # marca o bloco como ocupado
    movb $1, (%r8)

    # retorna o endereco dos dados do bloco
    movq %r8, %rax
    addq $9, %rax
    ret


novo_bloco:

    # preserva o tamanho solicitado em r10
    movq %rdi, %r10

    # prepara a syscall brk
    movq $12, %rax
    movq heap_end(%rip), %rdi

    # calcula o novo limite da heap, verificando overflow nas somas
    addq %r10, %rdi
    jc erro_alocacao

    addq $9, %rdi
    jc erro_alocacao

    syscall

    # verifica se a expansao da heap foi bem sucedida
    cmp %rax, %rdi
    jne erro_alocacao

    # inicializa o cabecalho do novo bloco
    movb $1, (%r9)
    movq %r10, 1(%r9)

    # atualiza o limite da heap
    movq %rax, heap_end(%rip)

    # retorna o endereco dos dados do novo bloco
    movq %r9, %rax
    addq $9, %rax
    ret


erro_alocacao:

    # retorna NULL em caso de parametros invalidos, overflow ou falha na alocacao
    movq $0, %rax
    ret