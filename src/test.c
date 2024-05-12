#include "error.h"
#include "scanner.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>

#define error(fmt, ...) fprintf(stderr, "[ERROR]: " fmt  __VA_OPT__(,) __VA_ARGS__)
#define info(fmt,  ...) printf( "[INFO]: " fmt __VA_OPT__(,) __VA_ARGS__)

#define next_nr_assert(value) do {           \
       assert(value);                        \
       info("test %02d [OK]\n", test_nr++);  \
    }while(0)

#define varg_sequence(...) ((token_type_t[]){__VA_ARGS__ __VA_OPT__(,) TOKEN_EOF})

bool assert_token(token_t t1, token_t t2){

    switch (t1.type) {
        case TOKEN_ERROR:
           if(t1.type == t2.type) {
               if(t1.error_or_lenth.errcode == t2.error_or_lenth.errcode) return true;
               error("Expected error %s but got error %s\n", 
                       error2str(t1.error_or_lenth.errcode),
                       error2str(t2.error_or_lenth.errcode)
                );
           } else {
               error("Expected error %s but got token %s\n", 
                       error2str(t1.error_or_lenth.errcode), ttype2str(t2.type)
            );
           }
           return false;
      default:
         if(t1.type == t2.type) return true;
         error("Expected %s but found %s\n", ttype2str(t1.type), ttype2str(t2.type));
         if(t2.type == TOKEN_ERROR)
             fprintf(stderr, "--> errmsg: %s\n", error2str(t2.error_or_lenth.errcode));
         return false;

    }

}


bool check_stream(token_type_t expected[], const char * source){
   token_type_t * curr = expected;
   token_t token;

   scanner_t scan;
   scanner_init(&scan, source);

   do {
       token = scanner_scan_next(&scan);
       if(token.type != *curr){
           token_t helper = { .type = *curr };
           error("Unmatch at position [%zu]\n", curr - expected);
           // to print message
           assert_token(helper, token);
           return false;
       }

   } while( *curr++ != TOKEN_EOF );

   return true;
}


// by now I support only one error c:
bool check_error(error_t errcode, const char * source){
   scanner_t scan;
   scanner_init(&scan, source);

   token_t token = scanner_scan_next(&scan);
   // while(token.type == TOKEN_EOF && token.type == TOKEN_ERROR){
   //      token = scanner_scan_next(&scan);
   // }

   token_t expected;
   expected.type = TOKEN_ERROR;
   expected.error_or_lenth.errcode = errcode;
   return assert_token(expected, token);
}

bool check_eofs(){
   scanner_t scan;
   scanner_init(&scan, "    ");

   token_t token = scanner_scan_next(&scan);
   token_t expected = { .type = TOKEN_EOF };
   for(int i = 0; i < 10; i++){
       if(!assert_token(expected, token))
           return false;
       token = scanner_scan_next(&scan);
   }

   return true;
}


void run_tests(){

    int test_nr = 0;
    next_nr_assert(check_stream( 
         varg_sequence(
            TOKEN_LEFT_BRACE,   TOKEN_RIGHT_BRACE,
            TOKEN_LEFT_BRACKET, TOKEN_RIGHT_BRACKET,
            TOKEN_COMMA, TOKEN_COLON, TOKEN_TRUE, TOKEN_FALSE
        ), "{}[],: true false"
    ));

    next_nr_assert(check_stream(
            varg_sequence(TOKEN_STRING), "\" \\t \\f \\r \\b \\n \\\\ \\\" \" "
    ));

    next_nr_assert(check_eofs());

    next_nr_assert(check_stream( varg_sequence(), ""));
    next_nr_assert(
               check_error(SCANNER_UNEXPECTED_SYMBOL, "@\n")
            && check_error(SCANNER_UNFINISH_STRING,   "\"json\n")
            && check_error(SCANNER_UNKNOWN_ESCAPE,    "\"json\\g\"")
    );

    next_nr_assert(check_stream(
        varg_sequence(
            TOKEN_LEFT_BRACE, TOKEN_STRING, TOKEN_COLON, TOKEN_STRING, TOKEN_COMMA,
            TOKEN_STRING, TOKEN_COLON, TOKEN_NUMBER, TOKEN_RIGHT_BRACE
        ), 
        " { \"name\": \"json-parser\", \"version\": 1 } "
    ));

    next_nr_assert(
               check_error(SCANNER_INVALID_INDENTIFIER, "json")
            && check_error(SCANNER_INVALID_INDENTIFIER, "True")
            && check_error(SCANNER_INVALID_INDENTIFIER, "falsE")
            && check_error(SCANNER_INVALID_INDENTIFIER, "truefalse")
    );
}

int main(void){
    run_tests();
    return 0;
}
