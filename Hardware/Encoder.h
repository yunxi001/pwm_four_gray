#ifndef _ENCODER_H
#define _ENCODER_H


void SystemClock_Config(void);       //ÅäÖÃÏµÍ³Ê±ÖÓ72M
void GET_Encoder(int *recv_buf); //»ñÈ¡µç»úµ±Ç°±àÂëÖµ
void RESET_Encoder(void);         //Çå³ıµç»ú±àÂëÖµ
void Encoder_EXTI_Init(void);        //³õÊ¼»¯±àÂëÆ÷


float App_Encoder_Pos_1(void);   //»ñÈ¡×óÇ°µç»úµ±Ç°½Ç¶È
float App_Encoder_Pos_2(void);   //»ñÈ¡Òµç»úµ±Ç°½Ç¶È
float App_Encoder_Pos_3(void);   //»ñÈ¡ÓÒºó µç»úµ±Ç°½Ç¶È
float App_Encoder_Pos_4(void);   //»ñÈ¡ÓÓÒµç»úµ±Ç°½Ç¶È


#endif
