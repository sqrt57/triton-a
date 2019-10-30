; String dictionary
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

hash = fnv_1a

fnv_offset_basis_32 = 2166136261
fnv_prime_32 = 16777619

; Calculates FNV-1a hash sum of string
; In: 1) Address of string
;     2) Length of string
; Out: EAX - hash sum
fnv_1a:
        .str equ dword [esp+16]
        .str_len equ dword [esp+20]

        push esi
        push edi
        push ebx

        mov eax, fnv_offset_basis_32
        mov ecx, .str_len
        test ecx, ecx
        jz .done

        mov esi, .str
        mov edi, fnv_prime_32
        xor ebx, ebx

    .loop:
        mov bl, [esi]
        xor eax, ebx
        inc esi
        mul edi
        dec ecx
        jnz .loop

    .done:
        pop ebx
        pop edi
        pop esi
        ret
