extern network hotelLogic(terminal s, freeze, q0, q1, q0new, q1new)
extern network no310 (terminal A, B, C, Y)
extern network dfr11 (terminal D, Q, R, QN, CK)
extern network iv110 (terminal A, Y)

network hotel(terminal s1, s2, s3, freeze, light, ck, reset)
{
{inst1} no310(s1, s2, s3, s_n);
{inst2} iv110(s_n, s);
{inst3} hotelLogic(s, freeze, q0, q1, q0new, q1new);
{inst4} dfr11(q0new, q0, reset, , ck);
{inst5} dfr11(q1new, q1, reset, , ck);
net {q1, light};
}
