#define COM3BASE 0x3E8
#define COM4BASE 0x2E8
#define TXDATA COM3BASE
#define LCR (COM3BASE+3) /*0x3F8 line control*/
#define LSR (COM3BASE+5) /*0x3FD line status*/
#include<conio.h>
#include<dos.h>
#include<stdio.h>
#include<string.h>

#define framelen 8

void setup_serial();
void send_character(char ch);
char get_character();

int main(){
setup_serial();

/* sender */
int i = 0, framesend = 0;
char ack;
char addrFILE[100] , frame[framelen] = {0};
int end = 0;
printf("enter file name : ");
gets(addrFILE);

FILE *ptraddr;
ptraddr = fopen(addrFILE, "r");
if(!ptraddr){ // can't find the file
    printf("can't access file");
}
/* send file name */
for(i = strlen(addrFILE); i > 0; i--){
    if(addrFILE[i] == '/'){
        i++;
        break;
    }
}
while(i < strlen(addrFILE)){
    send_character(addrFILE[i]);
    i++;
}
send_character('\0');

while(1){
    printf("enter frame : ");
    printf("%d\n", framesend);
    i = 0;
    /* read file */
    do{
        frame[i] = fgetc(ptraddr);
        if(frame[i] == EOF){
            frame[i] = 6;
            break;
        }
        i++;
    }while(i < framelen);

    do{

        /* send frame number */
        send_character(framesend);

        /* send data */
        /* frame send size = 8 */
        for(i = 0; i < framelen; i++){
            send_character(frame[i]);
            if(framesend[i] == 6){
                break;
            }
        }
        send_character('\0');

        /* recieve ack */
        print("Timeout : ");
        ack = getch();
        printf("%c\n", ack);
        if(ack != 't'){
            printf("recieve ACK%d\n\n", framesend+1);
        }
        else {
            printf("Retransmit frame %d\n", framesend);
        }

        if(get_character() == 6){
            printf("send complete");
            getch();
            return 0;
        }
    }while(ack == 't');
    framesend ^= 1;
}

}

void setup_serial(){
	outportb(LCR, 0x80);
	outportb(TXDATA, 0x0C);
	outportb(TXDATA + 1, 0x00);
	outportb(LCR, 0x0A);
}

void send_character(char ch){
	int status;
	do{
		status = inportb(LSR) & 0x40;
	}while(status != 0x40);
	outportb (TXDATA, ch);
}

char get_character(){
	int status;
	do{
		status = inportb(LSR) & 0x01;
	}while(status != 0x01);
return(inportb(TXDATA));
}