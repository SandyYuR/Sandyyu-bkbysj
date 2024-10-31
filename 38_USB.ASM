;//*******************************************************
;//*文件名:USB.ASM  for  51MCU

;功能: USB 总线通信实验。

;接线: 用8位数据线连接USB接口JD1D到CPU模块的JD8(P1口)。
;//*******************************************************
Z8279    EQU     08701H  ;8279 状态/命令口地址
D8279    EQU     08700H  ;8279 数据口地址
LEDMOD   EQU     00H     ;左边输入 八位字符显示
                         ;外部译码键扫描方式,双键互锁
LEDFEQ   EQU     2FH     ;扫描速率
LEDCLS   EQU     0C1H    ;清除显示 RAM
LEDWR0   EQU     80H     ;设定的将要写入的显示RAM地址

LEDDBuf  EQU	 30H     ;显示第一位的缓冲地址
      
        ORG	  0000H
        LJMP  Main
        ORG	  0100H

Main:  
        LCALL INIT8279
Mloop:       
        MOV	  P1,#0FFH
        MOV	  A,P1         ;  接受USB上位机发送的数据

    	MOV	  R0,#LEDDBuf
    	MOV	  B,A
        ANL   A,#0FH
	    XCH	  A,@R0    	
	    MOV   A,B    	
	    INC	  R0    	
	    SWAP  A
        ANL   A,#0FH
	    XCH	  A,@R0

	    LCALL DISPLAY
        Lcall delay
	    LJMP  Mloop

INIT8279:                       ;8279初始化子程序
        PUSH   DPH              ;保存现场
        PUSH   DPL
        PUSH   ACC
        LCALL  DELAY            ;延时
        MOV    DPTR ,#Z8279
        MOV    A,#LEDMOD        ;置8279工作方式
        MOVX   @DPTR,A
        MOV    A,#LEDFEQ        ;置键盘扫描速率
        MOVX   @DPTR,A
        MOV    A,#LEDCLS        ;清除 LED 显示
        MOVX   @DPTR,A
        LCALL  DELAY            ;延时
        POP    ACC              ;恢复现场
        POP    DPL
        POP    DPH
        RET
;显示字符子程序
;输入: R4,位置 R5,值
DISLED:  PUSH   DPH             ;保存现场
         PUSH   DPL
         PUSH   ACC
         MOV    A,#LEDWR0       ;置显示起始地址
         ADD    A,R4            ;加位置偏移量
         MOV    DPTR,#Z8279
         MOVX   @DPTR,A         ;设定显示位置
         MOV    DPTR,#LEDSEG    ;置显示常数表起始位置
         MOV    A,R5
         MOVC   A,@A+DPTR       ;查表
         MOV    DPTR,#D8279
         MOVX   @DPTR,A         ;显示数据
         POP    ACC             ;恢复现场
         POP    DPL
         POP    DPH
         RET
 
              
DISPLAY:
         MOV   R0,#LEDDBUF
         MOV   R1,#2    
         MOV   R4,#0
NEXTBIT: MOV   A, @R0 
         MOV   R5,A
         LCALL DISLED
         INC   R0
         INC   R4
         DJNZ  R1,NEXTBIT 
         RET               

DELAY:                          ;延时子程序
        PUSH    0               ;保存现场
        PUSH    1
        MOV     0,#0H
DELAY1: MOV     1,#0H
        DJNZ    1,$
        DJNZ    0,DELAY1
        POP     1               ;恢复现场
        POP     0
        RET
        
LEDSEG: DB     3FH,06H,5BH,4FH,66H,6DH,7DH,07H ;'0,1,2,3,4,5,6,7'
        DB     7FH,67H,77H,7CH,39H,5EH,79H,71H ;'8,9,A,B,C,D,E,F'
        DB     00H                             ;' ' 




        END
