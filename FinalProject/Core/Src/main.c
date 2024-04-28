/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Final Project
  ******************************************************************************
  * @attention
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
void SetChannels(const char* values);
void SaveGesture(void);
void BeginGesture(void);
void TestGesture(void);
void PrintError(char* values);
void PrintMessage(char* values);
bool CompareChannels(const char* values);
void SetGestureFlags(bool identified, bool unidentified, bool error);
bool compareStrings(const char *str1, const char *str2);
unsigned long kalman_filter(unsigned long ADC_Value);
void ADCInit(void);
static void MX_ADC_Init(void);
void ADC_Select_CH1(void);
void ADC_Select_CH2(void);
void ADC_Select_CH3(void);
void ADC_Select_CH4(void);
void ADC_Select_CH5(void);
void ADC_Select_CH6(void);
void ADC_Select_CH7(void);
void ADC_Select_CH8(void);
void StoreADC(void);
bool ApproxGesture(unsigned long input[]);
void store_in_2d_array(unsigned long input[]);
void printArray(unsigned long* arr, int size);
char* arrayToString(unsigned long* arr, int size);
int countNonEmptyIndices(char array[]);

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
  HAL_Init();
  SystemClock_Config();
	
	// dealing with enabling/configuring various clocks
	ClockLogic();
	
	// clearing registers for all various GPIOs' needs
	SetupGPIOs();
	
	// Setting up ADC's registers and clocks (using 8-bit resolution, with 8 input channels)
	ADCInit();
	
	// initializing ADC pin configurations/parameters (such as clock cycles)
  MX_ADC_Init();
	
	// Set values in the channels array
	// For now, usingn pre-defined channel values to do something
  SetChannels("00000000"); // all 0's -> gesture not identified
  
	// Configuring UART, set state of LEDs, turn on UART
	UARTConfig();
	InitLEDsState();
	EnableUART();
	
		
	// handles all logic for UART, aka communication.
	alreadyCalibrated = false;
	calibrationStarted = false;
	calibrationCounter = 0;
	testCounter = 0;
		
	// Number of gestures we want in our calibration (we put 1 for now)	
	calibrateGestureNumber = 1;
	
	// Number of gestures we want to test against our stored, calibrated gestures (we put 1 for now)
	testGestureNumber = 1;
		
  /* Infinite loop */
  while (1)
  { 
		// resetting gesture flags to ensure previous states dont conflict
		SetGestureFlags(false, false, false);
		// general Delay, can decrease if problems w/ data coming in quicker than delay inhibits it being received
		HAL_Delay(20);
		if (testCounter >= testGestureNumber)
					{
						PrintError("Main method: Finished!!");
						while(1)
						{
							// empty nested while loop
						
						}
					}
		
		if (!calibrationStarted)
		{
			// If calibration mode not yet indicated, UART will keep asking
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
				BeginGesture();
				GetSignals();
				// If SaveGesture successful, input gesture data stored, success msg printed,
				// and calibration counter increased.
				SaveGesture();			
							
			}	
			else
			{
				if (testCounter == 0 && (!gestureUnidentified) && (!gestureError))
				{
					PrintMessage("All gestures calibrated, enter 'test.' to start testing gestures!");
					UARTBegin();
				}
				if(testStarted)
				{
					if (testCounter == 0)
					{
						PrintMessage("Test mode selected: push blue button when ready to test gesture!");
					}
					if (testCounter < testGestureNumber)
					{
						BeginGesture();
						GetSignals();
						TestGesture();
					}			
					else
					{
						PrintError("Main Method: Error!, restart program");
						while(1)
						{
							// empty nested while loop
						
						}
					}			
				}			
			}
		}
		
		//End of main's while loop
	}
	//End of main method
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

void ADCInit(void)
{
	//Input Pin PC4 
	GPIO_InitTypeDef adc = {GPIO_PIN_4, GPIO_MODE_ANALOG, GPIO_SPEED_FREQ_LOW, GPIO_NOPULL};
	HAL_GPIO_Init(GPIOC, &adc);

	
	// PC4 ca be channel 1, we need 7 more to find1111
	
	__HAL_RCC_ADC1_CLK_ENABLE();
	//8 Bit Resolution
	ADC1->CFGR1 |= (1 << 4);
	ADC1->CFGR1 |= (1 << 13);
	ADC1->CHSELR |= (1 << 14);
	
	// for the rest of the channels, do this too!!!

}


// *** update Kalman Filter parameters to match these signal types.
void GetSignals(void)
{
	// ************ Under the presuption that HAL_GetTick is in milliseconds, otherwise 3000 may be too quick *************************
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

bool ApproxGesture(unsigned long input[]) {
	for (int i = 0; i < calibrateGestureNumber; i++) {
        bool match = true;
        for (int j = 0; j < COLS; j++) {
					// *** Fix if statement below to do comparison with given threshold values, so not relying on perfect match ***********8
					// setting approximation values between the calibration and test gesture ADC values
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

void store_in_2d_array(unsigned long input[]) {
    if (calibrationCounter < ROWS) {
        for (int j = 0; j < COLS; j++) {
            ADCLibrary[calibrationCounter][j] = input[j];
        }
    } else {
			PrintMessage("Store 2D Array: 2D array is full. Cannot store more data.");
    }
}


// This method takes the 8-bit ditigal representation of the gesture and stores it in a datastructure for later access
void SaveGesture (void)
{
	int arrSize = sizeof(KALMAN_VALS) / sizeof(KALMAN_VALS[0]); // Calculate the size of the array
	PrintMessage("SaveGesture: KALMAN Values--> ");
	printArray(KALMAN_VALS, arrSize); // print values 
	
	if (gestureError)
	{
		// logic where if error, dont save it, print error message method, and return
		PrintMessage("SaveGesture: Error saving gesture, trying to re-calibrate!");
	} else
	{
		// store all 8 channels ADC values (after through kalman filter) into 2-D unsigned long array
		store_in_2d_array(KALMAN_VALS);
		// logic where if no error and gestureError is still false, save it
		PrintMessage("SaveGesture: Success!");
		
		calibrationCounter++;
	}
}



// Method uses CharReader to print to Putty the problem that has occurred
// this method also sets all lights off, and turns on Red LED to indicate error.
void PrintError(char* values)
{
	CharReader(values);
	
	// Turning off all LEDs, turning red LED on --> indicate an error
	GPIOC->ODR |= (1<<6); // setting red LED on
	GPIOC->ODR &= ~(1<<7);
	GPIOC->ODR &= ~(1<<8);
	GPIOC->ODR &= ~(1<<9);
	
}

// Method uses CharReader to print to Putty the status that has occurred
void PrintMessage(char* values)
{
	CharReader(values);
}


// Method to set values in the channels array
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

// Method to compare values in the channels array with a given string
bool CompareChannels(const char* values) {
    for (int i = 0; i < 8; ++i) {
        if (channels[i] != values[i]) {
            return false; // Return false if any characters don't match
        }
    }
    return true; // Return true if all characters match
}

// Method to set the values of gestureIdentified, gestureUnidentified, and gestureError
void SetGestureFlags(bool identified, bool unidentified, bool error) {
    gestureIdentified = identified;
    gestureUnidentified = unidentified;
    gestureError = error;
}

/**
  * @brief Enable clock for required peripherals.
  * @retval None
  */
void ClockLogic(void)
{
		// Enable clock for GPIOA (user btn)
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	
    // Enable clock for GPIOB (TX and RX pins for UART)
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    // Enable clock for USART3 (UART obviously)
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    // Enable clock for GPIOC (LEDs)
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
}

//configuring UART for use
void UARTConfig(void)
{
	// Pins on board for this are PB10(TX) and PB 11(RX), other pins work but those are close to ground 
	// *note for user remember to use putty, also use alternate function mode 4 (0100)
	GPIOB->AFR[1] &=~ ((1<<8)|(1<<9)|(1<<11)|(1<<12)|(1<<13)|(1<<15)); // alternate function on pb 10 and 11
	GPIOB->AFR[1] |= ((1<<10)|(1<<14));
	// set frequency to 115200
	USART3->BRR = 69; // actual is 69.44 (8000000/115200), actual baud rate now is 115942
	// Usart control register enable 
	USART3->CR1 |= ((1<<2)|(1<<3)); // enable reciever and transmitter  	
}

void EnableUART(void)
{
	USART3->CR1 |= (1<<0); // enable USART
}


void UARTBegin(void)
{
		userInput = 0; // reset user input
    int inputIndex = 0; // index for storing user inputs in the array
    char userInputArray[8]; // array to store user inputs

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
			
		// Check if userInputArray matches the string "start."
    if (compareStrings(userInputArray, "start")) {	
			if(!calibrationStarted){ // if "start" selected and not previously calibrated
				GPIOC->ODR |= (1<<9); // turn green LED on
				GPIOC->ODR &= ~(1<<8); // turn off orange LED (in case previous gestures tested)
				GPIOC->ODR &= ~(1<<7); // turn off blue LED (we already know its reset)
				CharReader(errorMsgGreenOn); // prints that green is on
				calibrationStarted = true; // condition for "test" to be used
			} else {
					CharReader(errorMsgCalibrated); // cant calibrate twice in a row
				}
    }else if (compareStrings(userInputArray, "test")) {	
			if(calibrationStarted){ // turn on orange LED if "test" selected
				GPIOC->ODR &= ~(1<<9); // turn off green LED if "start" previously uswed
				GPIOC->ODR |= (1<<8); // turn on orange LED
				GPIOC->ODR &= ~(1<<7); // turn off blue LED (we already know its reset)
				CharReader(errorMsgOrangeOn); // prints that orange is on
				testStarted = true;
			} else {
					CharReader(errorMsgNotCalibrated); //cant 'test.' gestures before you 'start.' calibration
				}
    } 
		else {
			// If neither 'start.' nor 'test.' entered, you get this message!
			CharReader(errorMsgBadArgument);
    }
		
		// Clear the userInputArray
    for (int i = 0; i < sizeof(userInputArray); ++i) {
        userInputArray[i] = '\0'; // Set each element to '\0' (null character)
		}
		// clear the user input
		userInput = 0;
		HAL_Delay(10); // small delay because baud rate is faster than board
}


void TransmitCharacter (char input) {
	uint32_t TXEmask = (1<<7);
  while ((USART3->ISR & TXEmask) == 0) { // checking if flag is up by looking at the TXE bit
	}
	USART3->TDR = input;
}

void CharReader(char string[]) {
    int i;
    int length = 0;
		int currLineDiff = MAX_STRING_LENGTH - countNonEmptyIndices(userInputArray);

    // Calculate the length of the string
    for (i = 0; string[i] != '\0'; i++) {
        length++;
    }

    // Transmit characters from the input string
    for (i = 0; i < length; i++) {
        TransmitCharacter(string[i]);
    }

    // Pad the string with space characters if its length is less than 189 characters
    if (length < currLineDiff) {
        for (i = length; i < currLineDiff; i++) {
            TransmitCharacter(' ');
        }
    }
}

char* arrayToString(unsigned long* arr, int size) {
    // Allocate memory for the string
    char* str = (char*)malloc((size * 12) * sizeof(char)); // Assuming max unsigned long size is 10 digits + 1 comma + 1 space + 1 null terminator

    if (str == NULL) {
        PrintMessage("Array To String: memory allocation failed");
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

    return str;
}

void printArray(unsigned long* arr, int size) {
    // Convert array to string
    char* str = arrayToString(arr, size);
    if (str == NULL) {
			PrintMessage("Printing array: memory allocation failed");
      return; // Memory allocation failed
    }

    // Print the resulting string
		PrintMessage(str);

    // Free allocated memory
    free(str);
}

int countNonEmptyIndices(char array[]) {
    int count = 0;
    
    // Iterate through the array and count non-empty elements
    for (int i = 0; i < 8; i++) {
        if (array[i] != '\0') {
            count++;
        }
    }   
    return count;
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
	//KALMAN_VALS[0] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
		
	// Channel 2
	ADC_Select_CH2();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[1] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	//KALMAN_VALS[1] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	// Channel 3
	ADC_Select_CH3();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[2] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	//KALMAN_VALS[2] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	// Channel 4
	ADC_Select_CH4();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[3] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	//KALMAN_VALS[3] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	// Channel 5
	ADC_Select_CH5();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	KALMAN_VALS[4] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	//KALMAN_VALS[4] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	// Channel 6
	ADC_Select_CH6();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[5] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	//KALMAN_VALS[5] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	// Channel 7
	ADC_Select_CH7();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000); 
	KALMAN_VALS[6] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	//KALMAN_VALS[6] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	// Channel 8
	ADC_Select_CH8();
	HAL_ADC_Start(&hadc);
	HAL_ADC_PollForConversion(&hadc, 1000);
	KALMAN_VALS[7] = kalman_filter((unsigned long)HAL_ADC_GetValue(&hadc));
	//KALMAN_VALS[7] = (unsigned long)HAL_ADC_GetValue(&hadc);
	HAL_ADC_Stop(&hadc);
	
	int arrSize = sizeof(KALMAN_VALS) / sizeof(KALMAN_VALS[0]);
	//printArray(KALMAN_VALS, arrSize);
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
  hadc.Init.Resolution = ADC_RESOLUTION_12B; // using 12-bit resolution, can switch to 8-bit if not very clear!!
	hadc.Init.ScanConvMode = ENABLE;
	hadc.Init.ContinuousConvMode = ENABLE;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	// missing number of conversions function, should be set to 1 from default 8
	hadc.Init.DMAContinuousRequests = DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }
}

// *** If problems, switch all ADC_SELECT method's rank to 1. if still problems, mess with sampling time. ***
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

/* USER CODE BEGIN 4 */

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
