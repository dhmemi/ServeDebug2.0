#ifndef COMMUNICATEDEFINE_H
#define COMMUNICATEDEFINE_H
//ins communicate with downServr

//Slot control
#define     NEED_REPlY              true
#define     NO_REPLY                false

/*A&Q Packet Instruction Set*/
#define     RECEIVE_OK              0x00 //receive ok    //NO_REPLY
#define     LENGTH_Error            0x01 //data overflow //NO_REPLY
#define     CHECK_Error             0x02 //check error   //NO_REPLY

/*-----------------PC command packet instruction set------------------------*/
#define     UploadServoConnection   0x03 //Get down machine information
#define     UploadServoPosition     0x04 //get digital servo current position(Digital_UBT12HB-only)
#define     EnableDigitalServo      0x05 //enable or disenable digital servo(Digital_UBT12HB-only)
#define     DisableDigitalServo     0x06 //disenable all digital servo(dDigital_UBT12HB-only)
#define     MoveServo               0x07 //move someone servo //one frame

/*Commands about Action */
#define     DownloadAction          0x08 //download action file
#define     ExecuteAction           0x09 //execute action
#define     DeleteAction            0x0a //delete action file
#define     ReNameAction            0x0b //rename action(oldLength, newLength, oldName, newName)
#define     UploadActionNames       0x0c //upload file name in flash
/*Commands about sequence */
#define     DownloadSequence        0x0d //download sequence file
#define     ExecuteSequence         0x0e //execute sequence
#define     UpLoadSequNames         0x0f //upload sequence file names
#define     UpLoadSequence          0x10 //upload sequence file accoding to special name(sequence name)
#define     RenameSequence          0x11 //rename sequencefile(oldLength, newLength, oldName, newName)
#define     DeleteSequence          0x12 //delete sequence

/*------------------down command packet instruction set---------------------*/
#define     ReplyServoConnection    0x13
#define     ReplyServoPosition      0x14
#define     ReplyActionNames        0x15
#define     ReplySequNames          0x16
#define     ReplySequContants       0x17 //reply sequence file accoding to special name

#define     GOOD_MESSAGE            0x18 //data : good message string
#define     BAD_MESSAGE             0x19 //data : bad message string

/**
 * command number
 * next command number = (current+1) % 0x100
 */
//#define     CMD_NUMBER_INIT         0x00 //

/***************************************************************
 * serises data:
 * begin: 0xFF 0xCN 0xLH 0xDH 0xLN 0xLn 0xCD 0xDATA... 0xXX
 * msgs : 0xFF 0xCN 0xLH 0xDH 0xLN 0xLn 0xCD 0xDATA... 0xXX
 * end  : 0xFF 0xCN 0xLH 0xDH 0x00 0x02 0xCD 0xFF 0xFF 0xXX
 */
/**
 * Reply servo connection:
 * begin: 0xFF 0xCN 0xLH 0xDH 0x00 0x02 0x13 0xST 0xSN 0xXX
 * msgs : 0xFF 0xCN 0xLH 0xDH 0xLN 0xLn 0x13 0xID 0xPN 0xPn 0xname... 0xXX (SN msgs)
 * end  : 0xFF 0xCN 0xLH 0xDH 0x00 0x02 0x13 0xFF 0xFF 0xXX
 */
/**
 *Reply servo position:
 * 0xFF 0xCN 0xLH 0xDH 0xLN 0xLn 0x14 0xST 0xSN 0xID 0xPN 0xPn 0xID 0xPN 0xPn... 0xXX
 */
/**
 * upload sequence file or download file
 * begin: 0xFF 0xCN 0xLH 0xDH 0xLN 0xLn 0x08 0xMN 0xMn 0xfileName 0xXX
 * msg  : 0xFF 0xCN 0xLH 0xDH 0xLN 0xLn 0x08 0xDATA... 0xXX (0xMNMn msgs)
 * end  : 0xFF 0xCN 0xLH 0xDH 0x00 0x02 0x08 0xFF 0xFF 0xXX
 */
 /**...................................................................
 * CN : command number;
 * CD : command ID;
 * LH : local host;
 * DH : down host;
 * LN : dataLength/0x100;
 * Ln : dataLength%0x100;
 * ST : servo type
 * SN : servo number
 * PN : (servo position)/0x100
 * Pn : (servo position)%0x100
 * MN : serises message numbers (except the begin message and the end message)/0x100
 * Mn : serises message numbers (except the begin message and the end message)%0x100
 * XX : check pos;
 ************************************************************************/

#endif // COMMUNICATEDEFINE_H
