# ECE 438 HW2: Transport Layer, TCP, UDP

Name: Shuyue Lai

NetID: shuyuel2



### (1) 

a) False. When sender does not send package successfully while the NACK drops during transmission, the sender will not know the situation and will not resent the missing package.

b) False. The next time CW will become 16+1=17.

c) True. Transport layer logical communication between processes.

d) False. Sender don't need to retransmit packets which have been received. For example, sender successfully sent package 30, 32 and 33. However, package 31 is missing. Once sender successfully resent package 31, receiver will send back ACK 33, which means package 32 and 33 don't need to be send again.

e) True.  If no timeout or dupACK occurs, CW will not decrease.

f) True. Since sender will move cw only when it received ACK from receiver. Thus, the lower end of the TCP transmitter's window will not larger than that of  receiver's.

g) False. Wireless environment is different from traditional environment, because that both congestion and unreliable  wireless channel can cause packet missing.  Thus, some notification should be made.

h)  False. It's possible that the sender resent the package before receiving ACK. Thus, the sender send a package again while receiver actually has already successfully received that package and move cw. Therefore, the receiver still need to send back ACK to notify the sender that the package successfully received. 




### (2) 

a) 

- t1: CW = 2 (CW = 1+ 1)
- t2: CW = 4 (Because received A3, which means both P2 and P3 are successfully received)
- t3: CW = 4
- A3: send package P4, P5, P6, P7
- A2: since after receiving A3, P3, P4, P5, P6 have been sent. There is nothing to do now.

b)

- t1: CW = 3
- t2: CW = 1(Since timeout before)
- After timeout: SST = floor(3/2) = 1 CW = SST = 1; then the sender will resend P3
- Since P3 has not been received successful, the last two ACK is A2 and dupACKcount will add 1 each time  when the sender received the A2 and become 2.

c) 

- t1: CW = 12 (Since A10, A11, A12 are lost and A13 is received by sender, CW = 8 + 4 = 12)
- After receiving A13, sender will send package [14 - 25].

d) 

- t1: CW = 16 + 2 = 18 ((Because received A21, which means bothP20 and P21 are successfully received)
- t2: CW = 19
- t3: CW = 20
- t4: CW = 20
- After receiving A21, will send P24-39. Though CW is 18, since P22 and P23 have been sent.
- After receiving A22, will send P40 and P41. Since others have been sent.
- After receiving A23, will send P42 and P43. Since others have been sent.
- After receiving A20, send nothing. 

e) 

- t1: CW = 10 + 1 = 11
- t2: CW = 1 (Since timeout and cw should be 1. Then it received a duplicate ACK as A5, cw keeps the same)
- After receiving A5: assume it is from sending P5. The window will be 6 - 16. Since P6-9 have been sent. The sender will send P10-16.

f) 

- t1: CW = 5 + 1/5 = 5.2
- t2: CW = 5.2 + 1/floor(5.2) = 5.4
- t3: CW = 5.4 + 2/floor(5.4) = 5.8
- t4: CW = 5.8
- t5: CW = 5.8 + 1/floor(5.8) = 6
- After receiving A10: send P15
- After receiving A11: send P16
- After receiving A12: send P17 and P18
- After  receiving A13: send P19 and P20