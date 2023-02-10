#ifndef _UI_BASE__H_
#define _UI_BASE__H_

#include <GxEPD2_GFX.h>
#include <fonts.h>

class PaperdinkUIBaseClass
{
    public:
        const GFXfont* font = &PAPERDINK_FONT_SML; 
        int primary_color = GxEPD_BLACK;
        int secondary_color = GxEPD_WHITE;
        int tertiary_color = GxEPD_BLACK;
};

class PaperdinkUIClass : public PaperdinkUIBaseClass
{
    public:

        void display_grid(GxEPD2_GFX& display);

        void display_text_center(GxEPD2_GFX& display, int16_t x, int16_t y, const char* string, int16_t w = 0, int16_t h = 0, const GFXfont* font = &PAPERDINK_FONT_SML, uint8_t color = GxEPD_BLACK, uint8_t size = 1);
}; 

extern PaperdinkUIClass Paperdink_UI;
#endif /* _UI_BASE__H_ */
