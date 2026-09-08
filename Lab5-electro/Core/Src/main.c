/* USER CODE BEGIN Header */
//*******************************************************************************//
// Universidad del Valle de Guatemala
// Laboratorio 5 - Electronica Digital 2
//  Juego de carreras
// Camila Tello 24404
//*******************************************************************************//
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

//*********************************************************************//
// VARIABLES DE LOS JUGADORES
//*********************************************************************//

// Posicion actual de cada jugador
volatile uint8_t contadorRojo = 0;
volatile uint8_t contadorAzul = 0;

// Guardan el tiempo de la ultima pulsacion para evitar rebotes
uint32_t tiempoBotonRojo = 0;
uint32_t tiempoBotonAzul = 0;

//*********************************************************************//
// VARIABLES PARA CONTROLAR EL JUEGO
//*********************************************************************//

// 0 = juego bloqueado y 1 = juego habilitado
volatile uint8_t juegoIniciado = 0;

// Indica que se debe realizar la secuencia del semaforo
volatile uint8_t iniciarSemaforo = 0;

// Evita iniciar dos secuencias al mismo tiempo
volatile uint8_t semaforoActivo = 0;

//*********************************************************************//
// VARIABLES PARA UART Y GANADOR
//*********************************************************************//

// Guarda el caracter recibido desde la computadora
uint8_t datoUART = 0;

// 0 = sin ganador, 1 = jugador rojo y 2 = jugador azul
volatile uint8_t ganador = 0;

// Indica que falta enviar el mensaje del ganador
volatile uint8_t enviarGanador = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

// Funciones para mostrar la posicion de los jugadores
void mostrarJugadorRojo(uint8_t posicion);
void mostrarJugadorAzul(uint8_t posicion);

// Funcion para realizar la secuencia del semaforo
void secuenciaSemaforo(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */

  //*********************************************************************//
  // ESTADO INICIAL DEL PROGRAMA
  //*********************************************************************//

  // Apagar los tres LEDs del semaforo antes de comenzar
  HAL_GPIO_WritePin(Ledrojo_GPIO_Port, Ledrojo_Pin, 0);
  HAL_GPIO_WritePin(Ledamarillo_GPIO_Port, Ledamarillo_Pin, 0);
  HAL_GPIO_WritePin(Ledverde_GPIO_Port, Ledverde_Pin, 0);

  // Colocar a los dos jugadores en la posicion inicial
  mostrarJugadorRojo(contadorRojo);
  mostrarJugadorAzul(contadorAzul);

  // El juego permanece bloqueado hasta recibir la letra S
  juegoIniciado = 0;

  // Mensaje para comprobar que la comunicacion funciona
  uint8_t mensajeInicio[] = "\r\nUART lista. Envie S\r\n";

  HAL_UART_Transmit(&huart2,
                    mensajeInicio,
                    sizeof(mensajeInicio) - 1,
                    HAL_MAX_DELAY);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    //-------------------------------------------------------------------//
    // REVISAR SI SE RECIBIO UN CARACTER POR UART
    //-------------------------------------------------------------------//

    // Se usa un tiempo corto para no detener el funcionamiento del juego
    if (HAL_UART_Receive(&huart2, &datoUART, 1, 10) == HAL_OK)
    {
      // La letra S permite iniciar una carrera nueva
      if ((datoUART == 'S') &&
          (juegoIniciado == 0) &&
          (semaforoActivo == 0) &&
          (enviarGanador == 0))
      {
        // Levantar las banderas para comenzar el semaforo
        semaforoActivo = 1;
        iniciarSemaforo = 1;
      }

      // Limpiar la variable para recibir otro caracter
      datoUART = 0;
    }

    //-------------------------------------------------------------------//
    // ENVIAR EL MENSAJE DEL GANADOR
    //-------------------------------------------------------------------//

    if (enviarGanador == 1)
    {
      if (ganador == 1)
      {
        uint8_t mensajeRojo[] = "El ganador es el jugador rojo!\r\n";

        HAL_UART_Transmit(&huart2,
                          mensajeRojo,
                          sizeof(mensajeRojo) - 1,
                          HAL_MAX_DELAY);
      }
      else if (ganador == 2)
      {
        uint8_t mensajeAzul[] = "El ganador es el jugador azul!\r\n";

        HAL_UART_Transmit(&huart2,
                          mensajeAzul,
                          sizeof(mensajeAzul) - 1,
                          HAL_MAX_DELAY);
      }

      // El mensaje ya fue enviado
      enviarGanador = 0;
    }

    //-------------------------------------------------------------------//
    // INICIAR UNA CARRERA NUEVA
    //-------------------------------------------------------------------//

    if (iniciarSemaforo == 1)
    {
      // Bajar la bandera para ejecutar la secuencia una sola vez
      iniciarSemaforo = 0;

      // Bloquear los botones durante el semaforo
      juegoIniciado = 0;

      // Reiniciar la posicion de los dos jugadores
      contadorRojo = 0;
      contadorAzul = 0;
      ganador = 0;

      mostrarJugadorRojo(contadorRojo);
      mostrarJugadorAzul(contadorAzul);

      // Avisar que la letra S fue recibida
      uint8_t mensajeRecibido[] = "S recibida\r\n";

      HAL_UART_Transmit(&huart2,
                        mensajeRecibido,
                        sizeof(mensajeRecibido) - 1,
                        HAL_MAX_DELAY);

      // Realizar la secuencia rojo, amarillo y verde
      secuenciaSemaforo();

      // Avisar que los jugadores ya pueden comenzar
      uint8_t mensajeListo[] = "Inicia el juego!\r\n";

      HAL_UART_Transmit(&huart2,
                        mensajeListo,
                        sizeof(mensajeListo) - 1,
                        HAL_MAX_DELAY);
    }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Ledrojo_Pin|Ledverde_Pin|Ledamarillo_Pin|Ledr1_Pin
                          |Ledr2_Pin|Ledr3_Pin|LedB1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LedB2_Pin|LedB3_Pin|LedB4_Pin|Ledr4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B2_Pin */
  GPIO_InitStruct.Pin = B2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Ledrojo_Pin Ledverde_Pin Ledamarillo_Pin Ledr1_Pin
                           Ledr2_Pin Ledr3_Pin LedB1_Pin */
  GPIO_InitStruct.Pin = Ledrojo_Pin|Ledverde_Pin|Ledamarillo_Pin|Ledr1_Pin
                          |Ledr2_Pin|Ledr3_Pin|LedB1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LedB2_Pin LedB3_Pin LedB4_Pin Ledr4_Pin */
  GPIO_InitStruct.Pin = LedB2_Pin|LedB3_Pin|LedB4_Pin|Ledr4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//*********************************************************************//
// FUNCION PARA MOSTRAR AL JUGADOR ROJO
//*********************************************************************//

void mostrarJugadorRojo(uint8_t posicion)
{
  // Apagar primero los cuatro LEDs del jugador rojo
  HAL_GPIO_WritePin(Ledr1_GPIO_Port, Ledr1_Pin, 0);
  HAL_GPIO_WritePin(Ledr2_GPIO_Port, Ledr2_Pin, 0);
  HAL_GPIO_WritePin(Ledr3_GPIO_Port, Ledr3_Pin, 0);
  HAL_GPIO_WritePin(Ledr4_GPIO_Port, Ledr4_Pin, 0);

  // Encender solamente el LED de la posicion actual
  switch (posicion)
  {
    case 1:
      HAL_GPIO_WritePin(Ledr1_GPIO_Port, Ledr1_Pin, 1);
      break;

    case 2:
      HAL_GPIO_WritePin(Ledr2_GPIO_Port, Ledr2_Pin, 1);
      break;

    case 3:
      HAL_GPIO_WritePin(Ledr3_GPIO_Port, Ledr3_Pin, 1);
      break;

    case 4:
      HAL_GPIO_WritePin(Ledr4_GPIO_Port, Ledr4_Pin, 1);
      break;

    default:
      // En la posicion 0 todos permanecen apagados
      break;
  }
}

//*********************************************************************//
// FUNCION PARA MOSTRAR AL JUGADOR AZUL
//*********************************************************************//

void mostrarJugadorAzul(uint8_t posicion)
{
  // Apagar primero los cuatro LEDs del jugador azul
  HAL_GPIO_WritePin(LedB1_GPIO_Port, LedB1_Pin, 0);
  HAL_GPIO_WritePin(LedB2_GPIO_Port, LedB2_Pin, 0);
  HAL_GPIO_WritePin(LedB3_GPIO_Port, LedB3_Pin, 0);
  HAL_GPIO_WritePin(LedB4_GPIO_Port, LedB4_Pin, 0);

  // Encender solamente el LED de la posicion actual
  switch (posicion)
  {
    case 1:
      HAL_GPIO_WritePin(LedB1_GPIO_Port, LedB1_Pin, 1);
      break;

    case 2:
      HAL_GPIO_WritePin(LedB2_GPIO_Port, LedB2_Pin, 1);
      break;

    case 3:
      HAL_GPIO_WritePin(LedB3_GPIO_Port, LedB3_Pin, 1);
      break;

    case 4:
      HAL_GPIO_WritePin(LedB4_GPIO_Port, LedB4_Pin, 1);
      break;

    default:
      // En la posicion 0 todos permanecen apagados
      break;
  }
}

//*********************************************************************//
// FUNCION PARA REALIZAR LA SECUENCIA DEL SEMAFORO
//*********************************************************************//

void secuenciaSemaforo(void)
{
  // Mantener bloqueado el juego durante toda la secuencia
  juegoIniciado = 0;

  //-------------------------------------------------------------------//
  // Encender el LED rojo durante un segundo
  //-------------------------------------------------------------------//
  HAL_GPIO_WritePin(Ledrojo_GPIO_Port, Ledrojo_Pin, 1);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(Ledrojo_GPIO_Port, Ledrojo_Pin, 0);

  //-------------------------------------------------------------------//
  // Encender el LED amarillo durante un segundo
  //-------------------------------------------------------------------//
  HAL_GPIO_WritePin(Ledamarillo_GPIO_Port, Ledamarillo_Pin, 1);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(Ledamarillo_GPIO_Port, Ledamarillo_Pin, 0);

  //-------------------------------------------------------------------//
  // Encender el LED verde durante un segundo
  //-------------------------------------------------------------------//
  HAL_GPIO_WritePin(Ledverde_GPIO_Port, Ledverde_Pin, 1);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(Ledverde_GPIO_Port, Ledverde_Pin, 0);

  // La secuencia terminó y el juego puede comenzar
  juegoIniciado = 1;

  // Permitir que se pueda recibir otra S al terminar una carrera
  semaforoActivo = 0;
}

//*********************************************************************//
// INTERRUPCIONES DE LOS BOTONES
//*********************************************************************//

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  //-------------------------------------------------------------------//
  // BOTON DEL JUGADOR ROJO
  //-------------------------------------------------------------------//

  if (GPIO_Pin == B1_Pin)
  {
    // El boton funciona solamente cuando la carrera esta activa
    if (juegoIniciado == 1)
    {
      // Aceptar una pulsacion cada 200 milisegundos
      if ((HAL_GetTick() - tiempoBotonRojo) >= 200)
      {
        tiempoBotonRojo = HAL_GetTick();

        // Avanzar hasta la posicion final
        if (contadorRojo < 4)
        {
          contadorRojo++;
        }

        mostrarJugadorRojo(contadorRojo);

        // Al llegar a 4, el jugador rojo gana
        if (contadorRojo == 4)
        {
          ganador = 1;
          enviarGanador = 1;

          // Bloquear inmediatamente los dos botones
          juegoIniciado = 0;
        }
      }
    }
  }

  //-------------------------------------------------------------------//
  // BOTON DEL JUGADOR AZUL
  //-------------------------------------------------------------------//

  if (GPIO_Pin == B2_Pin)
  {
    // El boton funciona solamente cuando la carrera esta activa
    if (juegoIniciado == 1)
    {
      // Cada jugador tiene su propio tiempo de anti-rebote
      if ((HAL_GetTick() - tiempoBotonAzul) >= 200)
      {
        tiempoBotonAzul = HAL_GetTick();

        // Avanzar hasta la posicion final
        if (contadorAzul < 4)
        {
          contadorAzul++;
        }

        mostrarJugadorAzul(contadorAzul);

        // Al llegar a 4, el jugador azul gana
        if (contadorAzul == 4)
        {
          ganador = 2;
          enviarGanador = 1;

          // Bloquear inmediatamente los dos botones
          juegoIniciado = 0;
        }
      }
    }
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
