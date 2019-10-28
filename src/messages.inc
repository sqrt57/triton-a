; String messages
;
; This file is part of Triton-A project.
; Copyright 2019, Dmitry Grigoryev

newline_str db 0x0a
newline_len = 1

done_str db 'Done.', 0x0a
done_len = $-done_str

quote_str db '"'
quote_len = $-quote_str

apostrophe_str db "'"
apostrophe_len = $-apostrophe_str

reading_str db 'Reading '
reading_len = $-reading_str

writing_str db 'Writing '
writing_len = $-writing_str

unknow_option_str db 'Error: unknown command line option: '
unknow_option_len = $-unknow_option_str

error_reading_output_filename_str db 'Error: -o option: output file name should be specified', 0x0a
error_reading_output_filename_len = $-error_reading_output_filename_str

output_file_not_specified_str db 'Error: output file name not specified', 0x0a
output_file_not_specified_len = $-output_file_not_specified_str

no_input_file_str db 'Error: no input file specified', 0x0a
no_input_file_len = $-no_input_file_str

usage_str db 'Usage:', 0x0a, '  trac -o program.exe file1.tra [file2.tra ...]', 0x0a
usage_len = $-usage_str

error_reading_file_str db 'Error reading file '
error_reading_file_len = $-error_reading_file_str

error_writing_file_str db 'Error writing file '
error_writing_file_len = $-error_writing_file_str

dummy_file_str db 'Dummy output file'
dummy_file_len = $-dummy_file_str

bad_char_str db 'Error: unexpected char in input: "'
bad_char_len = $-bad_char_str

lexer_starting_str db 'Lexer starting', 0x0a
lexer_starting_len = $-lexer_starting_str

lexer_done_str db 'Lexer done', 0x0a
lexer_done_len = $-lexer_done_str

identifier_str db 'Identifier "'
identifier_len = $-identifier_str

special_str db 'Special "'
special_len = $-special_str

char_token_str db 'Character literal: '''
char_token_len = $-char_token_str

string_token_str db 'String literal: "'
string_token_len = $-string_token_str

number_token_str db 'Number token: '
number_token_len = $-number_token_str

operator_too_long_str db 'Error: operator should be no longer than 4 character: "'
operator_too_long_len = $-operator_too_long_str

expected_char_literal_str db 'Error: expected character literal but got: "'
expected_char_literal_len = $-expected_char_literal_str

char_literal_close_quote_expected_str db 'Error: closing apostrophe expected in character literal: "'
char_literal_close_quote_expected_len = $-char_literal_close_quote_expected_str

string_literal_close_quote_expected_str db "Error: closing quote expected in string literal: '"
string_literal_close_quote_expected_len = $-string_literal_close_quote_expected_str

char_unknown_escape_code_str db "Error: unknown escape code: '"
char_unknown_escape_code_len = $-char_unknown_escape_code_str

char_unknown_escape_code_end_str db "' in character literal", 0x0a
char_unknown_escape_code_end_len = $-char_unknown_escape_code_end_str

string_unknown_escape_code_str db "Error: unknown escape code: '"
string_unknown_escape_code_len = $-string_unknown_escape_code_str

string_unknown_escape_code_end_str db "' in string literal", 0x0a
string_unknown_escape_code_end_len = $-string_unknown_escape_code_end_str

comment_not_closed_str db 'Comment should be closed before end of file', 0x0a
comment_not_closed_len = $-comment_not_closed_str

number_too_big_str db 'Number literal does not fit into 32 bits: '
number_too_big_len = $-number_too_big_str

number_literal_bad_char_str db 'Illegal character in number literal: '
number_literal_bad_char_len = $-number_literal_bad_char_str

hex_number_literal_digits_expected_str db 'Digits expected in hexadecimal number literal: '
hex_number_literal_digits_expected_len = $-hex_number_literal_digits_expected_str
