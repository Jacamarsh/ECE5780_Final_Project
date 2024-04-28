/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Final Project
  ******************************************************************************
  * This is the controlling .c file of the program, responsible for all logic
  * that occurs in this program. Program functionality all under Project's
  * README.md file. All logic discussed in file is handled in this code.
  *
  * @attention
  * Authors: Jack Marshall, Andrew Piskadlo, and Emily Erickson
  * University of Utah, ECE 5780: Embedded Systems
  * April 20th, 2024
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h> // Include the standard library for boolean variables
#include <stdio.h>
#include <stdlib.h>

#define ROWS 10 // Number of rows in the 2D array, so 10 guestures each with data for 8 channels can be saved
#define COLS 8  // Number of columns in the 2D array, so 8 channels can be saved

#define MAX_ARRAY_SIZE 100  // Maximum size of the array holding the 8-channels ADC data (12 digits per, so 96 total?), adjust as needed
#define MAX_STRING_LENGTH 189 // string length we need to transmit to UART to force each message to a new line (when putty in fullscreen mode)

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// Declare boolean variables
bool gestureIdentified = false;
bool gestureUnidentified = false;
bool gestureError = false;
bool alreadyCalibrated;
bool calibrationStarted;
bool testStarted;
char userInput;
char button;
int test;
int calibrationCounter = 0;
int testCounter;
int calibrateGestureNumber;
int testGestureNumber;
char errorMsgWrongLetter[] = " wrong input press r, g, b or o.";
char errorMsgWrongNumber[] = " wrong input press 0, 1, or 2. ";
char errorMsgRedOn[] = "                     turned on red LED.";
char errorMsgRedOff[] = "                     turned off red LED.";
char errorMsgRedToggle[] = "                     toggled red LED.";
char errorMsgGreenOn[] = "                     turned on green LED.";
char errorMsgGreenOff[] = "                     turned off green LED.";
char errorMsgGreenToggle[] = "                     toggled green LED.";
char errorMsgBlueOn[] = "                     turned on blue LED.";
char errorMsgBlueOff[] = "                     turned off blue LED.";
char errorMsgBlueToggle[] = "                     toggled blue LED.";
char errorMsgOrangeOn[] = "                     turned on orange LED.";
char errorMsgOrangeOff[] = "                     turned off orange LED.";
char errorMsgOrangeToggle[] = "                     toggled orange LED.";
char errorMsgBadArgument[] = " must enter 'start.' to calibrate, or 'test' to identify gesture. must 'start' before using 'test'.";
char errorMsgNotCalibrated[] = " cant 'test' before you 'start' calibration.";
char errorMsgCalibrated[] = " cant 'calibrate' twice in a row! push reset button to start over to recalibrate.";
char userInputArray[8];


int x = 115; // hard code length of message

// Placeholder for 0's and 1's in channels 1-8 after filtering and ADC
// Used to influence case statements for UART to print messages.
char channels[8];
uint32_t ADC_VAL[8] = {0,0,0,0,0,0,0,0}; // empty ADC values for all 8 channels
unsigned long ADC_TEST_VALS[8] = {0,0,0,0,0,0,0,0}; // empty ADV values for all 8 channels
unsigned long KALMAN_VALS[8] = {0,0,0,0,0,0,0,0}; // empty ADV values for all 8 channels
unsigned long ADCLibrary[ROWS][COLS]; // Global 2D array

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

/* USER CODE BEGIN PV */
ADC_HandleTypeDef hadc;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void TransmitCharacter (char input);
void CharReader(char string[x]);
void UARTConfig(void);
void UARTBegin(void);
void ClockLogic(void);
void SetupGPIOs(void);
void InitLEDsState(void);
void EnableUART(void);
void GetSignals(void);
void SaveGesture(void);
void BeginGesture(void);
void TestGesture(void);
void ADCInit(void);
void StoreADC(void);
void ADC_Select_CH1(void);
void ADC_Select_CH2(void);
void ADC_Select_CH3(void);
void ADC_Select_CH4(void);
void ADC_Select_CH5(void);
void ADC_Select_CH6(void);
void ADC_Select_CH7(void);
void ADC_Select_CH8(void);
void PrintError(char* values);
void PrintMessage(char* values);
void store_in_2d_array(unsigned long input[]);
void printArray(unsigned long* arr, int size);
bool ApproxGesture(unsigned long input[]);
bool CompareChannels(const char* values);
void SetChannels(const char* values);
void SetGestureFlags(bool identified, bool unidentified, bool error);
bool compareStrings(const char *str1, const char *str2);
unsigned long kalman_filter(unsigned long ADC_Value);
char* arrayToString(unsigned long* arr, int size);
int countNonEmptyIndices(char array[]);
static void MX_ADC_Init(void);

/* USER CODE BEGIN PFP */

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
  HAL_Init(); // Initialize the Hardware Abstraction Layer (HAL)

  SystemClock_Config(); // Configure the system clock

  ClockLogic(); // Set up and configure various clocks

  SetupGPIOs(); // Clear registers for GPIO configuration

  ADCInit(); // Initialize Analog to Digital Converter (ADC)

  MX_ADC_Init(); // Initialize ADC pin configurations and parameters

  SetChannels("00000000"); // Set initial values in the channels array for gesture identification

  UARTConfig(); // Configure UART (Universal Asynchronous Receiver/Transmitter)

  InitLEDsState(); // Initialize LEDs state

  EnableUART(); // Turn on UART communication

  alreadyCalibrated = false; // Initialize calibration flags
  calibrationStarted = false;
  calibrationCounter = 0;
  testCounter = 0;

  calibrateGestureNumber = 1; // Set the number of gestures to calibrate and test
  testGestureNumber = 1;

  /* Infinite loop */
  while (1)
  {
    SetGestureFlags(false, false, false); // Reset gesture flags

    HAL_Delay(20); // General delay to control the rate of data processing

    // Check if all test gestures have been completed
    if (testCounter >= testGestureNumber)
    {
      PrintError("Main method: Finished!!");
      while (1)
      {
        // Infinite loop to halt execution
      }
    }

    if (!calibrationStarted)
    {
      // If calibration mode is not yet started, prompt user to initiate calibration
      PrintMessage("Enter 'start.' to start calibrating!");
      UARTBegin();
    }
    else
    {
      if (calibrationCounter == 0)
      {
        PrintMessage("Calibrate mode selected: push blue button to calibrate gesture!");
      }

      if (calibrationCounter < calibrateGestureNumber)
      {
        BeginGesture(); // Wait for the user to press a button to start recording gesture data
        GetSignals(); // Acquire signal data from the ADC
        SaveGesture(); // Save the recorded gesture data
      }
      else
      {
        if (testCounter == 0 && (!gestureUnidentified) && (!gestureError))
        {
          PrintMessage("All gestures calibrated, enter 'test.' to start testing gestures!");
          UARTBegin();
        }

        if (testStarted)
        {
          if (testCounter == 0)
          {
            PrintMessage("Test mode selected: push blue button when ready to test gesture!");
          }

          if (testCounter < testGestureNumber)
          {
            BeginGesture(); // Wait for the user to press a button to start recording gesture data
            GetSignals(); // Acquire signal data from the ADC
            TestGesture(); // Test the recorded gesture against calibrated gestures
          }
          else
          {
            PrintError("Main Method: Error!, restart program");
            while (1)
            {
              // Infinite loop to halt execution
            }
          }
        }
      }
    }
  }
}

/*****************************************************
*Function name: kalman_filter
 *Function function: ADC_filter
 *Entry parameter: ADC_Value
 *Export parameters: kalman_adc
*****************************************************/
unsigned long kalman_filter(unsigned long ADC_Value)
{
    float x_k1_k1,x_k_k1;
    static float ADC_OLD_Value;
    float Z_k;
    static float P_k1_k1;

    static float Q = 0.0001;//Q: Regulation noise, Q increases, dynamic response becomes faster, and convergence stability becomes worse
    static float R = 0.005; //R: Test noise, R increases, dynamic response becomes slower, convergence stability becomes better
    static float Kg = 0;
    static float P_k_k1 = 1;

    float kalman_adc;
    static float kalman_adc_old=0;
    Z_k = ADC_Value;
    x_k1_k1 = kalman_adc_old;

    x_k_k1 = x_k1_k1;
    P_k_k1 = P_k1_k1 + Q;

    Kg = P_k_k1/(P_k_k1 + R);

    kalman_adc = x_k_k1 + Kg * (Z_k - kalman_adc_old);
    P_k1_k1 = (1 - Kg)*P_k_k1;
    P_k_k1 = P_k1_k1;

    ADC_OLD_Value = ADC_Value;
    kalman_adc_old = kalman_adc;

    return kalman_adc;
}


// Waits until USER btn (the blue button) is held/pressed (to indicate I'm ready to record
// the input data from these gestures.
void BeginGesture(void)
{
	uint32_t debouncer = 0; // Variable to store button state for debounce

    while (1) {
        debouncer = (debouncer << 1); // Always shift every loop iteration
        if (GPIOA->IDR & GPIO_PIN_0) { // If input signal is set/high
            debouncer |= 0x01; // Set lowest bit of bit-vector
        }

        // Check if the debounce variable has reached a steady state
        if (debouncer == 0xFFFFFFFF) {
            // This logic executes if button properly pressed/held, which indicates begin gesture!!
					PrintMessage("BeginGesture: Button pressed to start gesture! ");
          break;
        }

        // When button is bouncing the bit-vector value is random since bits are set when the button is high and not when it bounces low.
        // Add a small delay to prevent debouncing issues
        for (volatile int i = 0; i < 1000; i++);
    }

}

// Once all gestures are calibrated and stored, we will test a gesture out!
void TestGesture(void)
{
  int arrSize = sizeof(KALMAN_VALS) / sizeof(KALMAN_VALS[0]); // Calculate the size of the array
	PrintMessage("TestGesture: KALMAN Values--> ");
	printArray(KALMAN_VALS, arrSize); // print values 
	
	if(ApproxGesture(KALMAN_VALS) && (!gestureUnidentified) && (!gestureError))
	{
		PrintMessage("TestGesture: Success, gesture was identified!");
		testCounter++;
	} else if ((!gestureError) && (gestureUnidentified))
	{
		PrintMessage("TestGesture: Error, something went wrong");
	} else if ((gestureError) || (!gestureUnidentified))
	{
		PrintMessage("TestGesture: Unidentified/flag, gesture not close to one calibrated!");
	} else
	{
		PrintMessage("TestGesture: Unidentified/bad approximation, gesture not close to one calibrated!");
	}
	
}

// Initializes the Analog-to-Digital Converter (ADC) for reading analog signals.
void ADCInit(void)
{
	// Configure PC4 pin as analog input
	GPIO_InitTypeDef adc = {GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOC, &adc);

	// Enable clock for ADC1
	__HAL_RCC_ADC1_CLK_ENABLE();

	// Set ADC resolution to 8-bit
	ADC1->CFGR1 |= (1 << 4);
	ADC1->CFGR1 |= (1 << 13);
	ADC1->CHSELR |= (1 << 14);

	// Additional configuration needed for other channels
}

// Reads signals from ADC for a duration of approximately 3 seconds.
void GetSignals(void)
{
	uint32_t startTime = HAL_GetTick(); // Get the current time in milliseconds
	uint32_t elapsedTime;

	while (1)
	{
		StoreADC(); // Call StoreADC function

		// Calculate elapsed time
		elapsedTime = HAL_GetTick() - startTime;

		// Check if 3 seconds have elapsed
		if (elapsedTime >= 1000)
		{
			break; // Exit the loop
		}
	}
}

// Checks if the input array matches any calibrated gesture within given thresholds.
bool ApproxGesture(unsigned long input[]) {
	for (int i = 0; i < calibrateGestureNumber; i++) {
		bool match = true;
		for (int j = 0; j < COLS; j++) {
			// Compare input values with calibrated values within thresholds
			if (!(((input[j] - ADCLibrary[i][j]) <= 20) || ((ADCLibrary[i][j] - input[j]) <= 5)))
			{ 
				PrintMessage("ApproxGesture: wrong channel value detected!");
				match = false;
				break;
			} 
		}
		if (match) {
			return true; // Found a match
		}
	}
	return false; // No match found
}

// Stores input array in a 2D array for gesture calibration.
void store_in_2d_array(unsigned long input[]) {
	if (calibrationCounter < ROWS) {
		for (int j = 0; j < COLS; j++) {
			ADCLibrary[calibrationCounter][j] = input[j];
		}
	} else {
		PrintMessage("Store 2D Array: 2D array is full. Cannot store more data.");
	}
}

// Saves the calibrated gesture data if no error occurred during calibration.
void SaveGesture (void)
{
	int arrSize = sizeof(KALMAN_VALS) / sizeof(KALMAN_VALS[0]); // Calculate the size of the array
	PrintMessage("SaveGesture: KALMAN Values--> ");
	printArray(KALMAN_VALS, arrSize); // Print values 
	
	if (gestureError)
	{
		// If error occurred during calibration, print error message and return
		PrintMessage("SaveGesture: Error saving gesture, trying to re-calibrate!");
	} else
	{
		// If no error, store calibrated gesture data
		store_in_2d_array(KALMAN_VALS);
		PrintMessage("SaveGesture: Success!");
		
		calibrationCounter++;
	}
}

// Prints an error message to Putty using CharReader, turns off all lights, and turns on the red LED to indicate an error.
void PrintError(char* values)
{
	CharReader(values); // Display the error message

	// Turn off all LEDs and turn on the red LED to indicate an error
	GPIOC->ODR |= (1<<6); // Turn on red LED
	GPIOC->ODR &= ~(1<<7); // Turn off other LEDs
	GPIOC->ODR &= ~(1<<8);
	GPIOC->ODR &= ~(1<<9);
}

// Prints a message to Putty using CharReader.
void PrintMessage(char* values)
{
	CharReader(values); // Display the message
}

// Sets values in the channels array.
void SetChannels(const char* values) {
    int i;
    // Copy values to the channels array
    for (i = 0; i < 8 && values[i] != '\0'; ++i) {
        channels[i] = values[i];
    }
    // Fill the remaining elements with null terminator if needed
    for (; i < 8; ++i) {
        channels[i] = '\0';
    }
}

// Compares two strings and returns true if they match, false otherwise.
bool compareStrings(const char *str1, const char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return false; // Characters don't match, return false
        }
        str1++;
        str2++;
    }
    // Check if both strings ended at the same position
    if (*str1 == '\0' && *str2 == '\0') {
        return true; // Strings match
    } else {
        return false; // Strings don't match in length
    }
}

// Compares values in the channels array with a given string.
bool CompareChannels(const char* values) {
    for (int i = 0; i < 8; ++i) {
        if (channels[i] != values[i]) {
            return false; // Return false if any characters don't match
        }
    }
    return true; // Return true if all characters match
}

// Sets the values of gestureIdentified, gestureUnidentified, and gestureError flags.
void SetGestureFlags(bool identified, bool unidentified, bool error) {
    gestureIdentified = identified;
    gestureUnidentified = unidentified;
    gestureError = error;
}

// Enable clock for required peripherals.
void ClockLogic(void)
{
    // Enable clock for GPIOA (user btn)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    // Enable clock for GPIOB (TX and RX pins for UART)
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Enable clock for USART3 (UART)
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    // Enable clock for GPIOC (LEDs)
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
}

// Configures UART for use.
void UARTConfig(void)
{
    // Configure pins PB10(TX) and PB11(RX) for UART
    GPIOB->AFR[1] &=~ ((1<<8)|(1<<9)|(1<<11)|(1<<12)|(1<<13)|(1<<15)); // Alternate function on PB10 and PB11
    GPIOB->AFR[1] |= ((1<<10)|(1<<14));

    // Set baud rate to 115200
    USART3->BRR = 69; // Actual is 69.44 (8000000/115200), actual baud rate now is 115942

    // Enable receiver and transmitter
    USART3->CR1 |= ((1<<2)|(1<<3));
}

// Enables UART.
void EnableUART(void)
{
    USART3->CR1 |= (1<<0); // Enable USART
}

// Begins UART communication and handles user input.
void UARTBegin(void)
{
    userInput = 0; // Reset user input
    int inputIndex = 0; // Index for storing user inputs in the array
    char userInputArray[8]; // Array to store user inputs

    // Initialize userInputArray with null characters
    for (int i = 0; i < 8; ++i) {
        userInputArray[i] = '\0';
    }

    while (1) {
        // Wait for user input
        while (!(USART3->ISR & (1 << 5))) {
            // Wait until data is ready to be received
        }
        // Read the user input
        char userInput = USART3->RDR;

        // Check if the user input is the period character
        if (userInput == '.') {
            // End of user input, break the loop
            break;
        }

        // Store the user input in the array
        userInputArray[inputIndex] = userInput;

        // Print the received character (for debugging)
        TransmitCharacter(userInput);

        // Increment the index for the next user input
        inputIndex++;

        // Check if the array is full
        if (inputIndex >= 8) {
            // Array is full, break the loop
            break;
        }
    }
    
    // Check user input and perform corresponding actions
    if (compareStrings(userInputArray, "start")) {
        if(!calibrationStarted){
            GPIOC->ODR |= (1<<9); // Turn on green LED
            GPIOC->ODR &= ~(1<<8); // Turn off orange LED
            GPIOC->ODR &= ~(1<<7); // Turn off blue LED
            CharReader(errorMsgGreenOn); // Print message that green LED is on
            calibrationStarted = true;
        } else {
            CharReader(errorMsgCalibrated); // Cannot calibrate twice in a row
        }
    } else if (compareStrings(userInputArray, "test")) {
        if(calibrationStarted){
            GPIOC->ODR &= ~(1<<9); // Turn off green LED
            GPIOC->ODR |= (1<<8); // Turn on orange LED
            GPIOC->ODR &= ~(1<<7); // Turn off blue LED
            CharReader(errorMsgOrangeOn); // Print message that orange LED is on
            testStarted = true;
        } else {
            CharReader(errorMsgNotCalibrated); // Cannot test gestures before calibration
        }
    } else {
        CharReader(errorMsgBadArgument); // Invalid input
    }
    
    // Clear the userInputArray
    for (int i = 0; i < sizeof(userInputArray); ++i) {
        userInputArray[i] = '\0'; // Set each element to '\0' (null character)
    }
    // Clear the user input
    userInput = 0;
    HAL_Delay(10); // Small delay because baud rate is faster than the board
}

// Transmits a single character through USART3.
void TransmitCharacter (char input) {
    uint32_t TXEmask = (1<<7); // Bit mask for checking TXE flag
    while ((USART3->ISR & TXEmask) == 0) { // Wait until TXE flag is set
    }
    USART3->TDR = input; // Write character to data register for transmission
}

// Reads a string and transmits its characters through USART3, padding with spaces if needed.
void CharReader(char string[]) {
    int i;
    int length = 0;
    int currLineDiff = MAX_STRING_LENGTH - countNonEmptyIndices(userInputArray); // Calculate current line difference

    // Calculate the length of the string
    for (i = 0; string[i] != '\0'; i++) {
        length++;
    }

    // Transmit characters from the input string
    for (i = 0; i < length; i++) {
        TransmitCharacter(string[i]);
    }

    // Pad the string with space characters if its length is less than currLineDiff characters
    if (length < currLineDiff) {
        for (i = length; i < currLineDiff; i++) {
            TransmitCharacter(' ');
        }
    }
}

// Converts an array of unsigned long integers to a string.
char* arrayToString(unsigned long* arr, int size) {
    // Allocate memory for the string
    char* str = (char*)malloc((size * 12) * sizeof(char)); // Assuming max unsigned long size is 10 digits + 1 comma + 1 space + 1 null terminator

    if (str == NULL) {
        PrintMessage("Array To String: memory allocation failed"); // Notify if memory allocation fails
        return NULL;
    }

    // Initialize index for the string
    int index = 0;

    // Convert each element of the array to string and append to the string with comma separation
    for (int i = 0; i < size; i++) {
        index += sprintf(&str[index], "%lu", arr[i]); // Convert unsigned long to string
        if (i != size - 1) {
            index += sprintf(&str[index], ","); // Add comma and space if not the last element
        }
    }

    return str; // Return the dynamically allocated string
}

// Prints an array of unsigned long integers.
void printArray(unsigned long* arr, int size) {
    // Convert array to string
    char* str = arrayToString(arr, size);
    if (str == NULL) {
        PrintMessage("Printing array: memory allocation failed"); // Notify if memory allocation fails
        return; // Memory allocation failed
    }

    // Print the resulting string
    PrintMessage(str); // Print the string through UART

    // Free allocated memory
    free(str); // Release the dynamically allocated string
}

// Counts the number of non-empty elements in a character array.
int countNonEmptyIndices(char array[]) {
    int count = 0;
    
    // Iterate through the array and count non-empty elements
    for (int i = 0; i < 8; i++) {
        if (array[i] != '\0') {
            count++;
        }
    }   
    return count; // Return the count of non-empty elements
}

// setup for GPIOA, GPIOB, and GPIOC
void SetupGPIOs(void)
{
	//GPIOC setup for LEDs
	// moder 01 for general purpose
	GPIOC->MODER |= ((1<<14)|(1<<12)|(1<<18)|(1<<16)); // normal function mode 
	GPIOC->MODER &=~  ((1<<15)|(1<<13)|(1<<19)|(1<<17)); // normal function mode 
	
	// GPIOB setup for PB10 and PB11's regsiters to use RX and TX in UART later
	GPIOB->MODER  |= ((1<<21)|(1<<23));
	GPIOB->MODER  &=~ ((1<<20)|(1<<22));
	
	//setting PA0 (User btn) to input mode
	GPIOA->MODER &= ~((1<<0) | (1<<1)); 	
	
	// setting PAO (user btn) to low-speed
	GPIOA->OSPEEDR |= (1<<0); 
	
	// setting PA0 (User btn) to pull-down mode --> low, then up (to VCC) when pressed
	// aka, 0 not pressed, 1 when pressed. used for if 0(not pressed), do this. if 1 (pressed) do this
	GPIOA->PUPDR &= ~(1<<0);
	GPIOA->PUPDR |= (1<<1);
}

// start with all LEDs on
void InitLEDsState(void)
{
	//Comented out because we want to start with no LEDs on (at least for now)
	GPIOC->ODR &= ~(1<<6);
	GPIOC->ODR |= (1<<7); // blue to indicate it is running
	GPIOC->ODR &= ~(1<<8);
	GPIOC->ODR &= ~(1<<9);
}

// Storing Analog-to-Digital Conversion for each of the 8 channels into ADC_VAL uint32_t array
void StoreADC(void)
{
	// Channel 1
	ADC_Select_CH1();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); // 1000 is the timeout
	KALMAN_VALS[0] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
		
	// Channel 2
	ADC_Select_CH2();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[1] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
	
	// Channel 3
	ADC_Select_CH3();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[2] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
	
	// Channel 4
	ADC_Select_CH4();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[3] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
	
	// Channel 5
	ADC_Select_CH5();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	KALMAN_VALS[4] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
	
	// Channel 6
	ADC_Select_CH6();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[5] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
	
	// Channel 7
	ADC_Select_CH7();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[6] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
	
	// Channel 8
	ADC_Select_CH8();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	KALMAN_VALS[7] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	HAL_ADC_Stop(&hadc);
	
	int arrSize = sizeof(KALMAN_VALS) / sizeof(KALMAN_VALS[0]);
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
	
// Alter values below to match source's settings if things go wrong!
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
	hadc.Init.ScanConvMode = ENABLE;
	hadc.Init.ContinuousConvMode = ENABLE;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
}

// Selects ADC channel 1 and configures its settings.
void ADC_Select_CH1(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// Selects ADC channel 2 and configures its settings.
void ADC_Select_CH2(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}
// Selects ADC channel 3 and configures its settings.
void ADC_Select_CH3(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_9;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// Selects ADC channel 4 and configures its settings.
void ADC_Select_CH4(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// Selects ADC channel 5 and configures its settings.
void ADC_Select_CH5(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// Selects ADC channel 6 and configures its settings.
void ADC_Select_CH6(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// Selects ADC channel 7 and configures its settings.
void ADC_Select_CH7(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

// Selects ADC channel 8 and configures its settings.
void ADC_Select_CH8(void)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	
	// Configure for the selected ADC regular channel, its corresponding rank in the sequencer, and its sample time.
	sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

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

#ifdef  USE_FULL_ASSERT
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
