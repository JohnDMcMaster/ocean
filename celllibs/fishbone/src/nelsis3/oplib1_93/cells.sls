/* * * * * * * * * * * * * * * * 
 *
 * Fishbone basic cell library. 12-1992
 * Anton Bakker, Paul Stravers, Patrick Groeneveld
 *
 * Contents: Contains the sls circuit description 
 *           of all cells in the 'oplib' library.
 *           See the library description in the 
 *           Ontwerppracticumhandleiding (TU Delft 
 *           internal report)
 * Purpose:  To create the oplib library.
 * Created:  dec 22, 1992 by Patrick Groeneveld
 * modified: jan 8, 1993 by Patrick Groeneveld (changed analog cells)
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

network de211 (terminal A, B, Y0, Y1, Y2, Y3, vss, vdd)
{
    @ invert tr=500p tf=500p (A, an);
    @ invert tr=500p tf=500p (B, bn);
    @ nor tr=300p tf=200p (A, B, Y0);
    @ nor tr=300p tf=200p (an, B, Y1);
    @ nor tr=300p tf=200p (A, bn, Y2);
    @ nor tr=300p tf=200p (an, bn, Y3);
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

network ln3x3 (terminal g, d, s, vss, vdd)
{
    nenh l=3.5u w=69.6u (g, d, s);
}

network lp3x3 (terminal g, d, s, vss, vdd)
{
    penh l=3.6u w=88.8u (g, d, s);
}

network mir_nin (terminal in, g, vss, vdd)
{
    nenh l=3.5u w=69.6u (in, g, in);
    nenh l=3.5u w=69.6u (g, vss, g);
}

network mir_nout (terminal in, g, out, vss, vdd)
{
    nenh l=3.5u w=69.6u (in, out, a);
    nenh l=3.5u w=69.6u (g, a, vss);
}

network mir_pin (terminal in, g, vss, vdd)
{
    penh l=3.5u w=88.8u (in, g, in);
    penh l=3.5u w=88.8u (g, vdd, g);
}

network mir_pout (terminal in, g, out, vss, vdd)
{
    penh l=3.5u w=88.8u (in, out, a);
    penh l=3.5u w=88.8u (g, a, vdd);
}

network mu111 (terminal A, B, S, Y, vss, vdd)
{
    @ invert tr=200p tf=200p (S, SN);
    @ invert (SN, S2);
    @ nand (A, SN, 1);
    @ nand (B, S2, 2);
    @ nand tr=1.1n tf=1n (1, 2, Y);
}

network mu210 (terminal S1, S2, A, B, C, D, Y, vss, vdd)
{
    @ invert tr=600p tf=400p (S1, s1n);
    @ invert (s1n, s1);
    @ invert (S2, s2n);
    @ and (s1n, s2n, A, 1);
    @ and (s1, s2n, B, 2);
    @ and (s1n, S2, C, 3);
    @ and (s1, S2, D, 4);
    @ or tr=2.1n tf=1.8n (1, 2, 3, 4, Y);
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

network osc10 (terminal E, F, XI, XO, vss, vdd)
{
    @ nand tr=100p tf=100p (E, XI, XO);
    @ nand tr=300p tf=400p (E, XO, F);
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

network De211 (terminal A, B, Y0, Y1, Y2, Y3, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, vss, 10);
    penh w=29.6u l=1.6u (A, 10, vdd);
    penh w=29.6u l=1.6u (B, 2, vdd);
    nenh w=23.2u l=1.6u (B, 2, vss);
    nenh w=23.2u l=1.6u (A, Y2, vss);
    penh w=29.6u l=1.6u (A, 3, vdd);
    penh w=29.6u l=1.6u (B, 4, vdd);
    nenh w=23.2u l=1.6u (B, Y0, vss);
    nenh w=23.2u l=1.6u (2, Y2, vss);
    penh w=29.6u l=1.6u (2, Y2, 3);
    penh w=29.6u l=1.6u (A, 4, Y0);
    nenh w=23.2u l=1.6u (A, Y0, vss);
    penh w=29.6u l=1.6u (10, 5, Y1);
    nenh w=23.2u l=1.6u (10, Y1, vss);
    nenh w=23.2u l=1.6u (10, Y3, vss);
    penh w=29.6u l=1.6u (10, 1, vdd);
    penh w=29.6u l=1.6u (B, 5, vdd);
    nenh w=23.2u l=1.6u (B, Y1, vss);
    nenh w=23.2u l=1.6u (2, Y3, vss);
    penh w=29.6u l=1.6u (2, 1, Y3);
    cap 204f (2, vss);
    cap 92f (3, vss);
    cap 93f (4, vss);
    cap 93f (5, vss);
    cap 130f (Y3, vss);
    cap 130f (Y1, vss);
    cap 132f (Y0, vss);
    cap 220f (10, vss);
    cap 133f (Y2, vss);
    cap 95f (1, vss);
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
    cap 160f (41, vss);
    cap 320f (QN, vss);
    cap 240f (Q, vss);
    cap 160f (42, vss);
    cap 140f (43, vss);
    cap 160f (44, vss);
    cap 140f (45, vss);
    cap 200f (46, vss);
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
    cap 160f (55, vss);
    cap 320f (QN, vss);
    cap 240f (Q, vss);
    cap 160f (56, vss);
    cap 220f (57, vss);
    cap 160f (58, vss);
    cap 140f (59, vss);
    cap 200f (60, vss);
    cap 140f (9, vss);
    cap 50f (14, vss);
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
    cap 140f (AN, vss);
    cap 100f (28, vss);
    cap 100f (29, vss);
    cap 200f (Y, vss);
    cap 190f (BN, vss);
    cap 40f (9, vss);
    cap 40f (18, vss);
}

network Iv110 (terminal A, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, Y, vss);
    penh w=29.6u l=1.6u (A, Y, vdd);
    cap 290f (Y, vss);
}

network Ln3x3 (terminal g, d, s, vss, vdd)
{
    nenh w=23.2u l=1.6u (60, 7, 41);
    cap 31.59254f (7, vss);
    nenh w=23.2u l=1.6u (61, 7, 14);
    cap 31.59254f (14, vss);
    nenh w=23.2u l=1.6u (59, 14, d);
    nenh w=23.2u l=1.6u (62, 27, d);
    cap 33.03638f (27, vss);
    nenh w=23.2u l=1.6u (58, 27, 34);
    cap 33.03638f (34, vss);
    nenh w=23.2u l=1.6u (63, 34, s);
    res 183.0893 (41, s);
    cap 50.78461f (41, vss);
    cap 41.23057f (s, vss);
    nenh w=23.2u l=1.6u (57, s, 48);
    cap 31.59254f (48, vss);
    nenh w=23.2u l=1.6u (64, 48, 55);
    cap 31.59254f (55, vss);
    nenh w=23.2u l=1.6u (56, 55, d);
    res 140.6821 (g, 56);
    res 143.2366 (g, 57);
    res 149.927 (g, 58);
    res 149.9249 (g, 59);
    res 150.6297 (g, 60);
    res 149.9249 (g, 61);
    res 149.927 (g, 62);
    res 149.927 (g, 63);
    res 143.2366 (g, 64);
    cap 34.26446f (g, vss);
    cap 4.539861f (56, vss);
    cap 4.544917f (57, vss);
    cap 4.848077f (58, vss);
    cap 4.35723f (59, vss);
    cap 4.352508f (60, vss);
    cap 4.35723f (61, vss);
    cap 4.848077f (62, vss);
    cap 4.848077f (63, vss);
    cap 4.544917f (64, vss);
    cap 81.16528f (d, vss);
    res 497.0514 (vss, 77);
    cap 41.12208f (vss, vss);
    cap 2.950429f (77, vss);
    cap 36.83792f (vdd, vss);
}

network Lp3x3 (terminal g, d, s, vss, vdd)
{
    penh w=29.6u l=1.6u (62, 9, 43);
    cap 90.25434f (9, vss);
    penh w=29.6u l=1.6u (63, 9, 16);
    cap 90.25434f (16, vss);
    penh w=29.6u l=1.6u (61, 16, s);
    penh w=29.6u l=1.6u (64, 29, s);
    cap 91.69818f (29, vss);
    penh w=29.6u l=1.6u (60, 29, 36);
    cap 91.69818f (36, vss);
    penh w=29.6u l=1.6u (65, 36, d);
    res 192.1254 (d, 43);
    cap 101.0897f (d, vss);
    cap 127.5971f (43, vss);
    penh w=29.6u l=1.6u (59, d, 50);
    cap 90.25434f (50, vss);
    penh w=29.6u l=1.6u (66, 50, 57);
    cap 90.25434f (57, vss);
    penh w=29.6u l=1.6u (58, 57, s);
    res 597.7727 (g, 58);
    res 598.3152 (g, 59);
    res 598.3883 (g, 60);
    res 598.1335 (g, 61);
    res 598.6038 (g, 62);
    res 598.1335 (g, 63);
    res 598.3883 (g, 64);
    res 598.3883 (g, 65);
    res 598.3152 (g, 66);
    cap 47.41822f (g, vss);
    cap 3.111008f (58, vss);
    cap 3.114514f (59, vss);
    cap 3.259562f (60, vss);
    cap 3.028629f (61, vss);
    cap 3.028149f (62, vss);
    cap 3.028629f (63, vss);
    cap 3.259562f (64, vss);
    cap 3.259562f (65, vss);
    cap 3.114514f (66, vss);
    cap 201.0074f (s, vss);
    cap 36.66864f (vss, vss);
    res 137.084 (vdd, 81);
    cap 40.13621f (vdd, vss);
    cap 4.092584f (81, vss);
}

network Mir_nin (terminal in, g, vss, vdd)
{
    nenh w=23.2u l=1.6u (61, 7, in);
    cap 30.1487f (7, vss);
    nenh w=23.2u l=1.6u (63, 7, 14);
    cap 30.1487f (14, vss);
    nenh w=23.2u l=1.6u (60, 14, g);
    nenh w=23.2u l=1.6u (58, 27, g);
    cap 31.59254f (27, vss);
    nenh w=23.2u l=1.6u (59, 27, 34);
    cap 31.59254f (34, vss);
    nenh w=23.2u l=1.6u (62, 34, in);
    nenh w=23.2u l=1.6u (56, 47, in);
    cap 31.59254f (47, vss);
    nenh w=23.2u l=1.6u (57, 47, 54);
    cap 31.59254f (54, vss);
    nenh w=23.2u l=1.6u (55, 54, g);
    res 140.6821 (in, 55);
    res 140.4388 (in, 56);
    res 143.2366 (in, 59);
    res 141.0979 (in, 60);
    res 139.2399 (in, 61);
    res 141.0979 (in, 63);
    res 143.2366 (in, 58);
    res 143.2366 (in, 62);
    res 143.2366 (in, 57);
    cap 115.0666f (in, vss);
    cap 4.539861f (55, vss);
    cap 4.578032f (56, vss);
    cap 4.544917f (57, vss);
    cap 4.544917f (58, vss);
    cap 4.544917f (59, vss);
    cap 4.089673f (60, vss);
    cap 4.120188f (61, vss);
    cap 4.544917f (62, vss);
    cap 4.089673f (63, vss);
    nenh w=23.2u l=1.6u (131, 76, g);
    cap 31.59254f (76, vss);
    nenh w=23.2u l=1.6u (130, 76, 83);
    cap 33.76926f (83, vss);
    nenh w=23.2u l=1.6u (129, 83, vss);
    nenh w=23.2u l=1.6u (127, 96, vss);
    cap 31.59254f (96, vss);
    nenh w=23.2u l=1.6u (128, 96, 103);
    cap 31.59254f (103, vss);
    nenh w=23.2u l=1.6u (126, 103, g);
    nenh w=23.2u l=1.6u (125, 116, g);
    cap 30.1487f (116, vss);
    nenh w=23.2u l=1.6u (132, 116, 123);
    cap 30.1487f (123, vss);
    nenh w=23.2u l=1.6u (124, 123, vss);
    res 138.5422 (g, 124);
    res 141.0979 (g, 125);
    res 143.2366 (g, 128);
    res 140.4388 (g, 129);
    res 143.2366 (g, 130);
    res 143.1658 (g, 131);
    res 143.2366 (g, 127);
    res 143.2366 (g, 126);
    res 141.0979 (g, 132);
    cap 161.5651f (g, vss);
    cap 4.0858f (124, vss);
    cap 4.089673f (125, vss);
    cap 4.544917f (126, vss);
    cap 5.592422f (127, vss);
    cap 4.544917f (128, vss);
    cap 4.578032f (129, vss);
    cap 4.544917f (130, vss);
    cap 4.535727f (131, vss);
    cap 4.089673f (132, vss);
    res 495.7327 (vss, 145);
    cap 143.8494f (vss, vss);
    cap 2.956572f (145, vss);
    cap 67.30832f (vdd, vss);
}

network Mir_nout (terminal in, g, out, vss, vdd)
{
    nenh w=23.2u l=1.6u (60, 7, 41);
    cap 31.59254f (7, vss);
    nenh w=23.2u l=1.6u (61, 7, 14);
    cap 31.59254f (14, vss);
    nenh w=23.2u l=1.6u (59, 14, 111);
    nenh w=23.2u l=1.6u (62, 27, 111);
    cap 33.03638f (27, vss);
    nenh w=23.2u l=1.6u (58, 27, 34);
    cap 33.03638f (34, vss);
    nenh w=23.2u l=1.6u (63, 34, out);
    res 181.3259 (out, 41);
    cap 41.14055f (out, vss);
    cap 50.73513f (41, vss);
    nenh w=23.2u l=1.6u (57, out, 48);
    cap 31.59254f (48, vss);
    nenh w=23.2u l=1.6u (64, 48, 55);
    cap 31.59254f (55, vss);
    nenh w=23.2u l=1.6u (56, 55, 111);
    res 147.3737 (in, 56);
    res 149.9249 (in, 57);
    res 149.9251 (in, 58);
    res 141.2747 (in, 59);
    res 141.9792 (in, 60);
    res 141.2747 (in, 61);
    res 149.9251 (in, 62);
    res 149.9251 (in, 63);
    res 149.9249 (in, 64);
    cap 34.09483f (in, vss);
    cap 4.349105f (56, vss);
    cap 4.35723f (57, vss);
    cap 4.84813f (58, vss);
    cap 4.60293f (59, vss);
    cap 4.596703f (60, vss);
    cap 4.60293f (61, vss);
    cap 4.84813f (62, vss);
    cap 4.84813f (63, vss);
    cap 4.35723f (64, vss);
    nenh w=23.2u l=1.6u (130, 77, 111);
    cap 31.59254f (77, vss);
    nenh w=23.2u l=1.6u (131, 77, 84);
    cap 31.59254f (84, vss);
    nenh w=23.2u l=1.6u (129, 84, vss);
    nenh w=23.2u l=1.6u (132, 97, vss);
    cap 31.59254f (97, vss);
    nenh w=23.2u l=1.6u (128, 97, 104);
    cap 31.59254f (104, vss);
    nenh w=23.2u l=1.6u (133, 104, 111);
    cap 126.365f (111, vss);
    nenh w=23.2u l=1.6u (127, 111, 118);
    cap 30.1487f (118, vss);
    nenh w=23.2u l=1.6u (134, 118, 125);
    cap 30.1487f (125, vss);
    nenh w=23.2u l=1.6u (126, 125, vss);
    res 138.5422 (g, 126);
    res 141.0979 (g, 127);
    res 149.9249 (g, 128);
    res 149.9249 (g, 129);
    res 150.6297 (g, 130);
    res 149.9249 (g, 131);
    res 149.9249 (g, 132);
    res 149.9249 (g, 133);
    res 141.0979 (g, 134);
    cap 34.031f (g, vss);
    cap 4.0858f (126, vss);
    cap 4.089673f (127, vss);
    cap 4.35723f (128, vss);
    cap 4.35723f (129, vss);
    cap 4.352508f (130, vss);
    cap 4.35723f (131, vss);
    cap 5.362361f (132, vss);
    cap 4.35723f (133, vss);
    cap 4.089673f (134, vss);
    res 495.7327 (vss, 147);
    cap 143.8494f (vss, vss);
    cap 2.956572f (147, vss);
    cap 67.30832f (vdd, vss);
}

network Mir_pin (terminal in, g, vss, vdd)
{
    penh w=29.6u l=1.6u (65, 9, in);
    cap 88.8105f (9, vss);
    penh w=29.6u l=1.6u (63, 9, 16);
    cap 88.8105f (16, vss);
    penh w=29.6u l=1.6u (60, 16, g);
    penh w=29.6u l=1.6u (62, 29, g);
    cap 90.25434f (29, vss);
    penh w=29.6u l=1.6u (64, 29, 36);
    cap 90.25434f (36, vss);
    penh w=29.6u l=1.6u (61, 36, in);
    penh w=29.6u l=1.6u (58, 49, in);
    cap 90.25434f (49, vss);
    penh w=29.6u l=1.6u (59, 49, 56);
    cap 90.25434f (56, vss);
    penh w=29.6u l=1.6u (57, 56, g);
    res 597.7727 (in, 57);
    res 599.2217 (in, 58);
    res 598.3152 (in, 61);
    res 598.3152 (in, 62);
    res 597.9395 (in, 63);
    res 599.5777 (in, 65);
    res 597.9395 (in, 60);
    res 598.3152 (in, 64);
    res 598.3152 (in, 59);
    cap 264.099f (in, vss);
    cap 3.111008f (57, vss);
    cap 3.123546f (58, vss);
    cap 3.114514f (59, vss);
    cap 2.883997f (60, vss);
    cap 3.114514f (61, vss);
    cap 3.114514f (62, vss);
    cap 2.883997f (63, vss);
    cap 3.114514f (64, vss);
    cap 2.892854f (65, vss);
    penh w=29.6u l=1.6u (129, 78, g);
    cap 90.25434f (78, vss);
    penh w=29.6u l=1.6u (133, 78, 85);
    cap 90.25434f (85, vss);
    penh w=29.6u l=1.6u (132, 85, vdd);
    penh w=29.6u l=1.6u (128, 98, vdd);
    cap 90.25434f (98, vss);
    penh w=29.6u l=1.6u (130, 98, 105);
    cap 90.25434f (105, vss);
    penh w=29.6u l=1.6u (131, 105, g);
    penh w=29.6u l=1.6u (127, 118, g);
    cap 88.8105f (118, vss);
    penh w=29.6u l=1.6u (134, 118, 125);
    cap 88.8105f (125, vss);
    penh w=29.6u l=1.6u (126, 125, vdd);
    res 597.3969 (g, 126);
    res 597.9395 (g, 127);
    res 598.3152 (g, 130);
    res 598.3152 (g, 133);
    res 598.7855 (g, 129);
    res 598.3152 (g, 132);
    res 599.2217 (g, 128);
    res 598.3152 (g, 131);
    res 597.9395 (g, 134);
    cap 354.5025f (g, vss);
    cap 2.880755f (126, vss);
    cap 2.883997f (127, vss);
    cap 4.191287f (128, vss);
    cap 3.113967f (129, vss);
    cap 3.114514f (130, vss);
    cap 3.114514f (131, vss);
    cap 3.114514f (132, vss);
    cap 3.114514f (133, vss);
    cap 2.883997f (134, vss);
    cap 67.13904f (vss, vss);
    res 137.0994 (vdd, 149);
    cap 265.4289f (vdd, vss);
    cap 4.124227f (149, vss);
}

network Mir_pout (terminal in, g, out, vss, vdd)
{
    penh w=29.6u l=1.6u (62, 9, 43);
    cap 90.25434f (9, vss);
    penh w=29.6u l=1.6u (63, 9, 16);
    cap 90.25434f (16, vss);
    penh w=29.6u l=1.6u (61, 16, 113);
    penh w=29.6u l=1.6u (64, 29, 113);
    cap 91.69818f (29, vss);
    penh w=29.6u l=1.6u (60, 29, 36);
    cap 91.69818f (36, vss);
    penh w=29.6u l=1.6u (65, 36, out);
    res 189.3639 (43, out);
    cap 127.3772f (43, vss);
    cap 101.5662f (out, vss);
    penh w=29.6u l=1.6u (59, out, 50);
    cap 90.25434f (50, vss);
    penh w=29.6u l=1.6u (66, 50, 57);
    cap 90.25434f (57, vss);
    penh w=29.6u l=1.6u (58, 57, 113);
    res 597.591 (in, 58);
    res 598.1335 (in, 59);
    res 605.5328 (in, 60);
    res 605.4285 (in, 61);
    res 605.8988 (in, 62);
    res 605.4285 (in, 63);
    res 605.5328 (in, 64);
    res 605.5328 (in, 65);
    res 598.1335 (in, 66);
    cap 46.10376f (in, vss);
    cap 3.025044f (58, vss);
    cap 3.028629f (59, vss);
    cap 3.47783f (60, vss);
    cap 3.334504f (61, vss);
    cap 3.333793f (62, vss);
    cap 3.334504f (63, vss);
    cap 3.47783f (64, vss);
    cap 3.47783f (65, vss);
    cap 3.028629f (66, vss);
    penh w=29.6u l=1.6u (132, 79, 113);
    cap 90.25434f (79, vss);
    penh w=29.6u l=1.6u (133, 79, 86);
    cap 90.25434f (86, vss);
    penh w=29.6u l=1.6u (131, 86, vdd);
    penh w=29.6u l=1.6u (134, 99, vdd);
    cap 90.25434f (99, vss);
    penh w=29.6u l=1.6u (130, 99, 106);
    cap 90.25434f (106, vss);
    penh w=29.6u l=1.6u (135, 106, 113);
    cap 306.0199f (113, vss);
    penh w=29.6u l=1.6u (129, 113, 120);
    cap 88.8105f (120, vss);
    penh w=29.6u l=1.6u (136, 120, 127);
    cap 88.8105f (127, vss);
    penh w=29.6u l=1.6u (128, 127, vdd);
    res 597.3969 (g, 128);
    res 597.9395 (g, 129);
    res 598.1335 (g, 130);
    res 598.1335 (g, 131);
    res 598.6038 (g, 132);
    res 598.1335 (g, 133);
    res 598.1335 (g, 134);
    res 598.1335 (g, 135);
    res 597.9395 (g, 136);
    cap 45.66866f (g, vss);
    cap 2.880755f (128, vss);
    cap 2.883997f (129, vss);
    cap 3.028629f (130, vss);
    cap 3.028629f (131, vss);
    cap 3.028149f (132, vss);
    cap 3.028629f (133, vss);
    cap 4.096869f (134, vss);
    cap 3.028629f (135, vss);
    cap 2.883997f (136, vss);
    cap 67.13904f (vss, vss);
    res 137.0994 (vdd, 151);
    cap 262.6801f (vdd, vss);
    cap 4.124227f (151, vss);
}

network Mu111 (terminal A, B, S, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (S, SN, vss);
    penh w=29.6u l=1.6u (S, SN, vdd);
    nenh w=23.2u l=1.6u (A, AN, vss);
    penh w=29.6u l=1.6u (A, AN, vdd);
    nenh w=23.2u l=1.6u (SN, AN, 32);
    penh w=29.6u l=1.6u (S, AN, 32);
    nenh w=23.2u l=1.6u (S, BN, 32);
    penh w=29.6u l=1.6u (SN, BN, 32);
    nenh w=23.2u l=1.6u (B, BN, vss);
    penh w=29.6u l=1.6u (B, BN, vdd);
    nenh w=23.2u l=1.6u (32, Y, vss);
    penh w=29.6u l=1.6u (32, Y, vdd);
    cap 400f (SN, vss);
    cap 100f (BN, vss);
    cap 250f (Y, vss);
    cap 250f (32, vss);
    cap 100f (AN, vss);
}

network Mu210 (terminal S1, S2, A, B, C, D, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (S1, 5, vss);
    penh w=29.6u l=1.6u (S1, 5, vdd);
    penh w=29.6u l=1.6u (A, 3, vdd);
    nenh w=23.2u l=1.6u (A, 3, vss);
    nenh w=23.2u l=1.6u (D, 1, vss);
    penh w=29.6u l=1.6u (D, 1, vdd);
    penh w=29.6u l=1.6u (S1, 3, 8);
    nenh w=23.2u l=1.6u (5, 3, 8);
    nenh w=23.2u l=1.6u (S1, 1, 7);
    penh w=29.6u l=1.6u (5, 1, 7);
    penh w=29.6u l=1.6u (B, 4, vdd);
    nenh w=23.2u l=1.6u (B, 4, vss);
    nenh w=23.2u l=1.6u (C, 2, vss);
    penh w=29.6u l=1.6u (C, 2, vdd);
    penh w=29.6u l=1.6u (5, 4, 8);
    nenh w=23.2u l=1.6u (S1, 4, 8);
    nenh w=23.2u l=1.6u (5, 2, 7);
    penh w=29.6u l=1.6u (S1, 2, 7);
    penh w=29.6u l=1.6u (S2, 8, 15);
    nenh w=23.2u l=1.6u (6, 8, 15);
    nenh w=23.2u l=1.6u (S2, 7, 15);
    penh w=29.6u l=1.6u (6, 7, 15);
    penh w=29.6u l=1.6u (S2, 6, vdd);
    nenh w=23.2u l=1.6u (S2, 6, vss);
    nenh w=23.2u l=1.6u (15, Y, vss);
    penh w=29.6u l=1.6u (15, Y, vdd);
    cap 150f (1, vss);
    cap 150f (2, vss);
    cap 150f (3, vss);
    cap 150f (4, vss);
    cap 600f (5, vss);
    cap 280f (7, vss);
    cap 280f (8, vss);
    cap 450f (Y, vss);
    cap 600f (15, vss);
}

network Na210 (terminal A, B, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 5, vss);
    nenh w=23.2u l=1.6u (B, 5, Y);
    penh w=29.6u l=1.6u (A, Y, vdd);
    penh w=29.6u l=1.6u (B, Y, vdd);
    cap 185f (Y, vss);
    cap 30f (5, vss);
}

network Na310 (terminal A, B, C, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 5, vss);
    penh w=29.6u l=1.6u (A, Y, vdd);
    nenh w=23.2u l=1.6u (B, 5, 10);
    penh w=29.6u l=1.6u (B, Y, vdd);
    nenh w=23.2u l=1.6u (C, 10, Y);
    penh w=29.6u l=1.6u (C, Y, vdd);
    cap 320f (Y, vss);
    cap 35f (5, vss);
    cap 35f (10, vss);
}

network No210 (terminal A, B, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, Y, vss);
    nenh w=23.2u l=1.6u (B, Y, vss);
    penh w=29.6u l=1.6u (A, 5, vdd);
    penh w=29.6u l=1.6u (B, 5, Y);
    cap 90f (5, vss);
    cap 240f (Y, vss);
}

network No310 (terminal A, B, C, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, Y, vss);
    penh w=29.6u l=1.6u (A, 5, vdd);
    nenh w=23.2u l=1.6u (B, Y, vss);
    penh w=29.6u l=1.6u (B, 5, 10);
    nenh w=23.2u l=1.6u (C, Y, vss);
    penh w=29.6u l=1.6u (C, 10, Y);
    cap 100f (5, vss);
    cap 100f (10, vss);
    cap 285f (Y, vss);
}

network Osc10 (terminal E, F, XI, XO, vss, vdd)
{
    nenh w=0.00017 l=1.6u (E, 6, XO);
    penh w=0.0002 l=1.6u (XI, XO, vdd);
    nenh w=0.00017 l=1.6u (XI, 6, vss);
    penh w=0.0002 l=1.6u (E, XO, vdd);
    nenh w=23.2u l=1.6u (E, 7, vss);
    penh w=29.6u l=1.6u (E, F, vdd);
    nenh w=23.2u l=1.6u (XO, 7, F);
    penh w=29.6u l=1.6u (XO, F, vdd);
    cap 40f (7, vss);
    cap 150f (F, vss);
}
