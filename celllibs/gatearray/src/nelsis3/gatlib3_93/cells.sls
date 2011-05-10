/* * * * * * * * * * * * * * * * 
 *
 * pm25 basic cell library. 3-1993
 * Patrick Groeneveld
 *
 * Contents: Contains the sls circuit description 
 *           of all cells in the 'oplib' library.
 *           See the library description in the 
 *           Ontwerppracticumhandleiding (TU Delft 
 *           internal report)
 * Purpose:  To create the oplib library.
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

network dfn10 (terminal D, CK, Q, QN, vss, vdd)
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

network dfr11 (terminal D, R, CK, Q, QN, vss, vdd)
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

network ex210 (terminal A, B, Y, vss, vdd)
{
    @ exor tr=900p tf=900p (A, B, Y);
}

network iv110 (terminal A, Y, vss, vdd)
{
    @ invert tr=400p tf=300p (A, Y);
}

network na210 (terminal A, B, Y, vss, vdd)
{
    @ nand tr=300p tf=400p (A, B, Y);
}

network na310 (terminal A, B, C, Y, vss, vdd)
{
    @ nand tr=500p tf=1n (A, B, C, Y);
}

network no210 (terminal A, B, Y, vss, vdd)
{
    @ nor tr=600p tf=300p (A, B, Y);
}

network no310 (terminal A, B, C, Y, vss, vdd)
{
    @ nor tr=1.2n tf=400p (A, B, C, Y);
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
}

network Dfn10 (terminal D, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (D, 43, vss);
    penh w=29.6u l=1.6u (D, 43, vdd);
    nenh w=23.2u l=1.6u (46, 42, 43);
    penh w=29.6u l=1.6u (CK, 42, 43);
    nenh w=23.2u l=1.6u (CK, 42, 45);
    penh w=29.6u l=1.6u (46, 42, 45);
    nenh w=23.2u l=1.6u (44, 45, vss);
    penh w=29.6u l=1.6u (44, 45, vdd);
    nenh w=23.2u l=1.6u (42, 44, vss);
    penh w=29.6u l=1.6u (42, 44, vdd);
    nenh w=23.2u l=1.6u (CK, 41, 44);
    penh w=29.6u l=1.6u (46, 41, 44);
    nenh w=23.2u l=1.6u (46, Q, 41);
    penh w=29.6u l=1.6u (CK, Q, 41);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    nenh w=23.2u l=1.6u (41, QN, vss);
    penh w=29.6u l=1.6u (41, QN, vdd);
    nenh w=23.2u l=1.6u (CK, 46, vss);
    penh w=29.6u l=1.6u (CK, 46, vdd);
}

network Dfr11 (terminal D, R, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (R, 9, vss);
    penh w=29.6u l=1.6u (R, 9, vdd);
    nenh w=23.2u l=1.6u (9, 14, vss);
    penh w=29.6u l=1.6u (9, 57, vdd);
    nenh w=23.2u l=1.6u (D, 14, 57);
    penh w=29.6u l=1.6u (D, 57, vdd);
    nenh w=23.2u l=1.6u (60, 56, 57);
    penh w=29.6u l=1.6u (CK, 56, 57);
    nenh w=23.2u l=1.6u (CK, 56, 59);
    penh w=29.6u l=1.6u (60, 56, 59);
    nenh w=23.2u l=1.6u (58, 59, vss);
    penh w=29.6u l=1.6u (58, 59, vdd);
    nenh w=23.2u l=1.6u (56, 58, vss);
    penh w=29.6u l=1.6u (56, 58, vdd);
    nenh w=23.2u l=1.6u (CK, 55, 58);
    penh w=29.6u l=1.6u (60, 55, 58);
    nenh w=23.2u l=1.6u (60, Q, 55);
    penh w=29.6u l=1.6u (CK, Q, 55);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    nenh w=23.2u l=1.6u (55, QN, vss);
    penh w=29.6u l=1.6u (55, QN, vdd);
    nenh w=23.2u l=1.6u (CK, 60, vss);
    penh w=29.6u l=1.6u (CK, 60, vdd);
}

network Ex210 (terminal A, B, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (B, BN, vss);
    penh w=29.6u l=1.6u (B, BN, vdd);
    nenh w=23.2u l=1.6u (BN, 9, vss);
    penh w=29.6u l=1.6u (B, 28, vdd);
    nenh w=23.2u l=1.6u (AN, 9, Y);
    penh w=29.6u l=1.6u (AN, 28, Y);
    nenh w=23.2u l=1.6u (B, 18, Y);
    penh w=29.6u l=1.6u (BN, 29, Y);
    nenh w=23.2u l=1.6u (A, 18, vss);
    penh w=29.6u l=1.6u (A, 29, vdd);
    nenh w=23.2u l=1.6u (A, AN, vss);
    penh w=29.6u l=1.6u (A, AN, vdd);
}

network Iv110 (terminal A, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, Y, vss);
    penh w=29.6u l=1.6u (A, Y, vdd);
}

network Na210 (terminal A, B, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 5, vss);
    nenh w=23.2u l=1.6u (B, 5, Y);
    penh w=29.6u l=1.6u (A, Y, vdd);
    penh w=29.6u l=1.6u (B, Y, vdd);
}

network Na310 (terminal A, B, C, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 5, vss);
    penh w=29.6u l=1.6u (A, Y, vdd);
    nenh w=23.2u l=1.6u (B, 5, 10);
    penh w=29.6u l=1.6u (B, Y, vdd);
    nenh w=23.2u l=1.6u (C, 10, Y);
    penh w=29.6u l=1.6u (C, Y, vdd);
}

network No210 (terminal A, B, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, Y, vss);
    nenh w=23.2u l=1.6u (B, Y, vss);
    penh w=29.6u l=1.6u (A, 5, vdd);
    penh w=29.6u l=1.6u (B, 5, Y);
}

network No310 (terminal A, B, C, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, Y, vss);
    penh w=29.6u l=1.6u (A, 5, vdd);
    nenh w=23.2u l=1.6u (B, Y, vss);
    penh w=29.6u l=1.6u (B, 5, 10);
    nenh w=23.2u l=1.6u (C, Y, vss);
    penh w=29.6u l=1.6u (C, 10, Y);
}

