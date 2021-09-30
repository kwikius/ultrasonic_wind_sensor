#ifndef ULTRASOUND_FLIGHT_SEQUENCE_H_INCLUDED
#define ULTRASOUND_FLIGHT_SEQUENCE_H_INCLUDED
/*
n.b. internal header
*/
/**
 * @brief set the transducer receive address to listen on after the pulse has been transmitted
**/
void set_receive_address();

/**
 * @brief at the end of a transmit receive sequence set up for the next one
**/
void start_next_flight();

/**
* @brief if no pulse was received in this cycle then set the relevant array entry to 0
**/
void validate_capture();

/**
* @brief Get the newest set of captures if available 
* @return A pointer to the latest entries if valid or null if not available yet
**/
volatile uint16_t const * get_captures();

void clear_capture();

#endif // ULTRASOUND_FLIGHT_SEQUENCE_H_INCLUDED
