; Windows-specific stuff, entry point
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

format PE console
entry start

section '.rdata' data readable
include 'messages.inc'

section '.data' data readable writable
include 'variables.inc'
        stdin dd ?
        win32_heap dd ?

section '.text' code executable readable

; Entry point
start:
        nop
        cld ; Clear direction flag according to calling conventions

        ; Initialize stdin
        push -11
        call [GetStdHandle]
        mov [stdin], eax

        ; Initialize heap
        call [GetProcessHeap]
        mov [win32_heap], eax

        call [GetCommandLineA]
        push eax

        ; Calculate length of command line
        push eax
        call str_length
        add esp, 4

        ; Call driver
        pop ecx
        push eax
        push ecx
        call drive
        add esp, 8

        push eax
        call [ExitProcess]

include 'driver.inc'
include 'memory.inc'
include 'io.inc'
include 'lexer.inc'
include 'parser_stub.inc'

section '.idata' import data readable writable

        dd 0, 0, 0, rva kernel32.name, rva kernel32.iat
        dd 0, 0, 0, 0, 0

        kernel32.name db 'kernel32.dll', 0

        align 4
    kernel32.iat:
        ExitProcess     dd rva ExitProcess_name
        GetCommandLineA dd rva GetCommandLineA_name
        GetStdHandle    dd rva GetStdHandle_name
        CloseHandle     dd rva CloseHandle_name
        CreateFileA     dd rva CreateFileA_name
        ReadFile        dd rva ReadFile_name
        WriteFile       dd rva WriteFile_name
        GetFileSizeEx   dd rva GetFileSizeEx_name
        GetProcessHeap  dd rva GetProcessHeap_name
        HeapAlloc       dd rva HeapAlloc_name
        HeapFree        dd rva HeapFree_name
        dd 0

        ExitProcess_name        db 0, 0, 'ExitProcess', 0
        GetCommandLineA_name    db 0, 0, 'GetCommandLineA', 0
        GetStdHandle_name       db 0, 0, 'GetStdHandle', 0
        CloseHandle_name        db 0, 0, 'CloseHandle', 0
        CreateFileA_name        db 0, 0, 'CreateFileA', 0
        ReadFile_name           db 0, 0, 'ReadFile', 0
        WriteFile_name          db 0, 0, 'WriteFile', 0
        GetFileSizeEx_name      db 0, 0, 'GetFileSizeEx', 0
        GetProcessHeap_name     db 0, 0, 'GetProcessHeap', 0
        HeapAlloc_name          db 0, 0, 'HeapAlloc', 0
        HeapFree_name           db 0, 0, 'HeapFree', 0
