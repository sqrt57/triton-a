; Memory utilities
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Calculates length of null-terminated string
; In: 1) Null-terminated string
; Out: EAX - length of string not including NULL character
str_length:
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

; Copy byte sequence
; In: 1) Destination address
;     2) Source address
;     3) String length
mem_copy:
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

; Compares two byte strings for equality
; In: 1) Address of string 1
;     2) Length of string 1
;     3) Address of string 2
;     4) Length of string 2
; Out: EAX=-1 if strings are equal, 0 otherwise
mem_eq:
        .str1 equ dword [ebp+8]
        .str1_len equ dword [ebp+12]
        .str2 equ dword [ebp+16]
        .str2_len equ dword [ebp+20]

        push ebp
        mov ebp, esp
        push esi
        push edi

        mov ecx, .str1_len
        mov edx, .str2_len
        cmp ecx, edx
        jne .noteq

        mov esi, .str1
        mov edi, .str2
    .next:
        jcxz .eq
        lodsb
        cmp al, byte [edi]
        jne .noteq
        inc edi
        dec ecx
        jmp .next
    .eq:
        xor eax, eax
        dec eax
        jmp .ret
    .noteq:
        xor eax, eax
    .ret:
        pop edi
        pop esi
        mov esp, ebp
        pop ebp
        ret

; Allocate memory
; In: 1) Required amount of memory
; Out: EAX - Address of allocate memory block or NULL on error
malloc:
        push dword [esp+4]
        push dword 0
        push [win32_heap]
        call [HeapAlloc]
        ret

; Free memory
; In: 1) Address of memory block
free:
        push dword [esp+4]
        push dword 0
        push [win32_heap]
        call [HeapFree]
        ret
