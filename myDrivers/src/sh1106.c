#include "sh1106.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>  // For memcpy

#if defined(SH1106_USE_I2C)

void SH1106_Reset(void) {
    /* for I2C - do nothing */
}

// Send a byte to the command register
void SH1106_WriteCommand(uint8_t byte) {
    HAL_I2C_Mem_Write(&SH1106_I2C_PORT, SH1106_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}

// Send data
void SH1106_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_I2C_Mem_Write(&SH1106_I2C_PORT, SH1106_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}

#elif defined(SH1106_USE_SPI)

void SH1106_Reset(void) {
    // CS = High (not selected)
    HAL_GPIO_WritePin(SH1106_CS_Port, SH1106_CS_Pin, GPIO_PIN_SET);

    // Reset the OLED
    HAL_GPIO_WritePin(SH1106_Reset_Port, SH1106_Reset_Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(SH1106_Reset_Port, SH1106_Reset_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}

// Send a byte to the command register
void SH1106_WriteCommand(uint8_t byte) {
    HAL_GPIO_WritePin(SH1106_CS_Port, SH1106_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SH1106_DC_Port, SH1106_DC_Pin, GPIO_PIN_RESET); // command
    HAL_SPI_Transmit(&SH1106_SPI_PORT, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SH1106_CS_Port, SH1106_CS_Pin, GPIO_PIN_SET); // un-select OLED
}

// Send data
void SH1106_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_GPIO_WritePin(SH1106_CS_Port, SH1106_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SH1106_DC_Port, SH1106_DC_Pin, GPIO_PIN_SET); // data
    HAL_SPI_Transmit(&SH1106_SPI_PORT, buffer, buff_size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SH1106_CS_Port, SH1106_CS_Pin, GPIO_PIN_SET); // un-select OLED
}

#else
#error "You should define SH1106_USE_SPI or SH1106_USE_I2C macro"
#endif


// Screenbuffer
static uint8_t SH1106_Buffer[SH1106_BUFFER_SIZE];

// Screen object
static SH1106_t SH1106;

/* Fills the Screenbuffer with values from a given buffer of a fixed length */
SH1106_Error_t SH1106_FillBuffer(uint8_t* buf, uint32_t len) {
    SH1106_Error_t ret = SH1106_ERR;
    if (len <= SH1106_BUFFER_SIZE) {
        memcpy(SH1106_Buffer,buf,len);
        ret = SH1106_OK;
    }
    return ret;
}

/* Initialize the oled screen */
void SH1106_Init(void) {
    // Reset OLED
    SH1106_Reset();

    // Wait for the screen to boot
    HAL_Delay(100);

    // Init OLED
    SH1106_SetDisplayOn(0); //display off

    SH1106_WriteCommand(0x20); //Set Memory Addressing Mode
    SH1106_WriteCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    SH1106_WriteCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SH1106_MIRROR_VERT
    SH1106_WriteCommand(0xC0); // Mirror vertically
#else
    SH1106_WriteCommand(0xC8); //Set COM Output Scan Direction
#endif

    SH1106_WriteCommand(0x00); //---set low column address
    SH1106_WriteCommand(0x10); //---set high column address

    SH1106_WriteCommand(0x40); //--set start line address - CHECK

    SH1106_SetContrast(0xFF);

#ifdef SH1106_MIRROR_HORIZ
    SH1106_WriteCommand(0xA0); // Mirror horizontally
#else
    SH1106_WriteCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SH1106_INVERSE_COLOR
    SH1106_WriteCommand(0xA7); //--set inverse color
#else
    SH1106_WriteCommand(0xA6); //--set normal color
#endif

// Set multiplex ratio.
#if (SH1106_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    SH1106_WriteCommand(0xFF);
#else
    SH1106_WriteCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SH1106_HEIGHT == 32)
    SH1106_WriteCommand(0x1F); //
#elif (SH1106_HEIGHT == 64)
    SH1106_WriteCommand(0x3F); //
#elif (SH1106_HEIGHT == 128)
    SH1106_WriteCommand(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    SH1106_WriteCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    SH1106_WriteCommand(0xD3); //-set display offset - CHECK
    SH1106_WriteCommand(0x00); //-not offset

    SH1106_WriteCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    SH1106_WriteCommand(0xF0); //--set divide ratio

    SH1106_WriteCommand(0xD9); //--set pre-charge period
    SH1106_WriteCommand(0x22); //

    SH1106_WriteCommand(0xDA); //--set com pins hardware configuration - CHECK
#if (SH1106_HEIGHT == 32)
    SH1106_WriteCommand(0x02);
#elif (SH1106_HEIGHT == 64)
    SH1106_WriteCommand(0x12);
#elif (SH1106_HEIGHT == 128)
    SH1106_WriteCommand(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    SH1106_WriteCommand(0xDB); //--set vcomh
    SH1106_WriteCommand(0x20); //0x20,0.77xVcc

    SH1106_WriteCommand(0x8D); //--set DC-DC enable
    SH1106_WriteCommand(0x14); //
    SH1106_SetDisplayOn(1); //--turn on SH1106 panel

    // Clear screen
    SH1106_Fill(Black);
    
    // Flush buffer to screen
    SH1106_UpdateScreen();
    
    // Set default values for screen object
    SH1106.CurrentX = 0;
    SH1106.CurrentY = 0;
    
    SH1106.Initialized = 1;
}

/* Fill the whole screen with the given color */
void SH1106_Fill(SH1106_COLOR color) {
    memset(SH1106_Buffer, (color == Black) ? 0x00 : 0xFF, sizeof(SH1106_Buffer));
}

/* Write the screenbuffer with changed to the screen */
void SH1106_UpdateScreen(void) {
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < SH1106_HEIGHT/8; i++) {
        SH1106_WriteCommand(0xB0 + i); // Set the current RAM page address.
        SH1106_WriteCommand(0x00 + SH1106_X_OFFSET_LOWER);
        SH1106_WriteCommand(0x10 + SH1106_X_OFFSET_UPPER);
        SH1106_WriteData(&SH1106_Buffer[SH1106_WIDTH*i],SH1106_WIDTH);
    }
}

/*
 * Draw one pixel in the screenbuffer
 * X => X Coordinate
 * Y => Y Coordinate
 * color => Pixel color
 */
void SH1106_DrawPixel(uint8_t x, uint8_t y, SH1106_COLOR color) {
    if(x >= SH1106_WIDTH || y >= SH1106_HEIGHT) {
        // Don't write outside the buffer
        return;
    }
   
    // Draw in the right color
    if(color == White) {
        SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] |= 1 << (y % 8);
    } else { 
        SH1106_Buffer[x + (y / 8) * SH1106_WIDTH] &= ~(1 << (y % 8));
    }
}

/*
 * Draw 1 char to the screen buffer
 * ch       => char om weg te schrijven
 * Font     => Font waarmee we gaan schrijven
 * color    => Black or White
 */
char SH1106_WriteChar(char ch, SH1106_Font_t Font, SH1106_COLOR color) {
    uint32_t i, b, j;
    
    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;
    
    // Check remaining space on current line
    if (SH1106_WIDTH < (SH1106.CurrentX + Font.width) ||
        SH1106_HEIGHT < (SH1106.CurrentY + Font.height))
    {
        // Not enough space on current line
        return 0;
    }
    
    // Use the font to write
    for(i = 0; i < Font.height; i++) {
        b = Font.data[(ch - 32) * Font.height + i];
        for(j = 0; j < Font.width; j++) {
            if((b << j) & 0x8000)  {
                SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR) color);
            } else {
                SH1106_DrawPixel(SH1106.CurrentX + j, (SH1106.CurrentY + i), (SH1106_COLOR)!color);
            }
        }
    }
    
    // The current space is now taken
    SH1106.CurrentX += Font.char_width ? Font.char_width[ch - 32] : Font.width;
    
    // Return written char for validation
    return ch;
}

/* Write full string to screenbuffer */
char SH1106_WriteString(char* str, SH1106_Font_t Font, SH1106_COLOR color) {
    while (*str) {
        if (SH1106_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        str++;
    }
    
    // Everything ok
    return *str;
}

/* Position the cursor */
void SH1106_SetCursor(uint8_t x, uint8_t y) {
    SH1106.CurrentX = x;
    SH1106.CurrentY = y;
}

/* Draw line by Bresenhem's algorithm */
void SH1106_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_COLOR color) {
    int32_t deltaX = abs(x2 - x1);
    int32_t deltaY = abs(y2 - y1);
    int32_t signX = ((x1 < x2) ? 1 : -1);
    int32_t signY = ((y1 < y2) ? 1 : -1);
    int32_t error = deltaX - deltaY;
    int32_t error2;
    
    SH1106_DrawPixel(x2, y2, color);

    while((x1 != x2) || (y1 != y2)) {
        SH1106_DrawPixel(x1, y1, color);
        error2 = error * 2;
        if(error2 > -deltaY) {
            error -= deltaY;
            x1 += signX;
        }
        
        if(error2 < deltaX) {
            error += deltaX;
            y1 += signY;
        }
    }
    return;
}

/* Draw polyline */
void SH1106_Polyline(const SH1106_VERTEX *par_vertex, uint16_t par_size, SH1106_COLOR color) {
    uint16_t i;
    if(par_vertex == NULL) {
        return;
    }

    for(i = 1; i < par_size; i++) {
        SH1106_Line(par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
    }

    return;
}

/* Convert Degrees to Radians */
static float SH1106_DegToRad(float par_deg) {
    return par_deg * (3.14f / 180.0f);
}

/* Normalize degree to [0;360] */
static uint16_t SH1106_NormalizeTo0_360(uint16_t par_deg) {
    uint16_t loc_angle;
    if(par_deg <= 360) {
        loc_angle = par_deg;
    } else {
        loc_angle = par_deg % 360;
        loc_angle = (loc_angle ? loc_angle : 360);
    }
    return loc_angle;
}

/*
 * DrawArc. Draw angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle in degree
 * sweep in degree
 */
void SH1106_DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SH1106_COLOR color) {
    static const uint8_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1,xp2;
    uint8_t yp1,yp2;
    uint32_t count;
    uint32_t loc_sweep;
    float rad;
    
    loc_sweep = SH1106_NormalizeTo0_360(sweep);
    
    count = (SH1106_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;
    while(count < approx_segments)
    {
        rad = SH1106_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sinf(rad)*radius);
        yp1 = y + (int8_t)(cosf(rad)*radius);    
        count++;
        if(count != approx_segments) {
            rad = SH1106_DegToRad(count*approx_degree);
        } else {
            rad = SH1106_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sinf(rad)*radius);
        yp2 = y + (int8_t)(cosf(rad)*radius);    
        SH1106_Line(xp1,yp1,xp2,yp2,color);
    }
    
    return;
}

/*
 * Draw arc with radius line
 * Angle is beginning from 4 quart of trigonometric circle (3pi/2)
 * start_angle: start angle in degree
 * sweep: finish angle in degree
 */
void SH1106_DrawArcWithRadiusLine(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, SH1106_COLOR color) {
    const uint32_t CIRCLE_APPROXIMATION_SEGMENTS = 36;
    float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1;
    uint8_t xp2 = 0;
    uint8_t yp1;
    uint8_t yp2 = 0;
    uint32_t count;
    uint32_t loc_sweep;
    float rad;
    
    loc_sweep = SH1106_NormalizeTo0_360(sweep);
    
    count = (SH1106_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;

    rad = SH1106_DegToRad(count*approx_degree);
    uint8_t first_point_x = x + (int8_t)(sinf(rad)*radius);
    uint8_t first_point_y = y + (int8_t)(cosf(rad)*radius);   
    while (count < approx_segments) {
        rad = SH1106_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sinf(rad)*radius);
        yp1 = y + (int8_t)(cosf(rad)*radius);    
        count++;
        if (count != approx_segments) {
            rad = SH1106_DegToRad(count*approx_degree);
        } else {
            rad = SH1106_DegToRad(loc_sweep);
        }
        xp2 = x + (int8_t)(sinf(rad)*radius);
        yp2 = y + (int8_t)(cosf(rad)*radius);    
        SH1106_Line(xp1,yp1,xp2,yp2,color);
    }
    
    // Radius line
    SH1106_Line(x,y,first_point_x,first_point_y,color);
    SH1106_Line(x,y,xp2,yp2,color);
    return;
}

/* Draw circle by Bresenhem's algorithm */
void SH1106_DrawCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,SH1106_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= SH1106_WIDTH || par_y >= SH1106_HEIGHT) {
        return;
    }

    do {
        SH1106_DrawPixel(par_x - x, par_y + y, par_color);
        SH1106_DrawPixel(par_x + x, par_y + y, par_color);
        SH1106_DrawPixel(par_x + x, par_y - y, par_color);
        SH1106_DrawPixel(par_x - x, par_y - y, par_color);
        e2 = err;

        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    return;
}

/* Draw filled circle. Pixel positions calculated using Bresenham's algorithm */
void SH1106_FillCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,SH1106_COLOR par_color) {
    int32_t x = -par_r;
    int32_t y = 0;
    int32_t err = 2 - 2 * par_r;
    int32_t e2;

    if (par_x >= SH1106_WIDTH || par_y >= SH1106_HEIGHT) {
        return;
    }

    do {
        for (uint8_t _y = (par_y + y); _y >= (par_y - y); _y--) {
            for (uint8_t _x = (par_x - x); _x >= (par_x + x); _x--) {
                SH1106_DrawPixel(_x, _y, par_color);
            }
        }

        e2 = err;
        if (e2 <= y) {
            y++;
            err = err + (y * 2 + 1);
            if (-x == y && e2 <= x) {
                e2 = 0;
            }
        }

        if (e2 > x) {
            x++;
            err = err + (x * 2 + 1);
        }
    } while (x <= 0);

    return;
}

/* Draw a rectangle */
void SH1106_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_COLOR color) {
    SH1106_Line(x1,y1,x2,y1,color);
    SH1106_Line(x2,y1,x2,y2,color);
    SH1106_Line(x2,y2,x1,y2,color);
    SH1106_Line(x1,y2,x1,y1,color);

    return;
}

/* Draw a filled rectangle */
void SH1106_FillRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SH1106_COLOR color) {
    uint8_t x_start = ((x1<=x2) ? x1 : x2);
    uint8_t x_end   = ((x1<=x2) ? x2 : x1);
    uint8_t y_start = ((y1<=y2) ? y1 : y2);
    uint8_t y_end   = ((y1<=y2) ? y2 : y1);

    for (uint8_t y= y_start; (y<= y_end)&&(y<SH1106_HEIGHT); y++) {
        for (uint8_t x= x_start; (x<= x_end)&&(x<SH1106_WIDTH); x++) {
            SH1106_DrawPixel(x, y, color);
        }
    }
    return;
}

SH1106_Error_t SH1106_InvertRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
  if ((x2 >= SH1106_WIDTH) || (y2 >= SH1106_HEIGHT)) {
    return SH1106_ERR;
  }
  if ((x1 > x2) || (y1 > y2)) {
    return SH1106_ERR;
  }
  uint32_t i;
  if ((y1 / 8) != (y2 / 8)) {
    /* if rectangle doesn't lie on one 8px row */
    for (uint32_t x = x1; x <= x2; x++) {
      i = x + (y1 / 8) * SH1106_WIDTH;
      SH1106_Buffer[i] ^= 0xFF << (y1 % 8);
      i += SH1106_WIDTH;
      for (; i < x + (y2 / 8) * SH1106_WIDTH; i += SH1106_WIDTH) {
        SH1106_Buffer[i] ^= 0xFF;
      }
      SH1106_Buffer[i] ^= 0xFF >> (7 - (y2 % 8));
    }
  } else {
    /* if rectangle lies on one 8px row */
    const uint8_t mask = (0xFF << (y1 % 8)) & (0xFF >> (7 - (y2 % 8)));
    for (i = x1 + (y1 / 8) * SH1106_WIDTH;
         i <= (uint32_t)x2 + (y2 / 8) * SH1106_WIDTH; i++) {
      SH1106_Buffer[i] ^= mask;
    }
  }
  return SH1106_OK;
}

/* Draw a bitmap */
void SH1106_DrawBitmap(uint8_t x, uint8_t y, const unsigned char* bitmap, uint8_t w, uint8_t h, SH1106_COLOR color) {
    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) {
        return;
    }

    for (uint8_t j = 0; j < h; j++, y++) {
        for (uint8_t i = 0; i < w; i++) {
            if (i & 7) {
                byte <<= 1;
            } else {
                byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }

            if (byte & 0x80) {
                SH1106_DrawPixel(x + i, y, color);
            }
        }
    }
    return;
}

void SH1106_SetContrast(const uint8_t value) {
    const uint8_t kSetContrastControlRegister = 0x81;
    SH1106_WriteCommand(kSetContrastControlRegister);
    SH1106_WriteCommand(value);
}

void SH1106_SetDisplayOn(const uint8_t on) {
    uint8_t value;
    if (on) {
        value = 0xAF;   // Display on
        SH1106.DisplayOn = 1;
    } else {
        value = 0xAE;   // Display off
        SH1106.DisplayOn = 0;
    }
    SH1106_WriteCommand(value);
}

uint8_t SH1106_GetDisplayOn() {
    return SH1106.DisplayOn;
}
