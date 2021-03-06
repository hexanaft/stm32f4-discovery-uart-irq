#include "uart_IRQ.h"
#include "discoveryf4utils.h"
//initialize buffers
volatile FIFO_TypeDef UART_Rx;

//******************************************************************************

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Configure the NVIC Preemption Priority Bits */
	//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = DBG_UART_DMA_STREAM_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = DBG_UART_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0D;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0D;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/**************************************************************************************/

void RCC_Configuration(void)
{
	/* --------------------------- System Clocks Configuration -----------------*/
	/* DBG_UART clock enable */
	RCC_DBG_UART_CLK_INIT(RCC_DBG_UART_CLK, ENABLE);

	/* GPIOD clock enable */
	RCC_DBG_UART_GPIO_CLK_INIT(RCC_DBG_UART_GPIO_CLK, ENABLE);
	
	/* dma clock enable */
	RCC_DBG_UART_DMA_CLK_INIT(RCC_DBG_UART_DMA_CLK, ENABLE);
}

/**************************************************************************************/

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*-------------------------- GPIO Configuration ----------------------------*/
	GPIO_InitStructure.GPIO_Pin = DBG_UART_TX_PIN | DBG_UART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DBG_UART_TX_GPIO_PORT, &GPIO_InitStructure);
	GPIO_Init(DBG_UART_RX_GPIO_PORT, &GPIO_InitStructure);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(DBG_UART_TX_GPIO_PORT, DBG_UART_TX_SOURCE, DBG_UART_GPIO_AF);  // DBG_UART_TX
	GPIO_PinAFConfig(DBG_UART_RX_GPIO_PORT, DBG_UART_RX_SOURCE, DBG_UART_GPIO_AF);  // DBG_UART_RX
}

/**************************************************************************************/

void DBG_UART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

	/* USARTx configuration ------------------------------------------------------*/
	/* USARTx configured as follow:
		- BaudRate = 115200 baud
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control disabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(DBG_UART, &USART_InitStructure);

	USART_Cmd(DBG_UART, ENABLE);
	
	//disable Transmit Data Register empty interrupt
//	USART_ITConfig(DBG_UART, USART_IT_TXE, DISABLE);
	//enable Receive Data register not empty interrupt
	USART_ITConfig(DBG_UART, USART_IT_RXNE, ENABLE);
}

/**************************************************************************************/

void UART_DMA_Receive_Init( void )
{
	
}

void UART_DMA_Receive( void )
{
	
}

bool UART_DMA_Receive_Available( void )
{
	return true;
}

/**************************************************************************************/

void UART_Configuration(void)
{
	BufferInit(&UART_Rx);
	
 	NVIC_Configuration(); /* Interrupt Config */
 	RCC_Configuration();
 	GPIO_Configuration();
	DBG_UART_Configuration();
}

// /**************************************************************************************/

// uint8_t UART_ReceiveByte( void )
// {
// 	uint8_t byte;
// 	while(USART_GetFlagStatus(DBG_UART, USART_IT_RXNE) == RESET);	
// 	byte = (uint8_t)USART_ReceiveData(DBG_UART);
// 	return byte;
// }

// /**************************************************************************************/

 uint16_t UART_ReceiveBuf( uint8_t *Data )
 {
 	uint16_t i = 0;
 	uint8_t byte = 0;
 	
 	while ( ! BufferIsEmpty(&UART_Rx) )
 	{
 		BufferGet(&UART_Rx,&byte);
 		Data[i++] = byte;
 	}
 	return i;
 }

// /**************************************************************************************/

// void UART_TransmitByte(uint8_t byte)
// {
// 	while(USART_GetFlagStatus(DBG_UART, USART_FLAG_TXE) == RESET);	
// 	USART_SendData(DBG_UART,  (uint16_t)( byte ) );
// }

// /**************************************************************************************/

// void UART_TransmitByteIT(uint8_t byte)
// {
// 	//put char to the buffer
// 	BufferPut(&UART_Tx, byte);
// 	//enable Transmit Data Register empty interrupt
// 	USART_ITConfig(DBG_UART, USART_IT_TXE, ENABLE);
// }

// /**************************************************************************************/

// void UART_TransmitBuf( uint16_t DataSize, const uint8_t *Data )
// {
// 	uint16_t i=0;
// 	for( i=0;i<DataSize;i++)
// 	{
// 		UART_TransmitByteIT( *Data++ );
// 	}
// }

// /**************************************************************************************/

 void DBG_UART_IRQHandler(void)
 {
 	uint8_t	byte;
 	//if Receive interrupt
 	if (USART_GetITStatus(DBG_UART, USART_IT_RXNE) == SET)
 	{
		//USART_ClearFlag(DBG_UART, USART_IT_RXNE);
 		byte = (uint8_t)USART_ReceiveData(DBG_UART);
 		//put char to the buffer
 		BufferPut(&UART_Rx, byte);
 		STM_EVAL_LEDToggle(LED_GREEN);
		//UART_TransmitBuf_DMA((uint8_t*)"QQQ",3);
 	}
// 	if (USART_GetITStatus(DBG_UART, USART_IT_TXE) != RESET)
// 	{
// 		//STM_EVAL_LEDToggle(LED_BLUE);
// 		
// 		if (BufferGet(&UART_Tx, &byte) == SUCCESS)//if buffer read
// 		{
// 			USART_SendData(DBG_UART, byte);
// 		}
// 		else//if buffer empty
// 		{
// 			//disable Transmit Data Register empty interrupt
// 			USART_ITConfig(DBG_UART, USART_IT_TXE, DISABLE);
// 		}
// 	}
 }

/**************************************************************************************/

// void UART_TransmitBuf_DMA( const uint8_t *buf, uint16_t len )
// {
// 	DMA_InitTypeDef DMA_InitStructure;

// // 	DMA_DeInit(DBG_UART_DMA_STREAM_RX);
// // 	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(DBG_UART->DR);
// // 	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) buf;
// // 	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
// // 	DMA_InitStructure.DMA_BufferSize = len;
// // 	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// // 	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// // 	DMA_InitStructure.DMA_PeripheralDataSize =
// // 					DMA_PeripheralDataSize_Byte;
// // 	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// // 	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// // 	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
// // 	//DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// // 	DMA_Init(DBG_UART_DMA_STREAM_RX, &DMA_InitStructure);

// 	DMA_DeInit(DBG_UART_DMA_STREAM_TX);
// 	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(DBG_UART->DR);
// 	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) buf;
// 	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
// 	DMA_InitStructure.DMA_BufferSize = len;
// 	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
// 	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
// 	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
// 	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
// 	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
// 	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
// //	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
// 	DMA_Init(DBG_UART_DMA_STREAM_TX, &DMA_InitStructure);

// 	USART_DMACmd(DBG_UART, USART_DMAReq_Tx, ENABLE); // | USART_DMAReq_Rx
// 	DMA_Cmd(DBG_UART_DMA_STREAM_TX, ENABLE);
// //	DMA_Cmd(DBG_UART_DMA_STREAM_RX, ENABLE);

// //	while (DMA_GetFlagStatus(DMA1_FLAG_TC6) == RESET);
// 	DMA_ITConfig(DBG_UART_DMA_STREAM_TX, DBG_UART_DMA_IT_TC_TX, ENABLE);
// 	
// 	
// 	STM_EVAL_LEDToggle(LED_RED);
// }

void UART_TransmitBuf_DMA( const uint8_t *buf, uint16_t len )
{
	DMA_InitTypeDef DMA_InitStructure;

	while( DMA_GetCurrDataCounter(DBG_UART_DMA_STREAM_TX) != 0 ){};
	
	DMA_DeInit(DBG_UART_DMA_STREAM_TX);
	DMA_InitStructure.DMA_Channel = DBG_UART_DMA_CHANNEL;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(DBG_UART->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) buf;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_BufferSize = len;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(DBG_UART_DMA_STREAM_TX, &DMA_InitStructure);
	
	USART_ClearFlag(DBG_UART, USART_FLAG_TC);
	USART_DMACmd(DBG_UART, USART_DMAReq_Tx, ENABLE);
	DMA_ITConfig(DBG_UART_DMA_STREAM_TX, DMA_IT_TC, ENABLE);
	
	DMA_Cmd(DBG_UART_DMA_STREAM_TX, ENABLE);
	
}

/**************************************************************************************/

void DBG_UART_DMA_STREAM_TX_IRQHandler( void )
{
	//if TransferComplete interrupt
	if (DMA_GetITStatus(DBG_UART_DMA_STREAM_TX, DBG_UART_DMA_IT_TC_TX) == SET)
	{
		DMA_ClearFlag(DBG_UART_DMA_STREAM_TX, DBG_UART_DMA_IT_TC_TX);
		STM_EVAL_LEDToggle(LED_BLUE);
		// Disable DMA
		USART_DMACmd(DBG_UART, USART_DMAReq_Tx, DISABLE);
		DMA_Cmd(DBG_UART_DMA_STREAM_TX, DISABLE);
	}
}

/**************************************************************************************/

void UART_TransmitLine( char* data )
{
	uint16_t size = 0;
	for(size=0;data[size];size++){};
	UART_TransmitBuf_DMA((uint8_t*)data,size);
}

/**************************************************************************************/
