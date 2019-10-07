; Main compiler driver
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

; Drive overall compilation process
; In: 1) Address of command line
;     2) Length of command line
; Out: EAX - program exit code, 0 means success
drive:
        .cmdline equ [ebp+8]
        .cmdline_len equ [ebp+12]
        .cur equ esi
        drive_cur equ esi
        .end equ [ebp-4]
        drive_end equ [ebp-4]
        .in_files_num equ [ebp-8]
        .out_filename equ [ebp-12]
        .out_filename_len equ [ebp-16]

        push ebp
        mov ebp, esp
        sub esp, 16
        push esi

        ; Initialize locals
        mov dword .in_files_num, 0
        mov dword .out_filename, 0
        mov dword .out_filename_len, 0
        mov eax, .cmdline
        mov .cur, eax
        add eax, .cmdline_len
        mov .end, eax

        ; Skip program name
        call drive_skip_arg0

    .arg_loop:
        ; Get arg
        call drive_get_arg
        ; Check for end
        cmp eax, 0
        je .done
        ; Check for options
        cmp byte [eax], '-'
        je .read_option
        push eax
        ; Process input file
        push ecx
        push eax
        call process_input_file
        add esp, 8
        call free
        add esp, 4
        ; Count input file
        inc dword .in_files_num
        ; Loop again
        jmp .arg_loop

    .read_option:
        cmp ecx, 2
        jne .unknow_option
        cmp byte [eax+1], 'o'
        jne .unknow_option
        push eax
        call free
        add esp, 4
        ; Get next arg
        call drive_get_arg
        cmp eax, 0
        je .error_reading_output_fname
        ; Store output file name
        mov .out_filename, eax
        mov .out_filename_len, ecx
        jmp .arg_loop

    .unknow_option:
        push ecx
        push eax
        push unknow_option_len
        push unknow_option_str
        call print
        add esp, 8
        call print
        add esp,8
        call newline
        push usage_len
        push usage_str
        call print
        add esp, 8
        jmp .ret

    .error_reading_output_fname:
        push error_reading_output_filename_len
        push error_reading_output_filename_str
        call print
        add esp, 8
        push usage_len
        push usage_str
        call print
        add esp, 8
        jmp .ret

    .done:
        cmp dword .out_filename, 0
        je .output_filename_missing
        cmp dword .in_files_num, 0
        je .no_input_file
        ; Process output file
        push dword .out_filename_len
        push dword .out_filename
        call process_output_file
        add esp, 8

        ; Print "Done"
        push done_len
        push done_str
        call print
        add esp, 8

        jmp .ret

    .output_filename_missing:
        push output_file_not_specified_len
        push output_file_not_specified_str
        call print
        add esp, 8
        push usage_len
        push usage_str
        call print
        add esp, 8
        jmp .ret

    .no_input_file:
        push no_input_file_len
        push no_input_file_str
        call print
        add esp, 8
        push usage_len
        push usage_str
        call print
        add esp, 8
        jmp .ret

    .ret:
        pop esi
        xor eax, eax
        mov esp, ebp
        pop ebp
        ret

; Inner function of drive
; Skips first argument (executable name)
; Uses drive_cur, drive_cur
drive_skip_arg0:
        xor eax, eax
        xor ecx, ecx
    .skip_whitespace:
        cmp drive_cur, drive_end
        jae .done
        mov edx, drive_cur
        lodsb
        cmp eax, ' '
        je .skip_whitespace
        cmp eax, 9 ; TAB
        je .skip_whitespace
        cmp eax, '"'
        jne .unquoted_loop
        mov edx, drive_cur
        cmp drive_cur, drive_end
        jae .done
        lodsb
    .quoted_loop:
        cmp drive_cur, drive_end
        jae .done
        cmp eax, '"'
        je .done
        inc ecx
        lodsb
        jmp .quoted_loop
    .unquoted_loop:
        cmp drive_cur, drive_end
        jae .done
        cmp eax, ' '
        je .done
        cmp eax, 9 ; TAB
        je .done
        inc ecx
        lodsb
        jmp .unquoted_loop
    .done:
        ret

; Inner function of drive
; Gets next argument from command line
; Uses drive_cur, drive_end
; Out: EAX - address of argument
;      ECX - length of argument
drive_get_arg:
        xor eax, eax
        xor ecx, ecx
        push drive_cur
        ; Calculate string length
    .len_skip_whitespace:
        cmp drive_cur, drive_end
        jae .len_ret_null
        lodsb
        cmp eax, ' '
        je .len_skip_whitespace
        cmp eax, 9 ; TAB
        je .len_skip_whitespace
        cmp eax, '"'
        jne .len_unquoted_loop
        cmp drive_cur, drive_end
        jae .len_ret_null
        lodsb
    .len_quoted_loop:
        cmp drive_cur, drive_end
        jae .len_found_end
        cmp eax, '"'
        je .len_done
        inc ecx
        lodsb
        jmp .len_quoted_loop
    .len_unquoted_loop:
        cmp drive_cur, drive_end
        jae .len_found_end
        cmp eax, ' '
        je .len_done
        cmp eax, 9 ; TAB
        je .len_done
        inc ecx
        lodsb
        jmp .len_unquoted_loop
    .len_found_end:
        inc ecx
    .len_done:
        pop drive_cur
        push ecx
        inc ecx
        push ecx
        call malloc
        add esp, 4
        push edi
        mov edi, eax
        push edi
        jmp .copy_arg
    .len_ret_null:
        add esp, 4
        xor eax, eax
        xor ecx, ecx
        ret
    .copy_arg:
        xor eax, eax
    .skip_whitespace:
        cmp drive_cur, drive_end
        jae .ret_null
        mov edx, drive_cur
        lodsb
        cmp eax, ' '
        je .skip_whitespace
        cmp eax, 9 ; TAB
        je .skip_whitespace
        cmp eax, '"'
        jne .unquoted_loop
        mov edx, drive_cur
        cmp drive_cur, drive_end
        jae .found_end
        lodsb
    .quoted_loop:
        cmp drive_cur, drive_end
        jae .found_end
        cmp eax, '"'
        je .done
        stosb
        lodsb
        jmp .quoted_loop
    .unquoted_loop:
        cmp drive_cur, drive_end
        jae .found_end
        cmp eax, ' '
        je .done
        cmp eax, 9 ; TAB
        je .done
        stosb
        lodsb
        jmp .unquoted_loop
    .found_end:
        stosb
        xor eax, eax
        stosb
        pop eax
        pop edi
        pop ecx
        ret
    .done:
        xor eax, eax
        stosb
        pop eax
        pop edi
        pop ecx
        ret
    .ret_null:
        pop eax
        pop edi
        pop ecx
        xor eax, eax
        xor ecx, ecx
        ret

; Read and compile input file
; In: 1) File name (null-terminated)
;     2) File name length
process_input_file:
        .filename_str equ dword [ebp+8]
        .filename_len equ dword [ebp+12]
        .file equ dword [ebp-4]
        .file_len equ dword [ebp-8]

        push ebp
        mov ebp, esp
        sub esp, 8

        ; Print "Reading filename\n"
        push reading_len
        push reading_str
        call print
        add esp, 8
        push .filename_len
        push .filename_str
        call print
        add esp, 8
        call newline

        ; Read file
        push .filename_len
        push .filename_str
        call read_file
        add esp, 8
        mov .file, eax
        mov .file_len, edx

        ; Print contents
        push .file_len
        push .file
        call print
        add esp, 8
        call newline

        ; Free memory block holding file contents
        push .file
        call free
        add esp, 4

        mov esp, ebp
        pop ebp
        ret

; Write result to output file
; In: 1) File name
;     2) File name length
process_output_file:
        .filename_str equ dword [ebp+8]
        .filename_len equ dword [ebp+12]

        push ebp
        mov ebp, esp

        ; Print "Writing filename\n"
        push writing_len
        push writing_str
        call print
        add esp, 8
        push .filename_len
        push .filename_str
        call print
        add esp, 8
        call newline

        ; Write file
        push dummy_file_len
        push dummy_file_str
        push .filename_len
        push .filename_str
        call write_file
        add esp, 8

        mov esp, ebp
        pop ebp
        ret
