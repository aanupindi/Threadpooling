#include <stdint.h>
#include <stdio.h>

#define stateSize 2
#define deltaSize 10
#define timestampSize 18
#define xStart 2
#define xEnd 3
#define yStart 4
#define yEnd 5
#define timeStart 6
#define timeEnd 7
#define UINT18_MAX ((1 << 18) -1)
#define UINT8_SIZE 8

#define XMASK ((1 << (xEnd+1)) -1) & (~0 << (xStart))

#define YMASK ((1 << (yEnd+1)) -1) & (~0 << (yStart))

struct mouseData{
	uint8_t states : stateSize;
	uint16_t deltaX : deltaSize;
	uint16_t deltaY : deltaSize;
	uint32_t timestamp;
	unsigned int previousTimeStamp:timestampSize;
}mData; 

void printReport(){

	printf("states: %d\n",mData.states );
	printf("deltaX: %d\n",mData.deltaX );
	printf("deltaY: %d\n",mData.deltaY );
	printf("timestamp: %d\n",mData.timestamp );
}

//Get the left/right click state
void handleState( uint8_t* buf){
	uint8_t statesMask = (1 << stateSize) - 1;
	mData.states = buf[4] & statesMask;
}

// Handle the change in deltaX and deltaY
void handleDeltas(uint8_t* buf){

	struct {
		uint16_t xMasker:deltaSize;
		uint16_t yMasker:deltaSize;
	}deltaMaskers;

	mData.deltaX = buf[2];
	deltaMaskers.xMasker = (buf[4] & XMASK);
	mData.deltaX |= (deltaMaskers.xMasker << (UINT8_SIZE - xStart));

	mData.deltaY = buf[3];
	deltaMaskers.yMasker = (buf[4] & YMASK);
	mData.deltaY |= (deltaMaskers.yMasker << (UINT8_SIZE - yStart));
}

// Handle the timestamp change.
// Logic: if the timestamp overflows, get the delta since the 
// last timestamp and append it to the total timestamp.
void handleTimeStamp( uint8_t* buf){
	unsigned int mask1 = 0;
	unsigned int mask2 = 0;

	//Get the 2 bits of the MSB and append it to the buf[5] and buf[6]
	uint32_t currentTimeStamp = ( (buf[4] >> timeStart) | ((buf[5] | mask1) << 2) | ((buf[6] | mask2 ) << 10));

	// We know there is an overflow if the current timestamp < previous timestamp
	if(mData.previousTimeStamp > currentTimeStamp){
		//overflow detected. Add to previous timestamp.
		mData.timestamp+= (UINT18_MAX - mData.previousTimeStamp) + currentTimeStamp;
	}
	else{
		mData.timestamp+= (currentTimeStamp - mData.previousTimeStamp);
	}

	//Store the current timestamp since we need it the next time
	mData.previousTimeStamp = currentTimeStamp;

}

//Handle the mouse change data
void handleReport(uint8_t* buffer, uint16_t length){

	if(length < 7 || buffer == NULL){
		fprintf(stderr,"Invalid buffer");
	}

	handleState(buffer);
	handleDeltas(buffer);
	handleTimeStamp(buffer);
}

// main function with test cases.
int main(){
	uint8_t buffer1[] = {0, 0, 20, 30, 101, 4, 5};

	handleReport(buffer1, 7);

	printReport();

	uint8_t buffer2[] = {0, 0, 55, 33, 44, 23, 12};

	handleReport(buffer2,7);

	printReport();

	uint8_t buffer0[] = {};

	handleReport(buffer0, 7);

	return 0;
}
