#include <video.h>

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base);

static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * const endVideo = (uint8_t*)0xB8FA0;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;

void ncPrint(const char * string)
{
	int i;

	for (i = 0; string[i] != 0; i++)
		ncPrintChar(string[i]);
}

void ncPrintln(const char * string) {
	ncPrint(string);
	ncNewline();
}

void ncPrintColor(const char * string, char color) {
	int i;
	for (i = 0; string[i] != 0; i++)
		ncPrintColorChar(color, string[i]);
}

void ncPrintlnColor(const char * string, char color) {
	ncPrintColor(string, color);
	ncNewline();	//\n without coloring
}

void ncPrintChar(char character)
{
	if(character == '\n') {
		ncNewline();
	}
	else if(character == '\b')	{
		ncBackspace();
	}
	else {
		*currentVideo = character;
		currentVideo += 2;
	}
	if(currentVideo >= endVideo) {
		ncScroll();
		currentVideo = video+(width*2*(height-1));
	}
}

void ncPrintColorChar(char character, char color)
{
	if(character == '\n') {
		do 
		{
			*(currentVideo++) = ' ';
			*(currentVideo++) = color;
		}
		while((uint64_t)(currentVideo - video) % (width * 2) != 0);
	}
	else {
		*(currentVideo++) = character;
		*(currentVideo++) = color;
	}
	if(currentVideo >= endVideo) {
		ncScroll();
		currentVideo = video+(width*2*(height-1));
	}
}

void ncNewline()
{
	do 
	{
		ncPrintChar(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncBackspace() {
	if(currentVideo > video) {
		currentVideo -= 2;		//Goes back if necessary
	}
	*currentVideo = 0;		//Clears char
}

void ncPrintDec(uint64_t value)
{
	ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value)
{
	ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value)
{
	ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base)
{
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear()
{
	int i = 0;

	while(i < height*width*2) {
		video[i++] = ' ';
		video[i++] = 0x07;
	}
	currentVideo = video;
}

void ncScroll() {
	ncScrollLines(1);
}

void ncScrollLines(uint8_t lines) {
	if(lines > height) {
		ncClear();
		return;
	}
	uint32_t current = (width*2*lines),
				end = (width*2*height)-2-current,
				i;
	//Move content up
	for(i = 0; video+current < endVideo; current += 2, i += 2) {
		video[i] = video[current];
	}
	//Clear bottom
	for(i = (width*2*height)-2; i > end; i -= 2) {
		video[i] = 0;
	}
}

uint32_t caretPosition() {
	return (currentVideo-video)/2;
}

static uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base)
{
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}