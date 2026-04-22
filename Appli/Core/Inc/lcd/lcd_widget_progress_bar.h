#ifndef INC_LCD_LCD_WIDGET_PROGRESS_BAR_H_
#define INC_LCD_LCD_WIDGET_PROGRESS_BAR_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define PROGRESS_BAR_H   	20
#define PROGRESS_BAR_W      350
#define PROGRESS_BAR_Y      240
#define PROGRESS_TEXT_Y		25

void MenuDrawProgress(uint8_t progressPercent);
void MenuResetProgressBar(void);

#endif /* INC_LCD_LCD_WIDGET_PROGRESS_BAR_H_ */
