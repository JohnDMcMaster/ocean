/* auto_set */
set /* no_edit */ vdd = h*~
set /* no_edit */ vss = l*~
/* begin_disabled
set #* no_edit *# ck = (l*50 h*50)*~
   end_disabled */
option sigunit = 1.000000e-09 
option outacc = 10p
option level = 2 
option simperiod = 2000 
option sta_file = on
/*
*%
tstep 0.1n
trise 0.5n
tfall 0.5n
*%
*%
.options cptime=500
*%
*/
/* auto_print */
/* auto_plot */
option level = 2 
set IN_0 = l*718 h*152 h*155 l*226 h*193 l*257 h*148 l*151 
set IN_1 = l*2000 
set R = h*208 l*1792 
set CK = (l*50 h*50 )*~
print /* auto */ IN_0
plot /* auto */ IN_0
print /* auto */ IN_1
plot /* auto */ IN_1
print /* auto */ OUT_0
plot /* auto */ OUT_0
print /* auto */ R
plot /* auto */ R
print /* auto */ CK
plot /* auto */ CK
print /* auto */ vss
plot /* auto */ vss
print /* auto */ vdd
plot /* auto */ vdd
