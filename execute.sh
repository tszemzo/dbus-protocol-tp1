#!/usr/bash
python2 ./cpplint.py --extensions=h,hpp,c,cpp --filter=-,+build/class,+build/deprecated,+build/include_what_you_use,+build/namespaces,+build/printf_format,+readability/braces,+readability/check,+readability/fn_size,+readability/function,+readability/multiline_comment,+readability/multiline_string,+readability/utf8,+runtime/arrays,+runtime/casting,+runtime/explicit,+runtime/init,+runtime/invalid_increment,+runtime/memset,+runtime/operator,+runtime/printf,+runtime/printf_format,+runtime/rtti,+runtime/string,+runtime/threadsafe_fn,+whitespace/blank_line,+whitespace/empty_loop_body,+whitespace/ending_newline,+whitespace/line_length,+whitespace/newline,+whitespace/parens,+whitespace/semicolon *.h *.c