#include "lcd_lang.h"

static Language currentLanguage = LANG_EN;

const char* LanguageToString(Language lang)
{
    switch(lang) {
        case LANG_EN:   return "ENGLISH";
        case LANG_HE:   return "HEBREW";
        default: 		return "";
    }
    return "";
}

void SetLanguage(Language lang) {
    
    if (lang >= LANG_EN && lang < LANG_COUNT) {
        currentLanguage = lang;
    }
    
}

Language GetLanguage(void) {
    return currentLanguage;
}
