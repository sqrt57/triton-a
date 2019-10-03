; String utilities
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Calculates length of null-terminated string
; In: 1) Null-terminated string
; Out: EAX - length of string not including NULL character
length:
        push edi
        mov edi, [esp+8]
        xor eax, eax
        xor ecx, ecx
        dec ecx
        repne scasb
        mov eax, ecx
        not eax
        dec eax
        pop edi
        ret

; Copy string
; In: 1) Destination address
;     2) Source address
;     3) String length
str_copy:
        .dest equ [esp+12]
        .source equ [esp+16]
        .length equ [esp+20]
        push esi
        push edi

        mov esi, .source
        mov edi, .dest
        mov ecx, .length
        rep movsb

        pop edi
        pop esi
        ret
