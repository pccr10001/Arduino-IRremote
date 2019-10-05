#include "IRremote.h"
#include "IRremoteInt.h"

//==============================================================================
//                      TTTTTTT EEEEEEE  CCCCC   OOOOO  
//                        TTT   EE      CC    C OO   OO 
//                        TTT   EEEEE   CC      OO   OO 
//                        TTT   EE      CC    C OO   OO 
//                        TTT   EEEEEEE  CCCCC   OOOO0  
//==============================================================================

#define TECO_BITS          31
#define TECO_HDR_MARK    4700
#define TECO_HDR_SPACE   2300

#define TECO_BIT_MARK     300
#define TECO_ONE_SPACE   2300
#define TECO_ZERO_SPACE   900
#define TECO_RPT_SPACE   2300

//+=============================================================================
#if SEND_TECO
void  IRsend::sendTeco (unsigned long data,  int nbits)
{
	// Set IR carrier frequency
	enableIROut(38);

	// Header
	mark(TECO_HDR_MARK);
	space(TECO_HDR_SPACE);

	// Data
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
		if (data & mask) {
			mark(TECO_BIT_MARK);
			space(TECO_ONE_SPACE);
		} else {
			mark(TECO_BIT_MARK);
			space(TECO_ZERO_SPACE);
		}
	}

	// Footer
	mark(TECO_BIT_MARK);
	space(0);  // Always end with the LED off
}
#endif


#if DECODE_TECO
bool  IRrecv::decodeTeco (decode_results *results)
{
	long  data   = 0;  // We decode in to here; Start with nothing
	int   offset = 1;  // Index in to results; Skip first entry!?

	// Check header "mark"
	if (!MATCH_MARK(results->rawbuf[offset], TECO_HDR_MARK))  return false ;
	offset++;

	// Check we have enough data
	if (irparams.rawlen < (2 * TECO_BITS) + 1)  return false ;

	// Check header "space"
	if (!MATCH_SPACE(results->rawbuf[offset], TECO_HDR_SPACE))  return false ;
	offset++;

	// Build the data
	for (int i = 0;  i < TECO_BITS;  i++) {
		// Check data "mark"
		if (!MATCH_MARK(results->rawbuf[offset], TECO_BIT_MARK))  return false ;
		offset++;
        // Suppend this bit
		if      (MATCH_SPACE(results->rawbuf[offset], TECO_ONE_SPACE ))  data = (data << 1) | 1 ;
		else if (MATCH_SPACE(results->rawbuf[offset], TECO_ZERO_SPACE))  data = (data << 1) | 0 ;
		else                                                            return false ;
		offset++;
	}

	// Success
	results->bits        = TECO_BITS;
	results->value       = data;
	results->decode_type = TECO;

	return true;
}
#endif
