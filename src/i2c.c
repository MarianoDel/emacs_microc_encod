//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### I2C.C #################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "i2c.h"
#include "stm32f10x.h"
#include "hard.h"


// Private Types Constants and Macros ------------------------------------------
#define RCC_I2C1_CLK    (RCC->APB1ENR & 0x00200000)
#define RCC_I2C1_CLKEN    (RCC->APB1ENR |= 0x00200000)
#define RCC_I2C1_CLKDIS    (RCC->APB1ENR &= ~0x00200000)

#define RCC_I2C2_CLK    (RCC->APB1ENR & 0x00400000)
#define RCC_I2C2_CLKEN    (RCC->APB1ENR |= 0x00400000)
#define RCC_I2C2_CLKDIS    (RCC->APB1ENR &= ~0x00400000)

#define RCC_I2C1_RST_SET    (RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST)
#define RCC_I2C1_RST_REL    (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C1RST))
#define RCC_I2C2_RST_SET    (RCC->APB1RSTR |= RCC_APB1RSTR_I2C2RST)
#define RCC_I2C2_RST_REL    (RCC->APB1RSTR &= ~(RCC_APB1RSTR_I2C2RST))

#ifdef I2C_WITH_INTS
typedef enum {
    wait_start,
    wait_addr,
    sending_bytes

} i2c1_int_states_e;

typedef struct {
    unsigned char slave_addr;
    unsigned char * pbuff;
    unsigned short buff_size;
    
} i2c1_int_pckt_st;
#endif

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Funtions -------------------------------------------------------------
////////////////////
// I2C1 FUNCTIONS //
////////////////////
#ifdef I2C_USE_I2C1
void I2C1_Init (void)
{
    // Clock for Peripheral
    if (!RCC_I2C1_CLK)
        RCC_I2C1_CLKEN;

    RCC_I2C1_RST_SET;
    RCC_I2C1_RST_REL;
    
    // Speed and Port options
    // APB freq = PCKL1 freq = 32MHz
    I2C1->CR2 = 32;
    // I2C1->CR2 = 20;    

    // I2C1->CCR = I2C_CCR_FS | I2C_CCR_DUTY | 0x0008;    //160KHz duty 16/9
    // I2C1->CCR = I2C_CCR_FS | 0x0008;    //160KHz duty 2 (no funciona)
    // I2C1->CCR = I2C_CCR_FS | I2C_CCR_DUTY | 0x0004;    //320KHz
    // I2C1->CCR = I2C_CCR_FS | 0x0004;    //320KHz
    // I2C1->CCR = I2C_CCR_FS | 40;    //270KHz duty 2
    I2C1->CCR = I2C_CCR_FS | 27;    //270KHz duty 2

    // I2C1->TRISE = 33;    // 450ns
    // I2C1->TRISE = 66;    // 600ns
    I2C1->TRISE = 16;    // 500ns

    // I2C1 remap to PB8 PB9 SCL SDA
    // unsigned int temp = 0;
    // temp = AFIO->MAPR;
    // temp |= AFIO_MAPR_I2C1_REMAP;
    // AFIO->MAPR = temp;

    I2C1->CR1 = I2C_CR1_PE;

#ifdef I2C1_WITH_INTS
    // I2C1->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN;
    I2C1->CR2 |= I2C_CR2_ITEVTEN;    
    // Int and priority
    NVIC_EnableIRQ(I2C1_EV_IRQn);
    NVIC_SetPriority(I2C1_EV_IRQn, 8);
#endif
}


void I2C1_Shutdown (void)
{
    // Clock for Peripheral
    RCC_I2C1_CLKDIS;
}


void I2C1_OwnAddress (unsigned char own_addr)
{
    I2C1->OAR1 = own_addr;
}


void I2C1_Ack (unsigned char set_reset)
{
    if (set_reset)
        I2C1->CR1 |= I2C_CR1_ACK;
    else
        I2C1->CR1 &= ~I2C_CR1_ACK;
}


void I2C1_SendByte (unsigned char addr, unsigned char data)
{
    // wait no busy line
    while (I2C1->SR2 & I2C_SR2_BUSY);

    // send START
    I2C1->CR1 |= I2C_CR1_START;
    // wait for START be sent
    while (!(I2C1->SR1 & I2C_SR1_SB));
    I2C1->SR1 &= ~I2C_SR1_AF;    // reset NACK
    // send slave addr
    I2C1->DR = addr;

    // wait for slave addr be sent
    unsigned short dummy = 0;
    unsigned char error = 1;
    do {
        if (I2C1->SR1 & I2C_SR1_ADDR)
        {
            dummy = I2C1->SR2;    //dummy read to clear ADDR
            dummy++;
        }
            
        if ((I2C1->SR1 & I2C_SR1_AF) ||
            (I2C1->SR1 & I2C_SR1_TIMEOUT))
        {
            error = 0;
            I2C1->CR1 |= I2C_CR1_STOP;
            return;
        }
            
    } while (error);


    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->DR = data;

    // wait for send STOP
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->CR1 |= I2C_CR1_STOP;

}


unsigned char I2C1_SendAddr (unsigned char addr)
{
    // wait no busy line
    while (I2C1->SR2 & I2C_SR2_BUSY);

    // send START
    I2C1->CR1 |= I2C_CR1_START;
    // wait for START be sent
    while (!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->SR1 &= ~I2C_SR1_AF;    // reset NACK
    I2C1->DR = addr;

    // wait for slave addr be sent
    unsigned short dummy = 0;
    unsigned char error = 1;
    do {
        if (I2C1->SR1 & I2C_SR1_ADDR)
        {
            dummy = I2C1->SR2;    //dummy read to clear ADDR
            error = 0;
            dummy = 1;
        }
            
        if ((I2C1->SR1 & I2C_SR1_AF) ||
            (I2C1->SR1 & I2C_SR1_TIMEOUT))
        {
            error = 0;
            dummy = 2;
        }
            
    } while (error);
        
    I2C1->CR1 |= I2C_CR1_STOP;
    return (unsigned char) dummy;
}


// no ints
unsigned char I2C1_SendMultiByte (unsigned char *pdata, unsigned char addr, unsigned short size)
{
    // check no master
    // while (I2C1->SR2 & I2C_SR2_MSL);

    // // wait no busy line
    while (I2C1->SR2 & I2C_SR2_BUSY);
    // while (I2C1->SR2 & I2C_SR2_BUSY)        
    // {
    //     Led_On();
    //     Wait_ms(1);
    //     Led_Off();
    //     Wait_ms(9);
    // }
    
    // send START
    I2C1->CR1 |= I2C_CR1_START;
    // wait for START be sent
    while (!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->SR1 &= ~I2C_SR1_AF;    // reset NACK

    // send slave addr is right aligned, addr as we get
    I2C1->DR = addr;
    // I2C1->DR = (addr << 1) & 0xFE;

    // wait for slave addr be sent
    unsigned short dummy = 0;
    unsigned char error = 1;
    do {
        if (I2C1->SR1 & I2C_SR1_ADDR)
        {
            dummy = I2C1->SR2;    //dummy read to clear ADDR
            dummy++;
            error = 0;
        }
            
        if ((I2C1->SR1 & I2C_SR1_AF) ||
            (I2C1->SR1 & I2C_SR1_TIMEOUT))
        {
            error = 0;
            I2C1->CR1 |= I2C_CR1_STOP;
            return 1;
        }
            
    } while (error);


    for (int i = 0; i < size; i++)
    {
        while (!(I2C1->SR1 & I2C_SR1_TXE));
        I2C1->DR = *(pdata + i);
    }

    // wait to send STOP
    while (!(I2C1->SR1 & I2C_SR1_TXE));
    I2C1->CR1 |= I2C_CR1_STOP;
    
    // while (I2C1->SR2 & I2C_SR2_MSL);
    return 0;
}


#ifdef I2C1_WITH_INTS
i2c1_int_pckt_st i2c1_int_pckt;
volatile unsigned char i2c1_int_active = 0;
// with ints
void I2C1_SendMultiByte_Int (unsigned char addr, unsigned char *pdata, unsigned short size)
{
    if (!(I2C1->SR2 & I2C_SR2_MSL))
    {
        i2c1_int_active = 1;
        i2c1_int_pckt.slave_addr = addr;
        i2c1_int_pckt.pbuff = pdata;
        i2c1_int_pckt.buff_size = size;
        // send START
        I2C1->CR1 |= I2C_CR1_START;
    }
}


unsigned char I2C1_CheckEnded_Int (void)
{
    if ((I2C1->SR2 & I2C_SR2_MSL) ||
        (i2c1_int_active))
        return 0;
    else
        return 1;
}


i2c1_int_states_e i2c1_int_state = wait_start;
volatile unsigned short i2c1_pdata_cnt = 0;
void I2C1_EV_IRQHandler (void)
{
    unsigned char error_send_stop = 0;
    
    switch (i2c1_int_state)
    {
    case wait_start:
        if (I2C1->SR1 & I2C_SR1_SB)
        {
            // send addr
            I2C1->DR = (i2c1_int_pckt.slave_addr << 1) & 0xFE;
            I2C1->SR1 &= ~I2C_SR1_AF;    // reset NACK
            
            i2c1_pdata_cnt = 0;
            i2c1_int_state++;
        }    // else discard silently
        break;

    case wait_addr:
        if (I2C1->SR1 & I2C_SR1_ADDR)
        {
            unsigned short dummy = I2C1->SR2;    //dummy read to clear ADDR
            dummy += 1;
            if (I2C1->SR1 & I2C_SR1_TXE)
            {
                I2C1->DR = *(i2c1_int_pckt.pbuff + i2c1_pdata_cnt);
                i2c1_pdata_cnt++;
                i2c1_int_state++;
            }
            else    // protocol error, send stop
                error_send_stop = 1;

        }
        else    // protocol error, send stop
            error_send_stop = 1;
        
        break;

    case sending_bytes:
        if (I2C1->SR1 & I2C_SR1_TXE)
        {
            if (i2c1_pdata_cnt < i2c1_int_pckt.buff_size)
            {
                I2C1->DR = *(i2c1_int_pckt.pbuff + i2c1_pdata_cnt);
                i2c1_pdata_cnt++;
            }
            else    // transmittion end, send stop
                error_send_stop = 1;
            
        }
        else    // protocol error, send stop
            error_send_stop = 1;
        
        break;

    }

    if (error_send_stop)
    {
        I2C1->CR1 |= I2C_CR1_STOP;
        i2c1_int_state = wait_start;
        i2c1_int_active = 0;
    }
}
#endif    //I2C1_WITH_INTS

#endif    //I2C_USE_I2C1

////////////////////
// I2C2 FUNCTIONS //
////////////////////
#ifdef I2C_USE_I2C2
void I2C2_Init (void)
{
    // Clock for Peripheral
    if (!RCC_I2C2_CLK)
        RCC_I2C2_CLKEN;

    RCC_I2C2_RST_SET;
    RCC_I2C2_RST_REL;
    
    // Speed and Port options
    // APB freq = PCKL1 freq = 32MHz
    I2C2->CR2 = 32;
    // I2C2->CR2 = 20;    

    // I2C2->CCR = I2C_CCR_FS | I2C_CCR_DUTY | 0x0008;    //160KHz duty 16/9
    // I2C2->CCR = I2C_CCR_FS | 0x0008;    //160KHz duty 2 (no funciona)
    // I2C2->CCR = I2C_CCR_FS | I2C_CCR_DUTY | 0x0004;    //320KHz
    // I2C2->CCR = I2C_CCR_FS | 0x0004;    //320KHz
    // I2C2->CCR = I2C_CCR_FS | 40;    //270KHz duty 2
    I2C2->CCR = I2C_CCR_FS | 27;    //270KHz duty 2

    // I2C2->TRISE = 33;    // 450ns
    // I2C2->TRISE = 66;    // 600ns
    I2C2->TRISE = 16;    // 500ns

    // I2C2 remap to PB8 PB9 SCL SDA
    // unsigned int temp = 0;
    // temp = AFIO->MAPR;
    // temp |= AFIO_MAPR_I2C2_REMAP;
    // AFIO->MAPR = temp;

    I2C2->CR1 = I2C_CR1_PE;

#ifdef I2C2_WITH_INTS
    // I2C2->CR2 |= I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN;
    I2C2->CR2 |= I2C_CR2_ITEVTEN;    
    // Int and priority
    NVIC_EnableIRQ(I2C2_EV_IRQn);
    NVIC_SetPriority(I2C2_EV_IRQn, 8);
#endif
}


void I2C2_SendByte (unsigned char addr, unsigned char data)
{
    // wait no busy line
    while (I2C2->SR2 & I2C_SR2_BUSY);

    // send START
    I2C2->CR1 |= I2C_CR1_START;
    // wait for START be sent
    while (!(I2C2->SR1 & I2C_SR1_SB));
    I2C2->SR1 &= ~I2C_SR1_AF;    // reset NACK
    // send slave addr
    I2C2->DR = addr;

    // wait for slave addr be sent
    unsigned short dummy = 0;
    unsigned char error = 1;
    do {
        if (I2C2->SR1 & I2C_SR1_ADDR)
        {
            dummy = I2C2->SR2;    //dummy read to clear ADDR
            dummy++;
        }
            
        if ((I2C2->SR1 & I2C_SR1_AF) ||
            (I2C2->SR1 & I2C_SR1_TIMEOUT))
        {
            error = 0;
            I2C2->CR1 |= I2C_CR1_STOP;
            return;
        }
            
    } while (error);


    while (!(I2C2->SR1 & I2C_SR1_TXE));
    I2C2->DR = data;

    // wait for send STOP
    while (!(I2C2->SR1 & I2C_SR1_TXE));
    I2C2->CR1 |= I2C_CR1_STOP;

}


unsigned char I2C2_SendAddr (unsigned char addr)
{
    // wait no busy line
    while (I2C2->SR2 & I2C_SR2_BUSY);

    // send START
    I2C2->CR1 |= I2C_CR1_START;
    // wait for START be sent
    while (!(I2C2->SR1 & I2C_SR1_SB));

    I2C2->SR1 &= ~I2C_SR1_AF;    // reset NACK
    I2C2->DR = addr;

    // wait for slave addr be sent
    unsigned short dummy = 0;
    unsigned char error = 1;
    do {
        if (I2C2->SR1 & I2C_SR1_ADDR)
        {
            dummy = I2C2->SR2;    //dummy read to clear ADDR
            error = 0;
            dummy = 1;
        }
            
        if ((I2C2->SR1 & I2C_SR1_AF) ||
            (I2C2->SR1 & I2C_SR1_TIMEOUT))
        {
            error = 0;
            dummy = 2;
        }
            
    } while (error);
        
    I2C2->CR1 |= I2C_CR1_STOP;
    return (unsigned char) dummy;
}


// no ints
unsigned char I2C2_SendMultiByte (unsigned char *pdata, unsigned char addr, unsigned short size)
{
    // check no master
    // while (I2C2->SR2 & I2C_SR2_MSL);

    // // wait no busy line
    while (I2C2->SR2 & I2C_SR2_BUSY);
    
    // send START
    I2C2->CR1 |= I2C_CR1_START;
    // wait for START be sent
    while (!(I2C2->SR1 & I2C_SR1_SB));

    I2C2->SR1 &= ~I2C_SR1_AF;    // reset NACK

    // send slave addr is right aligned, addr as we get
    I2C2->DR = addr;
    // I2C2->DR = (addr << 1) & 0xFE;

    // wait for slave addr be sent
    unsigned short dummy = 0;
    unsigned char error = 1;
    do {
        if (I2C2->SR1 & I2C_SR1_ADDR)
        {
            dummy = I2C2->SR2;    //dummy read to clear ADDR
            dummy++;
            error = 0;
        }
            
        if ((I2C2->SR1 & I2C_SR1_AF) ||
            (I2C2->SR1 & I2C_SR1_TIMEOUT))
        {
            error = 0;
            I2C2->CR1 |= I2C_CR1_STOP;
            return 1;
        }
            
    } while (error);


    for (int i = 0; i < size; i++)
    {
        while (!(I2C2->SR1 & I2C_SR1_TXE));
        I2C2->DR = *(pdata + i);
    }

    // wait to send STOP
    while (!(I2C2->SR1 & I2C_SR1_TXE));
    I2C2->CR1 |= I2C_CR1_STOP;
    
    // while (I2C2->SR2 & I2C_SR2_MSL);
    return 0;
}


#endif    //I2C2_USE_I2C2

//--- end of file ---//
