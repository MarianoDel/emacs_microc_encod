//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### USART_CHANNELS.H ######################
//---------------------------------------------

#ifndef _USART_CHANNELS_H_
#define _USART_CHANNELS_H_


// Module Exported Macros ------------------------------------------------------
#define UsartChannel1Config()    Usart2Config()
#define UsartChannel1Send(X)    Usart2Send(X)
#define UsartChannel1ReadBuffer(X,Y)    Usart2ReadBuffer(X,Y)
#define UsartChannel1HaveData()    Usart2HaveData()
#define UsartChannel1HaveDataReset()    Usart2HaveDataReset()
#define UsartChannel1Shutdownn()    Usart2Shutdown()

#define UsartChannel2Config()    Usart3Config()
#define UsartChannel2Send(X)    Usart3Send(X)
#define UsartChannel2ReadBuffer(X,Y)    Usart3ReadBuffer(X,Y)
#define UsartChannel2HaveData()    Usart3HaveData()
#define UsartChannel2HaveDataReset()    Usart3HaveDataReset()
#define UsartChannel2Shutdownn()    Usart3Shutdown()

#define UsartChannel3Config()    Uart4Config()
#define UsartChannel3Send(X)    Uart4Send(X)
#define UsartChannel3ReadBuffer(X,Y)    Uart4ReadBuffer(X,Y)
#define UsartChannel3HaveData()    Uart4HaveData()
#define UsartChannel3HaveDataReset()    Uart4HaveDataReset()
#define UsartChannel3Shutdownn()    Uart4Shutdown()

#define UsartChannel4Config()    Uart5Config()
#define UsartChannel4Send(X)    Uart5Send(X)
#define UsartChannel4ReadBuffer(X,Y)    Uart5ReadBuffer(X,Y)
#define UsartChannel4HaveData()    Uart5HaveData()
#define UsartChannel4HaveDataReset()    Uart5HaveDataReset()
#define UsartChannel4Shutdownn()    Uart5Shutdown()

#define UsartRpiConfig()    Usart1Config()
#define UsartRpiSend(X)    Usart1Send(X)
#define UsartRpiReadBuffer(X,Y)    Usart1ReadBuffer(X,Y)
#define UsartRpiHaveData()    Usart1HaveData()
#define UsartRpiHaveDataReset()    Usart1HaveDataReset()
#define UsartRpiShutdown()    Usart1Shutdown()


#endif     /* _USART_CHANNELS_H_ */
