#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include "error.h"
#include "scanner.h"



void print_token_error(const char * source, token_t token){
    printf("Error: %s\n", error2str(token.error_or_lenth.errcode));

    char * line = NULL;
    asprintf(&line, " %zu | ", token.line_nr);

    const char * line_start = token.lexeme;
    while(line_start[-1] != '\n' && line_start > source)
        line_start--;

    const char * line_end   = token.lexeme - 1;
    while(line_end[1] != '\n' && line_end[1] != '\0')
        line_end++;

    int line_length = (int) (line_end - line_start) + 1;
    printf("%s%.*s\n", line, line_length, line_start);
    int chars_to_spot = (int) (strlen(line) + (token.lexeme - line_start) );
    printf("%*s^ here\n", chars_to_spot, "");
    free(line);
}

void run_scanner(const  char * filename){
    printf("scanning '%s'\n", filename);

    struct stat st;
    int fd = open(filename, O_RDONLY);
    if(fd < 0) {
        fprintf(stderr, "Error opening '%s': %s\n", filename, strerror(errno));
        exit(1);
    }

    if( fstat(fd, &st) < 0 ){
        fprintf(stderr, "Error getting stats of '%s': %s\n", filename, strerror(errno));
        exit(1);
    }

    char buffer[st.st_size + 1];
    if( read(fd, buffer, st.st_size) < st.st_size ) {
        fprintf(stderr, "Error reading '%s': %s\n", filename, strerror(errno));
        exit(1);
    }

    buffer[st.st_size] = 0;

    scanner_t scan; 
    scanner_init(&scan, buffer);

    token_t token = scanner_scan_next(&scan);
    for(;;){
        switch(token.type) {
            case TOKEN_ERROR:
                print_token_error(buffer, token);
            case TOKEN_EOF: 
                return;
            case TOKEN_NUMBER:
            case TOKEN_STRING:
                printf("- %.*s\n", (int) token.error_or_lenth.lexeme_lenght, token.lexeme);
                break;
            default:
                printf("- %s\n", ttype2str(token.type));
        }

        token = scanner_scan_next(&scan);
    }

}

int main(int argc, char * argv[]){
    if(argc < 2) {
        fprintf(stderr, "usage: %s <filename>\n", argc > 0 ? argv[0] : "main");
        exit(1);
    }

    run_scanner(argv[1]);
    return 0;
}


