#ifndef INC_SCREENS_LCD_LANG_H_
#define INC_SCREENS_LCD_LANG_H_

typedef enum {
    LANG_EN,
    LANG_HE,
    LANG_COUNT
} Language;

void SetLanguage(Language lang);
Language GetLanguage(void);
const char* LanguageToString(Language lang);

#endif /* INC_SCREENS_LCD_LANG_H_ */
