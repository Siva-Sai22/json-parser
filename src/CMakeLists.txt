add_executable(json_parser)

target_sources(json_parser
    PRIVATE
        main.c
        lexer.c
        parser.c
)

target_include_directories(json_parser
    PRIVATE
        include
)
