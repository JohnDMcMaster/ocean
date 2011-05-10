/* * * * * * * * * * * * * * * * 
 *
 * Fishbone extended cell library. 8-1993
 * by Paul Stravers, Patrick Groeneveld
 *
 * Contents: Contains the sls circuit description 
 *           of all cells in the 'exlib' library.
 * Purpose:  To create the exlib library.
 * Created:  july 18th, 1993 by Patrick Groeneveld
 * Modified: aug 11th, 1993 by Patrick Groeneveld
 * Remark:   ingore the warnings of inconnected vss and
 *           vdd terminals
 */

/* * * * * 
 *
 * Part 1: Original circuit cells.
 *         These cell may contain functional decrsiptions
 *         to speed up simulation.
 */
network buf20 (terminal A, Y, vss, vdd)
{
    @ and tr=600p tf=750p (A, Y);
}

network dfn102 (terminal D, CK, Q, QN, vss, vdd)
{
    @ invert (D, 1);
    @ and (1, 8, 2);
    @ nor tr=200p tf=200p (2, 6, 3);
    @ and (3, ck, 4);
    @ nor (4, q, QN);
    @ and (D, 8, 5);
    @ nor tr=200p tf=200p (3, 5, 6);
    @ and (6, ck, 7);
    @ nor (QN, 7, q);
    @ invert (ck, 8);
    @ invert tr=300p tf=200p (QN, Q);
    @ and tr=1.2n tf=1.2n (CK, ck);
}

network dfr112 (terminal D, R, CK, Q, QN, vss, vdd)
{
    @ invert (D, dn);
    @ nor tr=200p tf=200p (dn, R, dd);
    @ invert (dd, 1);
    @ and (1, 8, 2);
    @ nor tr=200p tf=200p (2, 6, 3);
    @ and (3, ck, 4);
    @ nor (4, q, QN);
    @ and (dd, 8, 5);
    @ nor tr=200p tf=200p (3, 5, 6);
    @ and (6, ck, 7);
    @ nor (QN, 7, q);
    @ invert (ck, 8);
    @ invert tr=300p tf=200p (QN, Q);
    @ and tr=1.2n tf=1.2n (CK, ck);
}

network add10 (terminal A, B, CI, CO, Y, vss, vdd)
{
   {na1} @ nand tr=300p tf=200p (A, B, t1);
   {ex1} @ exor tr=500p tf=500p (A, B, t2);
   {na2} @ nand tr=300p tf=200p (t2, CI, t3);
   {na3} @ nand tr=700p tf=500p (t1, t3, CO);
   /* in the layout, Y is buffered with buf20: */
   {ex2} @ exor tr=500p tf=500p (t2, CI, Y);
}

network buf100 (terminal A, Y, vss, vdd)
{
   @ and tr=600p tf=700p (A, Y);
}

network itb20 (terminal A, EN, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (EN, 21, vss);
    penh w=29.6u l=1.6u (EN, 21, vdd);
    nenh w=23.2u l=1.6u (A, 15, vss);
    penh w=29.6u l=1.6u (A, 16, vdd);
    cap 33.35232f (15, vss);
    nenh w=23.2u l=1.6u (EN, 15, Y);
    cap 94.62592f (16, vss);
    penh w=29.6u l=1.6u (21, 16, Y);
    nenh w=23.2u l=1.6u (EN, Y, 26);
    cap 37.89234f (EN, vss);
    penh w=29.6u l=1.6u (21, Y, 27);
    cap 172.783f (21, vss);
    cap 149.8887f (Y, vss);
    cap 36.78832f (26, vss);
    nenh w=23.2u l=1.6u (A, 26, vss);
    cap 98.06192f (27, vss);
    penh w=29.6u l=1.6u (A, 27, vdd);
    cap 39.126f (A, vss);
    cap 116.3673f (vss, vss);
    cap 245.069f (vdd, vss);
}


/* * * * * * * 
 *
 * Part 2: Extracted cells 
 *         These cells start with capitals
 *         They contain only transistors, 
 *         resistors and capacitors
 */

network Buf20 (terminal A, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 11, vss);
    penh w=29.6u l=1.6u (A, 11, vdd);
    nenh w=46u l=1.6u (11, Y, vss);
    penh w=60u l=1.6u (11, Y, vdd);
    cap 300f (11, vss);
    cap 350f (Y, vss);
}

network Dfn102  (terminal D, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (D, 5, vss);
    penh w=29.6u l=1.6u (D, 5, vdd);
    cap 16.79064f (D, vss);
    nenh w=23.2u l=1.6u (47, 5, 19);
    cap 145.1766f (5, vss);
    penh w=29.6u l=1.6u (CK, 5, 19);
    nenh w=23.2u l=1.6u (CK, 14, 19);
    penh w=29.6u l=1.6u (47, 14, 19);
    nenh w=23.2u l=1.6u (24, 14, vss);
    cap 161.0561f (14, vss);
    penh w=29.6u l=1.6u (24, 14, vdd);
    nenh w=23.2u l=1.6u (19, 24, vss);
    penh w=29.6u l=1.6u (19, 24, vdd);
    cap 169.0782f (19, vss);
    nenh w=23.2u l=1.6u (CK, 24, 38);
    cap 174.5596f (24, vss);
    penh w=29.6u l=1.6u (47, 24, 38);
    nenh w=23.2u l=1.6u (47, 33, 38);
    penh w=29.6u l=1.6u (CK, 33, 38);
    nenh w=23.2u l=1.6u (QN, 33, vss);
    cap 161.0561f (33, vss);
    penh w=29.6u l=1.6u (QN, 33, vdd);
    nenh w=23.2u l=1.6u (38, QN, vss);
    penh w=29.6u l=1.6u (38, QN, vdd);
    cap 163.3281f (38, vss);
    nenh w=23.2u l=1.6u (CK, 47, vss);
    cap 258.4742f (47, vss);
    penh w=29.6u l=1.6u (CK, 47, vdd);
    cap 122.442f (CK, vss);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    cap 194.1671f (QN, vss);
    cap 133.7273f (Q, vss);
    cap 234.5542f (vss, vss);
    cap 491.4977f (vdd, vss);
}

network Dfr112 (terminal D, R, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (R, 9, vss);
    penh w=29.6u l=1.6u (R, 9, vdd);
    cap 15.69586f (R, vss);
    nenh w=23.2u l=1.6u (9, 14, vss);
    penh w=29.6u l=1.6u (9, 19, vdd);
    cap 153.8717f (9, vss);
    cap 32.1792f (14, vss);
    nenh w=23.2u l=1.6u (D, 14, 19);
    penh w=29.6u l=1.6u (D, 19, vdd);
    cap 16.24325f (D, vss);
    nenh w=23.2u l=1.6u (61, 19, 33);
    cap 243.426f (19, vss);
    penh w=29.6u l=1.6u (CK, 19, 33);
    nenh w=23.2u l=1.6u (CK, 28, 33);
    penh w=29.6u l=1.6u (61, 28, 33);
    nenh w=23.2u l=1.6u (38, 28, vss);
    cap 161.0561f (28, vss);
    penh w=29.6u l=1.6u (38, 28, vdd);
    nenh w=23.2u l=1.6u (33, 38, vss);
    penh w=29.6u l=1.6u (33, 38, vdd);
    cap 169.0782f (33, vss);
    nenh w=23.2u l=1.6u (CK, 38, 52);
    cap 174.5596f (38, vss);
    penh w=29.6u l=1.6u (61, 38, 52);
    nenh w=23.2u l=1.6u (61, 47, 52);
    penh w=29.6u l=1.6u (CK, 47, 52);
    nenh w=23.2u l=1.6u (QN, 47, vss);
    cap 161.0561f (47, vss);
    penh w=29.6u l=1.6u (QN, 47, vdd);
    nenh w=23.2u l=1.6u (52, QN, vss);
    penh w=29.6u l=1.6u (52, QN, vdd);
    cap 163.3281f (52, vss);
    nenh w=23.2u l=1.6u (CK, 61, vss);
    cap 258.4742f (61, vss);
    penh w=29.6u l=1.6u (CK, 61, vdd);
    cap 122.442f (CK, vss);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    cap 194.1671f (QN, vss);
    cap 133.7273f (Q, vss);
    cap 290.7382f (vss, vss);
    cap 609.4773f (vdd, vss);
}


network Add10 (terminal A, B, CI, CO, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (CI, 1, vss);
    penh w=29.6u l=1.6u (CI, 1, vdd);
    penh w=29.6u l=1.6u (CI, 10, vdd);
    nenh w=23.2u l=1.6u (CI, 2, vss);
    nenh w=23.2u l=1.6u (1, 3, vss);
    penh w=29.6u l=1.6u (1, 4, vdd);
    cap 203.1392f (1, vss);
    penh w=29.6u l=1.6u (15, 10, vdd);
    cap 34.2472f (2, vss);
    nenh w=23.2u l=1.6u (15, 2, 10);
    cap 35.70496f (3, vss);
    nenh w=23.2u l=1.6u (9, 3, 5);
    cap 99.87968f (4, vss);
    penh w=29.6u l=1.6u (15, 4, 5);
    nenh w=23.2u l=1.6u (CI, 5, 6);
    penh w=29.6u l=1.6u (CI, 5, 7);
    cap 93.29594f (CI, vss);
    penh w=29.6u l=1.6u (5, 8, vdd);
    nenh w=23.2u l=1.6u (5, 8, vss);
    cap 187.4038f (5, vss);
    cap 33.60112f (6, vss);
    nenh w=23.2u l=1.6u (15, 6, vss);
    cap 96.97728f (7, vss);
    penh w=29.6u l=1.6u (9, 7, vdd);
    penh l=1.6u w=59.2u (8, Y, vdd);
    nenh l=1.6u w=46.4u (8, Y, vss);
    nenh w=23.2u l=1.6u (15, 9, vss);
    penh w=29.6u l=1.6u (15, 9, vdd);
    cap 180.6604f (8, vss);
    cap 142.1868f (Y, vss);
    cap 168.4069f (9, vss);
    nenh w=23.2u l=1.6u (A, 11, vss);
    penh w=29.6u l=1.6u (A, 11, vdd);
    penh w=29.6u l=1.6u (10, CO, vdd);
    nenh w=23.2u l=1.6u (10, 12, vss);
    cap 173.1535f (10, vss);
    nenh w=23.2u l=1.6u (11, 13, vss);
    penh w=29.6u l=1.6u (11, 14, vdd);
    cap 163.8735f (11, vss);
    penh w=29.6u l=1.6u (19, CO, vdd);
    cap 34.79616f (12, vss);
    nenh w=23.2u l=1.6u (19, 12, CO);
    cap 35.70496f (13, vss);
    nenh w=23.2u l=1.6u (20, 13, 15);
    cap 100.1768f (14, vss);
    penh w=29.6u l=1.6u (B, 14, 15);
    cap 138.7009f (CO, vss);
    nenh w=23.2u l=1.6u (A, 15, 16);
    cap 259.6281f (15, vss);
    penh w=29.6u l=1.6u (A, 15, 17);
    penh w=29.6u l=1.6u (A, 19, vdd);
    nenh w=23.2u l=1.6u (A, 18, vss);
    cap 100.5079f (A, vss);
    cap 33.60112f (16, vss);
    nenh w=23.2u l=1.6u (B, 16, vss);
    cap 96.10672f (17, vss);
    penh w=29.6u l=1.6u (20, 17, vdd);
    penh w=29.6u l=1.6u (B, 19, vdd);
    cap 33.62304f (18, vss);
    nenh w=23.2u l=1.6u (B, 18, 19);
    nenh w=23.2u l=1.6u (B, 20, vss);
    penh w=29.6u l=1.6u (B, 20, vdd);
    cap 77.93922f (B, vss);
    cap 167.0151f (19, vss);
    cap 176.7056f (20, vss);
    cap 574.968f (vss, vss);
    cap 1.36064p (vdd, vss);
}

network Buf100 (terminal A, Y, vss, vdd)
{
    nenh l=1.6u w=46.4u (A, 56, vss);
    penh l=1.6u w=118.4u (A, 56, vdd);
    cap 51.45411f (A, vss);
    nenh l=1.6u w=139.2u (56, Y, vss);
    penh l=1.6u w=266.4u (56, Y, vdd);
    cap 618.8519f (Y, vss);
    cap 417.9174f (56, vss);
    cap 333.073f (vss, vss);
    cap 768.343f (vdd, vss);
}

network Itb20 (terminal A, EN, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (EN, 21, vss);
    penh w=29.6u l=1.6u (EN, 21, vdd);
    nenh w=23.2u l=1.6u (A, 15, vss);
    penh w=29.6u l=1.6u (A, 16, vdd);
    cap 33.35232f (15, vss);
    nenh w=23.2u l=1.6u (EN, 15, Y);
    cap 94.62592f (16, vss);
    penh w=29.6u l=1.6u (21, 16, Y);
    nenh w=23.2u l=1.6u (EN, Y, 26);
    cap 37.89234f (EN, vss);
    penh w=29.6u l=1.6u (21, Y, 27);
    cap 172.783f (21, vss);
    cap 149.8887f (Y, vss);
    cap 36.78832f (26, vss);
    nenh w=23.2u l=1.6u (A, 26, vss);
    cap 98.06192f (27, vss);
    penh w=29.6u l=1.6u (A, 27, vdd);
    cap 39.126f (A, vss);
    cap 116.3673f (vss, vss);
    cap 245.069f (vdd, vss);
}









