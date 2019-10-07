; I/O utilities
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Exits program
; In: 1) Exit code
sys_exit:
        push dword [esp+4]
        call [ExitProcess]

; Prints string to stdandard output
; In: 1) Address of string
;     2) Length of string
print:
        .bytes_written equ [ebp-4]
        push ebp
        mov ebp, esp
        sub esp, 4
        push dword 0
        lea eax, .bytes_written
        push eax
        push dword [ebp+12]
        push dword [ebp+8]
        push [stdin]
        call [WriteFile]
        mov esp, ebp
        pop ebp
        ret

; Prints newline to stdandard output
newline:
        sub esp, 4
        mov eax, esp
        push dword 0
        push eax
        push dword newline_len
        push dword newline_str
        push [stdin]
        call [WriteFile]
        add esp, 4
        ret

; Reads a file into memory
; In: 1) File path address (null-terminated)
;     2) File path length
; Out: EAX - memory address of file data (NULL in case of error)
;      EDX - length of file data
read_file:
        .file_path_str equ [ebp+8]
        .file_path_len equ [ebp+12]

        .handle equ [ebp-4]
        .buffer equ [ebp-8]
        .size_high equ [ebp-12]
        .size_low equ [ebp-16]
        .bytes_written equ [ebp-20]
        .result equ [ebp-24]
        ; Locals:
        ; [EBP-4] File handle
        ; [EBP-8] Allocated memory address
        ; [EBP-12] High doubleword of file size
        ; [EBP-16] Low doubleword of file size
        ; [EBP-20] Number of bytes read
        ; [EBP-24] Result of ReadFile

        push ebp
        mov ebp, esp
        sub esp, 24

        push dword 0               ; hTemplateFile
        push dword 0x80            ; dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL
        push dword 3               ; dwCreationDisposition = OPEN_EXISTING
        push dword 0               ; lpSecurityAttributes
        push dword 1               ; dwShareMode = FILE_SHARE_READ
        push dword 0x80000000      ; dwDesiredAccess = GENERIC_READ
        push dword .file_path_str  ; Null-terminated file path
        call [CreateFileA]
        mov .handle, eax
        cmp eax, -1
        je .error

        lea edx, .size_low
        push edx
        push eax
        call [GetFileSizeEx]
        test eax, eax
        jz .error

        cmp dword .size_high, 0
        jnz .error

        push dword .size_low
        call malloc
        add esp, 4
        mov .buffer, eax
        test eax, eax
        jz .error

        lea edx, .bytes_written
        push dword 0
        push edx
        push dword .size_low
        push dword .buffer
        push dword .handle
        call [ReadFile]
        mov .result, eax

        push dword .handle
        call [CloseHandle]

        cmp dword .result, 0
        je .error

        mov eax, .buffer
        mov edx, .size_low

        mov esp, ebp
        pop ebp
        ret

    .error:
        push error_reading_file_len
        push error_reading_file_str
        call print
        add esp, 8
        push dword .file_path_len
        push dword .file_path_str
        call print
        add esp, 8
        call newline
        push dword 1
        call sys_exit
