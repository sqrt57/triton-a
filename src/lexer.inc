; Lexer
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Split source stream into lexemes and call token evaluator
; In: 1) Address of memory block with source
;     2) Length of memory bock
lex:
        .source_addr equ dword [ebp+8]
        .source_len equ dword [ebp+12]

        .end_addr equ dword [ebp-4]
        .char equ dword [ebp-8]

        push ebp
        mov ebp, esp
        sub esp, 8
        push esi

        call lexeme_start_file

        mov esi, .source_addr
        mov eax, esi
        add eax, .source_len
        mov .end_addr, eax

    .next:
        xor eax, eax
    .skip_whitespace:
        cmp esi, .end_addr
        jae .ret
        lodsb
        call is_whitespace
        jz .skip_whitespace

        call is_identifier_start
        jz .read_identifier

        jmp .bad_char

    .read_identifier:
        lea edx, [esi-1]
        push edx
    .identifier_char:
        cmp esi, .end_addr
        jae .done_identifier
        lodsb
        call is_identifier_char
        jz .identifier_char
        dec esi
    .done_identifier:
        pop eax
        mov edx, esi
        sub edx, eax
        push edx
        push eax
        call lexeme_identifier
        add esp, 8
        jmp .next

    .ret:
        call lexeme_end_file

        pop esi
        mov esp, ebp
        pop ebp
        ret

    .bad_char:
        mov .char, eax

        push bad_char_len
        push bad_char_str
        call print
        add esp, 8

        lea eax, .char
        push dword 1
        push eax
        call print
        add esp, 8

        call print_quote
        call newline

        mov eax, 1
        call sys_exit

; Checks whether byte is whitespace character
; In: AL - input byte
; Out: ZF=1 iff input is whitespace
;      AL - preserves input
is_whitespace:
        ; We consider 09-0d, 20 whitespace
        cmp eax, 0x20
        jne .range_low
        ret
    .range_low:
        cmp eax, 0x09
        jae .range_high
        ret
    .range_high:
        cmp eax, 0x0d
        jbe .return_true
        ret
    .return_true:
        cmp eax, eax
        ret

; Checks whether byte is start of identifier
; In: AL - input byte
; Out: ZF=1 iff input is start of identifier
;      AL - preserves input
is_identifier_start:
        ; Identifiear can start with a-z A-Z _
        cmp eax, '_'
        jne .range1_low
        ret
    .range1_low:
        cmp eax, 'a'
        jae .range1_high
        jmp .range2_low
    .range1_high:
        cmp eax, 'z'
        jbe .return_true
        ; jmp .range2_low
    .range2_low:
        cmp eax, 'A'
        jae .range2_high
        ret
    .range2_high:
        cmp eax, 'Z'
        jbe .return_true
        ret
    .return_true:
        cmp eax, eax
        ret

; Checks whether byte is part of identifier
; In: AL - input byte
; Out: ZF=1 iff input is part of identifier
;      AL - preserves input
is_identifier_char:
        ; Identifier consists of a-z A-Z 0-9 _
        cmp eax, '_'
        jne .range1_low
        ret
    .range1_low:
        cmp eax, 'a'
        jae .range1_high
        jmp .range2_low
    .range1_high:
        cmp eax, 'z'
        jbe .return_true
        ; jmp .range2_low
    .range2_low:
        cmp eax, 'A'
        jae .range2_high
        jmp .range3_low
    .range2_high:
        cmp eax, 'Z'
        jbe .return_true
        ; jmp .range3_low
    .range3_low:
        cmp eax, '0'
        jae .range3_high
        ret
    .range3_high:
        cmp eax, '9'
        jbe .return_true
        ret
    .return_true:
        cmp eax, eax
        ret

