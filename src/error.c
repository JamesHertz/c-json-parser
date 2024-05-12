#include "error.h"

const char * error2str(error_t error){
    switch (error) {
        case SCANNER_UNKNOWN_ESCAPE:
            return "Unknown escaped sequence symbol.";
        case SCANNER_UNEXPECTED_SYMBOL:
            return "Unexpected symbol!";
        case SCANNER_UNFINISH_STRING:
            return "Expected enclosing '\"'.";
        case SCANNER_BAD_NUMBER_FORMAT:
            return "No well-formatted number (should end with a digit).";
        case SCANNER_INVALID_INDENTIFIER:
            return "Invalid indentifier, should've been either 'true' or 'false'";
        default:
            return NULL;
    }
}
