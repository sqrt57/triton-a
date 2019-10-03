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
