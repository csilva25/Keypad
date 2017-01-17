
#include "stm32F30x.h"
#include "STM32f3_discovery.h"
#include "stm32f30x_gpio.h"
#include  "stm32f30x_i2c.h"
#include "main.h"
#include  "stdio.h"
#include  "string.h"

RCC_ClocksTypeDef 			RCC_Clocks;
GPIO_InitTypeDef        GPIO_InitStructure;

void    Init_GPIOE(void);
void    Init_Keypad(void);
void    Init_LCD(void);
void  LCD_cursor(void);
void  LCD_setpos(int row, int col);

void  LCD_write(int,int, char); // hor, vert, charactor
void  LCD_clear(void);
void  LCD_contrast(int);                // Contrast level = 1..50
void  LCD_backlight(int);               // Backlight level = 1..8
void Delay(uint32_t nTime);

unsigned  int   tick_count = 0;
char strDisp[20] ;  
volatile int     ButtonPressed = 0;
static __IO uint32_t TimingDelay;

char        message[16];
char        ready_msg[] = "Cristian Silva";

int main(void) {
		int i=0,l=0,j=0,r=0,c=0,a=0;
		char myArray[4][3]  = {{ '1', '2', '3'},{'4', '5', '6'},{'7', '8', '9'},{'*', '0', '#'}};
    Init_GPIOE();	Init_LCD(); LCD_cursor();
for (l=0; l < strlen(ready_msg); l++) LCD_write(0, l, ready_msg[l]);                 
   
    while (!ButtonPressed)
		{
		if (a==8) a=0;
		ButtonPressed = 0;
		GPIOE -> ODR = (0x0100 << a++ );
		Delay(150);
    }   
		LCD_clear();
		
while (1){
	LCD_setpos(r,c);
	//---LED---
			if (a > 7) a=0;
			if (a<0) a=7;
 //----------------------------------FOR ROW 0-3 ------------------------------------		
for (i=0; i<4;i++)
		{
			GPIOC -> ODR = (0x0100 << i);
			Delay(10);
//----------------------------------FOR ROW 0-3 AND COL 3-----------------------------						
		if (i==0 &&  (GPIOB->IDR & 0x1000 << 3 ) != 0 )		{					
								a=(a+4)%8;		
							if (r>0) r=0;
							else r=1;
								Delay(200);
								GPIOE -> ODR = (0x0100 << a );
						}	
		if (i==1 &&  (GPIOB->IDR & 0x1000 << 3 ) != 0 )	{
								a=(a+4)%8;		
							if (r>0)	r=0;
							else r=1;
								Delay(200);
							GPIOE -> ODR = (0x0100 << a );
						}
		if (i==2 &&  (GPIOB->IDR & 0x1000 << 3 ) != 0 )	{
							a+=1;
						if (a>7)	a=0;
						if (c > 14)	c=0;
						else	c++;
							Delay(200);
							GPIOE -> ODR = (0x0100 << a );
						}	
		if (i==3 &&  (GPIOB->IDR & 0x1000 << 3 ) != 0 )	{
							a-=1;
							if(a<0) a=7;
							if (c < 1) c=15;
							else	c--;
								Delay(200);
								GPIOE -> ODR = (0x0100 << a );					
						}		
//----------------------------------COL 0-2-----------------------------									
		for (j=0; j<3;j++)	{
				if ((GPIOB->IDR & 0x1000 << j ) != 0)	{
						if (c>15 && r==1)	{
					c=0;	r=0;
					LCD_clear();
				}
				if (c>15 && r==0)	{
					c=0;r=1;
				}
				LCD_write(r, c,myArray [i][j]);
				Delay(150);
				c++;	
			}

			}
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~       
       
void Init_GPIOE() {
  
		EXTI_InitTypeDef   EXTI_InitStructure;
		GPIO_InitTypeDef   GPIO_InitStructure;
		NVIC_InitTypeDef   NVIC_InitStructure;
			 
			
		/* SysTick end of count event each 1ms */
		RCC_GetClocksFreq(&RCC_Clocks);
		SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);


		/* GPIOE Periph clock enable */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

		/* Configure PE14 and PE15 in output pushpull mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12
			| GPIO_Pin_11| GPIO_Pin_10| GPIO_Pin_9| GPIO_Pin_8;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;              
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             //GPIO_PuPd_NOPULL
		GPIO_Init(GPIOE, &GPIO_InitStructure);
		
			/* Enable GPIOA clock */
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

		/* Configure PA0 pin as input floating */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
		
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

		/* Configure PE14 and PE15 in output pushpull mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;              
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;             //GPIO_PuPd_NOPULL
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
				RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

		/* Configure PE14 and PE15 in output pushpull mode */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;              
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;             //GPIO_PuPd_NOPULL
		GPIO_Init(GPIOC, &GPIO_InitStructure);


		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

		/* Connect EXTI0 Line to PA0 pin */
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
		

		/* Configure EXTI0 line */
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set EXTI0 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

}




void Init_LCD(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		I2C_InitTypeDef  I2C_InitStructure;

		RCC_I2CCLKConfig(RCC_I2C2CLK_SYSCLK);  

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);  

		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_4);
		GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_4);
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
		GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		I2C_DeInit(I2C2);
		I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;  
		I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
		I2C_InitStructure.I2C_DigitalFilter = 0x00;
		I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
		I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
		I2C_InitStructure.I2C_Timing = 0xC062121F; 

		I2C_Init(I2C2, &I2C_InitStructure);  
		I2C_Cmd(I2C2, ENABLE);
		LCD_clear();
		
}


//
//      Set LCD Cursor Position - row = 0,1  col = 0..15
//
void  LCD_setpos(int row, int col) {
        
        // Move to sepcified row, col
  
        //while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
          
        I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0xFE);
               
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0x45);
        
  
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);        
        if (!row)               // if row == 0
            I2C_SendData(I2C2, col);
         else                  // else row asumed to be 1
            I2C_SendData(I2C2, (0x40 + col));       
         
        
}         

//
//      Turn on LCD Cursor - Makes cursor visible
//
void  LCD_cursor(void) {
  
        //while(I2C_GetFlagStatus(I2C2, I2C_ISR_BUSY) != RESET);
          
        I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0xFE);
               
        while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);
      
        I2C_SendData(I2C2, 0x47);  	// underline   (Blinking = 0x4B)
      
        Delay(20);
}         


void  LCD_write(int row, int col, char data) {
          
		// Move to sepcified row, col
			
		I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0xFE);
					 
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0x45);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);        
		if (!row)               // if row == 0
				I2C_SendData(I2C2, col);
		 else                  // else row asumed to be 1
				I2C_SendData(I2C2, (0x40 + col));       
		 
		I2C_TransferHandling(I2C2, 0x50 , 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
			
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);                        
		I2C_SendData(I2C2, data);

}         


//
//      Set LCD Contrast - Level should be 1..50 (Seems to work best if > 35)
//

void  LCD_contrast(int level) {
     
		I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0xFE);
					 
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0x52);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, level); 

		Delay(20);
}         

//
//      Set LCD Backlight - Level should be 1..8 (Seems to work best if > 1)
//

void  LCD_backlight(int level) {
  

		I2C_TransferHandling(I2C2, 0x50 , 3, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0xFE);
				 
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0x53);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, level);
			
		Delay(20);
}         


void  LCD_clear() {
       
		I2C_TransferHandling(I2C2, 0x50 , 2, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);

		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0xFE);
					 
		while(I2C_GetFlagStatus(I2C2, I2C_ISR_TXIS) == RESET);

		I2C_SendData(I2C2, 0x51);

		Delay(20);
}         

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
  
}


void SysTick_Handler(void)
{
     TimingDelay_Decrement();
     
     tick_count++;
}


void EXTI0_IRQHandler(void)
{

	if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
  {
		
		
     
    ButtonPressed = 1;
		
    /* Clear the EXTI line 0 pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }    

}

