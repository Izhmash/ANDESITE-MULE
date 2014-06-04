#include "Arduino.h"
#include "AT.h"

//Makes AT command without parameters
void ATRemoteRequest::atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY, 
	int frameId, int remoteCommandOptions, char atCommand[])
{
	byte packet[] = {Shared().getStart(), MSB, LSB, FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, remoteCommandOptions,
		atCommand[0], atCommand[1], CHECKSUM}; //packet frame, the 0s are adresses to be added later

	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	breakUp(xBeeSH, sh, shLength);
	breakUp(xBeeSL, sl, slLength);

	for(int i = 0; i < shLength; i++) {
		packet[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++)
	{
		packet[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packet[i + 13] = my[i];
	}

	//calculates and inserts length of the packet length
	packet[Shared().getLSBIndex()] = Shared().getLSB(sizeof(packet));

	//calculates and inserts checksum of the packet
	packet[sizeof(packet) - 1] = Shared().getChecksum(packet, sizeof(packet));

	for(int i = 0; i < sizeof(packet); i++) {
		atPacket[i] = packet[i];
	}
}

//Makes AT command with parameters
void ATRemoteRequest::atRemoteCommand(byte atPacket[], long xBeeSH, long xBeeSL, int xBeeMY,
	int frameId, int remoteCommandOptions, char atCommand[], char parameters[])
{
	byte packetFrame[] = {Shared().getStart(), MSB, LSB, FRAME_TYPE, frameId, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, remoteCommandOptions,
		atCommand[0], atCommand[1]}; //packet frame, the 0s are adresses to be added later

	//split up and put 64-bit address in
	//need to use two parts because it's 8 bytes but a long is 4 bytes
	int shLength = 4;
	int slLength = 4;
	byte sh[shLength];
	byte sl[slLength];
	breakUp(xBeeSH, sh, shLength);
	breakUp(xBeeSL, sl, slLength);

	for(int i = 0; i < shLength; i++) {
		packetFrame[i + 5] = sh[i];
	}

	for(int i = 0; i < slLength; i++) {
		packetFrame[i + 9] = sl[i];
	}

	//Splits up and puts the 16-bit address into the frame
	int myLength = 2;
	byte my[myLength];
	breakUp(xBeeMY, my, myLength);
	for(int i = 0; i < myLength; i++) {
		packetFrame[i + 13] = my[i];
	}

	//transfer everything from packetFrame to packet
	int frameLength = sizeof(packetFrame);
	int packetLength = frameLength + sizeof(parameters) + 1;
	byte packet[packetLength];
	for(int i = 0; i < frameLength; i++) {
			packet[i] = packetFrame[i];
		}

	for(int i = frameLength; i < packetLength - 1; i++) {
		packet[i] = parameters[i - frameLength];
	}

	//calculates and inserts length of the packet length
	packet[Shared().getLSBIndex()] = Shared().getLSB(sizeof(packet));

	//calculates and inserts checksum of the packet
	packet[sizeof(packet) - 1] = Shared().getChecksum(packet, sizeof(packet));

	for(int i = 0; i < sizeof(packet); i++) {
		atPacket[i] = packet[i];
	}
}