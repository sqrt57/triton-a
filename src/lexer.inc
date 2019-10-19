; Lexer
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

lexer_init:
        xor ecx, ecx
        inc ecx

        mov eax, is_single_char_table
        mov [eax + '{'], cl
        mov [eax + '}'], cl
        mov [eax + '('], cl
        mov [eax + ')'], cl
        mov [eax + '['], cl
        mov [eax + ']'], cl
        mov [eax + ';'], cl
        mov [eax + ','], cl
        mov [eax + '.'], cl
        mov [eax + '^'], cl
        mov [eax + '@'], cl

        mov eax, is_operator_char_table
        mov [eax + '+'], cl
        mov [eax + '-'], cl
        mov [eax + '*'], cl
        mov [eax + '/'], cl
        mov [eax + '<'], cl
        mov [eax + '>'], cl
        mov [eax + '='], cl
        mov [eax + '!'], cl
        mov [eax + ':'], cl
        ret

; Split source stream into lexemes and call token evaluator
; In: 1) Address of memory block with source
;     2) Length of memory bock
lex:
        .source_addr equ dword [ebp+8]
        .source_len equ dword [ebp+12]

        .end_addr equ dword [ebp-4]
        .char equ dword [ebp-8]
        . equ dword [ebp-12]

        push ebp
        mov ebp, esp
        sub esp, 12
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

        call is_single_char
        jz .read_single_char

        call is_operator_char
        jz .read_operator

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

    .read_single_char:
        push 1
        push eax
        call lexeme_special
        add esp, 8
        jmp .next

    .read_operator:
        push dword 1
        mov ecx, eax

        cmp esi, .end_addr
        jae .operator_done_eof
        lodsb
        call is_operator_char
        jnz .operator_done

        shl eax, 8
        or ecx, eax
        mov [esp], dword 2

        cmp esi, .end_addr
        jae .operator_done_eof
        xor eax, eax
        lodsb
        call is_operator_char
        jnz .operator_done

        shl eax, 16
        or ecx, eax
        mov [esp], dword 3

        cmp esi, .end_addr
        jae .operator_done_eof
        xor eax, eax
        lodsb
        call is_operator_char
        jnz .operator_done

        shl eax, 24
        or ecx, eax
        mov [esp], dword 4

        cmp esi, .end_addr
        jae .operator_done_eof
        xor eax, eax
        lodsb
        call is_operator_char
        jnz .operator_done

        add esp, 4
        push operator_too_long_len
        push operator_too_long_str
        call print
        add esp, 8
        push dword 5
        lea edx, [esi-5]
        push edx
        call print
        add esp, 8
        call print_quote
        call newline

        push dword 1
        call sys_exit

    .operator_done:
        dec esi
    .operator_done_eof:
        push ecx
        call lexeme_special
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

        push dword 1
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
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is start of identifier
;      EAX - preserves input
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
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is part of identifier
;      EAX - preserves input
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

; Checks whether char is part of operator
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input is part of operator
;      EAX - preserves input
; Notes: trashes EDX value
is_operator_char:
        movzx edx, byte [is_operator_char_table+eax]
        cmp edx, 1
        ret

; Checks whether char consitutes a separate lexeme
; In: EAX - input byte, 3 high bytes should be zero
; Out: ZF=1 iff input consitutes a separate lexeme
;      EAX - preserves input
; Notes: trashes EDX value
is_single_char:
        movzx edx, byte [is_single_char_table+eax]
        cmp edx, 1
        ret
