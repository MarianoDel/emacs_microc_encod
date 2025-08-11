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

#include "usart.h"

// Module Exported Macros ------------------------------------------------------
#define UsartMainConfig()    Usart3Config()
#define UsartMainSend(X)    Usart3Send(X)
#define UsartMainReadBuffer(X,Y)    Usart3ReadBuffer(X,Y)
#define UsartMainHaveData()    Usart3HaveData()
#define UsartMainHaveDataReset()    Usart3HaveDataReset()
#define UsartMainShutdownn()    Usart3Shutdown()

#define UsartEncoder1Config()    Usart1Config()
#define UsartEncoder1Send(X)    Usart1Send(X)
#define UsartEncoder1SendUnsigned(X,Y)    Usart1SendUnsigned(X,Y)
#define UsartEncoder1ReadBuffer(X,Y)    Usart1ReadBuffer(X,Y)
#define UsartEncoder1HaveData()    Usart1HaveData()
#define UsartEncoder1HaveDataReset()    Usart1HaveDataReset()
#define UsartEncoder1Shutdown()    Usart1Shutdown()

#define UsartEncoder2Config()    Usart2Config()
#define UsartEncoder2Send(X)    Usart2Send(X)
#define UsartEncoder2SendUnsigned(X,Y)    Usart2SendUnsigned(X,Y)
#define UsartEncoder2ReadBuffer(X,Y)    Usart2ReadBuffer(X,Y)
#define UsartEncoder2HaveData()    Usart2HaveData()
#define UsartEncoder2HaveDataReset()    Usart2HaveDataReset()
#define UsartEncoder2Shutdownn()    Usart2Shutdown()

#define UsartEncoder3Config()    Uart4Config()
#define UsartEncoder3Send(X)    Uart4Send(X)
#define UsartEncoder3SendUnsigned(X,Y)    Uart4SendUnsigned(X,Y)
#define UsartEncoder3ReadBuffer(X,Y)    Uart4ReadBuffer(X,Y)
#define UsartEncoder3HaveData()    Uart4HaveData()
#define UsartEncoder3HaveDataReset()    Uart4HaveDataReset()
#define UsartEncoder3Shutdownn()    Uart4Shutdown()

#define UsartEncoder4Config()    Uart5Config()
#define UsartEncoder4Send(X)    Uart5Send(X)
#define UsartEncoder4SendUnsigned(X,Y)    Uart5SendUnsigned(X,Y)
#define UsartEncoder4ReadBuffer(X,Y)    Uart5ReadBuffer(X,Y)
#define UsartEncoder4HaveData()    Uart5HaveData()
#define UsartEncoder4HaveDataReset()    Uart5HaveDataReset()
#define UsartEncoder4Shutdownn()    Uart5Shutdown()



#endif     /* _USART_CHANNELS_H_ */
