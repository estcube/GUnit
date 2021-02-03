#include "FreeRTOS.h"
#include "task.h"

#include "mcp4161_driver.h"
#include "echal_gpio.h"
#include "echal_rcc.h"
#include "common.h"
#include "ecbal.h"

#include "GUnit.h"

/**
 * This unit test goes through all the MCP411 potentiometer functions to test their work.
 * @NOTE: You need to power the 8.4V line for the potentiometer to work!
 * @NOTE: To execute these unit tests GUnit is required!
 */

#if defined(BOARD_SP_PROTOTYPE)
  #define MCP4161_SPI  MPPT_SPI
  #define MCP4161_MISO MPPT_MISO
  #define MCP4161_MOSI MPPT_MOSI
  #define MCP4161_CLK  MPPT_CLK
  #define MCP4161_CS   MPPT_CS
#else
  #error This test is only meant to be run on the engineering models listed above!
#endif

/**
 * Tests if setting and reading resistance works properly.
 */
void test_set_get_resistance(uint8_t sensor_index) {
  /**
   * Set resistance to 95 Ohms
   */
  uint16_t get_resistance;
  uint16_t set_resistance = 95;
  GASSERT(MCP4161_OK, mcp4161_set_resistance(sensor_index, set_resistance));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &get_resistance));
  GASSERT(get_resistance, (set_resistance-1));

  /**
   * Set resistance to 1482 Ohms
   */
  set_resistance = 1482;
  GASSERT(MCP4161_OK, mcp4161_set_resistance(sensor_index, set_resistance));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &get_resistance));
  GASSERT(get_resistance, (set_resistance-1));

  /**
   * Set resistance to the upper limit of 5 kOhms
   */
  set_resistance = 5000;
  GASSERT(MCP4161_OK, mcp4161_set_resistance(sensor_index, set_resistance));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &get_resistance));
  GASSERT(get_resistance, (set_resistance-4));

  /**
   * Set resistance to the lower limit of 75 Ohms
   */
  set_resistance = 75;
  GASSERT(MCP4161_OK, mcp4161_set_resistance(sensor_index, set_resistance));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &get_resistance));
  GASSERT(get_resistance, set_resistance);

  /**
   * Try setting the resistance below the lower limit of 75 Ohms
   */
  set_resistance = 0;
  GASSERT(MCP4161_OUT_OF_RANGE, mcp4161_set_resistance(sensor_index, set_resistance));

  /**
   * Try setting the resistance above the upper limit of 5 kOhms
   */
  set_resistance = 6000;
  GASSERT(MCP4161_OUT_OF_RANGE, mcp4161_set_resistance(sensor_index, set_resistance));
}

/**
 * Test increment and decrement resistance functionality of the potentiometer
 */
void test_increment_decrement(uint8_t sensor_index) {
  uint16_t get_resistance;
  uint16_t initial_resistance;

  //// First incremement step ////
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &initial_resistance));
  GASSERT(MCP4161_OK, mcp4161_increment_resistance(sensor_index));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &get_resistance));
  GASSERT(true, get_resistance > initial_resistance);

  //// Second incremement step ////
  GASSERT(MCP4161_OK, mcp4161_increment_resistance(sensor_index));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &initial_resistance));
  GASSERT(true, get_resistance < initial_resistance);

  //// First decremement step ////
  GASSERT(MCP4161_OK, mcp4161_decrement_resistance(sensor_index));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &initial_resistance));
  GASSERT(true, get_resistance == initial_resistance);

  //// Second decremement step ////
  GASSERT(MCP4161_OK, mcp4161_decrement_resistance(sensor_index));
  GASSERT(MCP4161_OK, mcp4161_get_resistance(sensor_index, &get_resistance));
  GASSERT(true, get_resistance < initial_resistance);
}

/**
 * Enable and disable the potentiometer
 */
void test_enable_disable(uint8_t sensor_index) {
  bool is_enabled;

  GASSERT(MCP4161_OK, mcp4161_disable(sensor_index));
  GASSERT(MCP4161_OK, mcp4161_is_enabled(sensor_index, &is_enabled));
  GASSERT(false, is_enabled);

  GASSERT(MCP4161_OK, mcp4161_enable(sensor_index));
  GASSERT(MCP4161_OK, mcp4161_is_enabled(sensor_index, &is_enabled));
  GASSERT(true, is_enabled);
}

/**
 * Test setting non-volatile reset resistance
 */
void test_reset_resistance(uint8_t sensor_index) {
  uint16_t get_reset_resistance;
  uint16_t set_reset_resistance = 2556;

  GASSERT(MCP4161_OK, mcp4161_set_reset_resistance(sensor_index, set_reset_resistance));
  GASSERT(MCP4161_OK, mcp4161_get_reset_resistance(sensor_index, &get_reset_resistance));
  GASSERT(get_reset_resistance, set_reset_resistance - 1);
}

/**
 * Test initializing the device
 */
void init_wrapper(uint8_t sensor_index, mcp4161_config_t *cfg){
  GASSERT(MCP4161_OK, mcp4161_initialize(sensor_index, cfg));
}

/**
 * Test deinitializing the device
 */
void deinit_wrapper(uint8_t sensor_index){
  GASSERT(MCP4161_OK, mcp4161_deinitialize(sensor_index));
}

/**
 * Test initializing the clocks in the MCU
 */
void rcc_init_wrapper(){
  GASSERT(HAL_OK, hal_rcc_init());
}

/**
 * Arguments needed for the unit test should be defined below
 */
mcp4161_config_t cfg1 = {
  .spi_peripheral_index = MCP4161_SPI,
  .mosi_pin = MCP4161_MOSI,
  .miso_pin = MCP4161_MISO,
  .sck_pin =  MCP4161_CLK,
  .cs_pin =   MCP4161_CS
};

/**
 * GUnit macros for passing arguments to different unit tests
 */
GINLINE_DATA(deinit_wrapper_1, deinit_wrapper, 1);
GINLINE_DATA(init_wrapper_1, init_wrapper, 1, &cfg1);
GINLINE_DATA(test_enable_disable_1, test_enable_disable, 1);
GINLINE_DATA(test_reset_resistance_1, test_reset_resistance, 1);
GINLINE_DATA(test_set_get_resistance_1, test_set_get_resistance, 1);
GINLINE_DATA(test_increment_decrement_1, test_increment_decrement, 1);

/**
 * FreeRTOS task responsible for running the unit tests
 */
static void test_task(void *) {
  GEXECUTE(init_wrapper_1, deinit_wrapper_1, test_set_get_resistance_1);
  GEXECUTE(init_wrapper_1, deinit_wrapper_1, test_increment_decrement_1);
  GEXECUTE(init_wrapper_1, deinit_wrapper_1, test_enable_disable_1);
  GEXECUTE(init_wrapper_1, deinit_wrapper_1, test_reset_resistance_1);
  GEND();
  while(1);
}

int main() {
  GSIMPLE_EXECUTE(rcc_init_wrapper);
  xTaskCreate(test_task, "TEST_TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  vTaskStartScheduler();
  while (1);
}
