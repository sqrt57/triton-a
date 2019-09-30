; String utilities
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Drives the whole compilation process
; In: 1) Null-terminated string
; Out: EAX - length of string not including NULL character
length:
        push edi
        mov edi, [esp+8]
        xor eax, eax
        mov ecx, -1
        repne scasb
        mov eax, ecx
        neg eax
        sub eax, 2
        pop edi
        ret
