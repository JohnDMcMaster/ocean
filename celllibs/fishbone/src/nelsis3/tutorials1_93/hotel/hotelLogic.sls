extern network na210 (terminal A, B, Y)
extern network no210 (terminal A, B, Y)
extern network na310 (terminal A, B, C, Y)
extern network no310 (terminal A, B, C, Y)
extern network iv110 (terminal A, Y)

network hotelLogic(terminal s, freeze, q0, q1, q0new, q1new)
{
{inst1}  na210(s, q0, q0new_1);
{inst2}  na210(freeze, q0, q0new_2);
{inst3}  na310(s_n, freeze_n, q1, q0new_3);
{inst4}  na310(q0new_1, q0new_2, q0new_3, q0new);
{inst5}  no210(freeze_n, q1_n, h_1);
{inst6}  no210(q0, q1_n, h_2);
{inst7}  no210(h_1, h_2, q1new_1);  
{inst8}  na310(s_n, q0, q1, q1new_2);            
{inst9}  na310(s, freeze_n, q0_n, q1new_3);          
{inst10} na310(q1new_1, q1new_2, q1new_3, q1new);
{inst11} iv110(s, s_n);
{inst12} iv110(freeze, freeze_n);
{inst13} iv110(q0, q0_n);
{inst14} iv110(q1, q1_n);
}
