option level = 2 
option sigunit = 4.444442e-09 
option outunit = 10p
plot  IN_0,IN_1,IN_2,IN_3,R,OUT_0,CK
print IN_0,IN_1,IN_2,IN_3,R,OUT_0,CK
/*
*%
tstep 0.2n
trise 0.3n
tfall 0.3n
*%
.options nomod
*%
*/
option simperiod = 720 
set vss = l*720 
set vdd = h*720 
set IN_0 = l*76 h*37 l*11 l*187 h*26 l*118 h*34 l*187 h*29 l*15 
set IN_1 = l*157 h*40 l*155 h*33 l*138 h*76 l*121 
set IN_2 = l*227 h*37 l*47 h*27 l*225 h*66 l*91 
set IN_3 = l*284 h*10 h*107 l*319 
set CK = (h*15 l*15 )*~
set R = h*40 l*680 
