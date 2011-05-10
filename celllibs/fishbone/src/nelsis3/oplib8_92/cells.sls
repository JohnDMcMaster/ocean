

network iv110 (terminal A, Y)
{
    @ invert (A, Y);
}

network na210 (terminal A, B, Y)
{
    @ nand (A, B, Y);
}

network na310 (terminal A, B, C, Y)
{
    @ nand (A, B, C, Y);
}

network no210 (terminal A, B, Y)
{
    @ nor (A, B, Y);
}

network no310 (terminal A, B, C, Y)
{
    @ nor (A, B, C, Y);
}

network dfn20 (terminal D, CK, Q, QN)
{
    @ invert (D, 1);
    @ and (1, 8, 2);
    @ nor (2, 6, 3);
    @ and (3, CK, 4);
    @ nor (4, Q, QN);
    @ and (D, 8, 5);
    @ nor (3, 5, 6);
    @ and (6, CK, 7);
    @ nor (QN, 7, Q);
    @ invert (CK, 8);
}

network lab10 (terminal SN, RN, Q, QN)
{
    @ nand (SN, QN, Q);
    @ nand (RN, Q, QN);
}

extern network lab10 (terminal SN, RN, Q, QN)

network lah10 (terminal D, LD, Q, QN)
{
    @ nand (D, LD, Y1);
    @ invert (D, DN);
    @ nand (DN, LD, Y2);
    lab10 (Y1, Y2, Q, QN);
}

network Dfn20 (terminal D, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (CK, 1, vss);
    penh w=29.6u l=1.6u (CK, 1, vdd);
    penh w=29.6u l=1.6u (D, 4, vdd);
    nenh w=23.2u l=1.6u (D, 4, vss);
    nenh w=23.2u l=1.6u (1, 2, vss);
    penh w=29.6u l=1.6u (1, 5, vdd);
    penh w=29.6u l=1.6u (1, 6, vdd);
    nenh w=23.2u l=1.6u (1, 3, vss);
    nenh w=23.2u l=1.6u (D, 2, 5);
    penh w=29.6u l=1.6u (D, 5, vdd);
    penh w=29.6u l=1.6u (4, 6, vdd);
    nenh w=23.2u l=1.6u (4, 3, 6);
    nenh w=23.2u l=1.6u (5, 7, vss);
    penh w=29.6u l=1.6u (5, 10, vdd);
    penh w=29.6u l=1.6u (6, 12, vdd);
    nenh w=23.2u l=1.6u (6, 8, vss);
    nenh w=23.2u l=1.6u (12, 7, 10);
    penh w=29.6u l=1.6u (12, 10, vdd);
    penh w=29.6u l=1.6u (10, 12, vdd);
    nenh w=23.2u l=1.6u (10, 8, 12);
    nenh w=23.2u l=1.6u (CK, 9, vss);
    penh w=29.6u l=1.6u (CK, 13, vdd);
    penh w=29.6u l=1.6u (CK, 14, vdd);
    nenh w=23.2u l=1.6u (CK, 11, vss);
    nenh w=23.2u l=1.6u (10, 9, 13);
    penh w=29.6u l=1.6u (10, 13, vdd);
    penh w=29.6u l=1.6u (12, 14, vdd);
    nenh w=23.2u l=1.6u (12, 11, 14);
    nenh w=23.2u l=1.6u (13, 15, vss);
    penh w=29.6u l=1.6u (13, Q, vdd);
    penh w=29.6u l=1.6u (14, QN, vdd);
    nenh w=23.2u l=1.6u (14, 16, vss);
    nenh w=23.2u l=1.6u (QN, 15, Q);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    penh w=29.6u l=1.6u (Q, QN, vdd);
    nenh w=23.2u l=1.6u (Q, 16, QN);
}

network Iv110 (terminal A, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, Y, vss);
    penh w=29.6u l=1.6u (A, Y, vdd);
}

network Na210 (terminal A, B, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 5, vss);
    penh w=29.6u l=1.6u (A, Y, vdd);
    nenh w=23.2u l=1.6u (B, 5, Y);
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
    penh w=29.6u l=1.6u (A, 5, vdd);
    nenh w=23.2u l=1.6u (B, Y, vss);
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

network dfr10 (terminal D, R, CK, Q, QN)
{
    @ invert (D, dn);
    @ nor (dn, R, dd);
    @ invert (dd, 1);
    @ and (1, 8, 2);
    @ nor (2, 6, 3);
    @ and (3, CK, 4);
    @ nor (4, Q, QN);
    @ and (dd, 8, 5);
    @ nor (3, 5, 6);
    @ and (6, CK, 7);
    @ nor (QN, 7, Q);
    @ invert (CK, 8);
}

network Dfr10 (terminal D, R, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (CK, 7, vss);
    penh w=29.6u l=1.6u (CK, 7, vdd);
    penh w=29.6u l=1.6u (2, 3, vdd);
    nenh w=23.2u l=1.6u (2, 1, vss);
    penh w=29.6u l=1.6u (D, 3, vdd);
    nenh w=23.2u l=1.6u (D, 1, 3);
    nenh w=23.2u l=1.6u (R, 2, vss);
    penh w=29.6u l=1.6u (R, 2, vdd);
    penh w=29.6u l=1.6u (2, 8, vdd);
    nenh w=23.2u l=1.6u (2, 4, vss);
    nenh w=23.2u l=1.6u (3, 5, vss);
    penh w=29.6u l=1.6u (3, 10, vdd);
    penh w=29.6u l=1.6u (D, 8, vdd);
    nenh w=23.2u l=1.6u (D, 4, 6);
    nenh w=23.2u l=1.6u (7, 5, 10);
    penh w=29.6u l=1.6u (7, 10, vdd);
    penh w=29.6u l=1.6u (7, 8, vdd);
    nenh w=23.2u l=1.6u (7, 6, 8);
    nenh w=23.2u l=1.6u (13, 9, vss);
    penh w=29.6u l=1.6u (13, 12, vdd);
    penh w=29.6u l=1.6u (8, 13, vdd);
    nenh w=23.2u l=1.6u (8, 11, vss);
    nenh w=23.2u l=1.6u (10, 9, 12);
    penh w=29.6u l=1.6u (10, 12, vdd);
    penh w=29.6u l=1.6u (12, 13, vdd);
    nenh w=23.2u l=1.6u (12, 11, 13);
    nenh w=23.2u l=1.6u (12, 14, vss);
    penh w=29.6u l=1.6u (12, 18, vdd);
    penh w=29.6u l=1.6u (13, 16, vdd);
    nenh w=23.2u l=1.6u (13, 15, vss);
    nenh w=23.2u l=1.6u (CK, 14, 18);
    penh w=29.6u l=1.6u (CK, 18, vdd);
    penh w=29.6u l=1.6u (CK, 16, vdd);
    nenh w=23.2u l=1.6u (CK, 15, 16);
    nenh w=23.2u l=1.6u (Q, 17, vss);
    penh w=29.6u l=1.6u (Q, QN, vdd);
    penh w=29.6u l=1.6u (16, Q, vdd);
    nenh w=23.2u l=1.6u (16, 19, vss);
    nenh w=23.2u l=1.6u (18, 17, QN);
    penh w=29.6u l=1.6u (18, QN, vdd);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    nenh w=23.2u l=1.6u (QN, 19, Q);
}

network Lab10 (terminal SN, RN, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (Q, 1, vss);
    penh w=29.6u l=1.6u (Q, QN, vdd);
    penh w=29.6u l=1.6u (SN, Q, vdd);
    nenh w=23.2u l=1.6u (SN, 2, vss);
    nenh w=23.2u l=1.6u (RN, 1, QN);
    penh w=29.6u l=1.6u (RN, QN, vdd);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    nenh w=23.2u l=1.6u (QN, 2, Q);
}

network Lah10 (terminal D, LD, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (D, 5, vss);
    penh w=29.6u l=1.6u (D, 5, vdd);
    nenh w=23.2u l=1.6u (5, 6, vss);
    penh w=29.6u l=1.6u (5, 10, vdd);
    penh w=29.6u l=1.6u (D, 8, vdd);
    nenh w=23.2u l=1.6u (D, 7, vss);
    nenh w=23.2u l=1.6u (LD, 6, 10);
    penh w=29.6u l=1.6u (LD, 10, vdd);
    penh w=29.6u l=1.6u (LD, 8, vdd);
    nenh w=23.2u l=1.6u (LD, 7, 8);
    nenh w=23.2u l=1.6u (Q, 9, vss);
    penh w=29.6u l=1.6u (Q, QN, vdd);
    penh w=29.6u l=1.6u (8, Q, vdd);
    nenh w=23.2u l=1.6u (8, 11, vss);
    nenh w=23.2u l=1.6u (10, 9, QN);
    penh w=29.6u l=1.6u (10, QN, vdd);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    nenh w=23.2u l=1.6u (QN, 11, Q);
}

network tg110 (terminal IN, SEL, SELN, OUT)
{
    nenh w=23.2u l=1.6u (SEL, IN, OUT);
    penh w=29.6u l=1.6u (SELN, IN, OUT);
}

network De210 (terminal A, B, Y0, Y1, Y2, Y3, vdd, vss)
{
    nenh w=23.2u l=1.6u (B, 3, vss);
    penh w=29.6u l=1.6u (B, 3, vdd);
    penh w=29.6u l=1.6u (A, 4, vdd);
    nenh w=23.2u l=1.6u (A, 4, vss);
    nenh w=23.2u l=1.6u (vss, 3, vss);
    penh w=29.6u l=1.6u (vdd, 3, vdd);
    penh w=29.6u l=1.6u (vdd, 4, vdd);
    nenh w=23.2u l=1.6u (vss, 4, vss);
    nenh w=23.2u l=1.6u (A, 1, vss);
    penh w=29.6u l=1.6u (A, Y1, vdd);
    penh w=29.6u l=1.6u (4, Y0, vdd);
    nenh w=23.2u l=1.6u (4, 2, vss);
    nenh w=23.2u l=1.6u (3, 1, Y1);
    penh w=29.6u l=1.6u (3, Y1, vdd);
    penh w=29.6u l=1.6u (3, Y0, vdd);
    nenh w=23.2u l=1.6u (3, 2, Y0);
    nenh w=23.2u l=1.6u (vss, Y1, vss);
    penh w=29.6u l=1.6u (vdd, vdd, vdd);
    penh w=29.6u l=1.6u (vdd, vdd, vdd);
    nenh w=23.2u l=1.6u (vss, Y0, vss);
    nenh w=23.2u l=1.6u (A, 5, vss);
    penh w=29.6u l=1.6u (A, Y3, vdd);
    penh w=29.6u l=1.6u (4, Y2, vdd);
    nenh w=23.2u l=1.6u (4, 6, vss);
    nenh w=23.2u l=1.6u (B, 5, Y3);
    penh w=29.6u l=1.6u (B, Y3, vdd);
    penh w=29.6u l=1.6u (B, Y2, vdd);
    nenh w=23.2u l=1.6u (B, 6, Y2);
    penh w=29.6u l=1.6u (vdd, vdd, vdd);
    penh w=29.6u l=1.6u (vdd, vdd, vdd);
}

network de210 (terminal A, B, Y0, Y1, Y2, Y3)
{
    @ invert (A, AN);
    @ invert (B, BN);
    @ nand (AN, BN, Y0);
    @ nand (A, BN, Y1);
    @ nand (AN, B, Y2);
    @ nand (A, B, Y3);
}

network laa10 (terminal S, R, Q, QN)
{
    @ nor (S, Q, QN);
    @ nor (R, QN, Q);
}

network fishbonec3tu2 ()
{
}

extern network tg111 (terminal A, SEL, SELN, Y)

network dfr11 (terminal D, R, CK, Q, QN)
{
    @ invert (R, RN);
    @ invert (CK, CKN);
    @ and (D, RN, Z1);
    tg111 (Z1, CKN, CK, Z2);
    @ invert (Z2, Z3);
    @ invert (Z3, Z7);
    tg111 (Z7, CK, CKN, Z2);
    tg111 (Z3, CK, CKN, Z4);
    @ invert (Z4, Z5);
    @ invert (Z5, Z6);
    tg111 (Z6, CKN, CK, Z4);
    @ invert (Z5, QN);
    @ invert (Z6, Q);
}

network Dfr11 (terminal D, R, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (R, 9, vss);
    penh w=29.6u l=1.6u (R, 9, vdd);
    nenh w=23.2u l=1.6u (9, 14, 19);
    penh w=29.6u l=1.6u (9, 19, vdd);
    nenh w=23.2u l=1.6u (D, 14, vss);
    penh w=29.6u l=1.6u (D, 19, vdd);
    nenh w=23.2u l=1.6u (19, 24, vss);
    penh w=29.6u l=1.6u (19, 24, vdd);
    nenh w=23.2u l=1.6u (65, 24, 38);
    penh w=29.6u l=1.6u (CK, 24, 38);
    nenh w=23.2u l=1.6u (CK, 33, 38);
    penh w=29.6u l=1.6u (65, 33, 38);
    nenh w=23.2u l=1.6u (43, 33, vss);
    penh w=29.6u l=1.6u (43, 33, vdd);
    nenh w=23.2u l=1.6u (38, 43, vss);
    penh w=29.6u l=1.6u (38, 43, vdd);
    nenh w=23.2u l=1.6u (CK, 43, 83);
    penh w=29.6u l=1.6u (65, 43, 83);
    nenh w=23.2u l=1.6u (65, 52, 83);
    penh w=29.6u l=1.6u (CK, 52, 83);
    nenh w=23.2u l=1.6u (74, 52, vss);
    penh w=29.6u l=1.6u (74, 52, vdd);
    nenh w=23.2u l=1.6u (83, 74, vss);
    penh w=29.6u l=1.6u (83, 74, vdd);
    nenh w=23.2u l=1.6u (CK, 65, vss);
    penh w=29.6u l=1.6u (CK, 65, vdd);
    nenh w=23.2u l=1.6u (74, QN, vss);
    penh w=29.6u l=1.6u (74, QN, vdd);
    nenh w=23.2u l=1.6u (74, QN, vss);
    penh w=29.6u l=1.6u (74, QN, vdd);
    nenh w=23.2u l=1.6u (83, Q, vss);
    penh w=29.6u l=1.6u (83, Q, vdd);
    nenh w=23.2u l=1.6u (83, Q, vss);
    penh w=29.6u l=1.6u (83, Q, vdd);
}

network Tg111 (terminal A, SEL, SELN, Y)
{
    nenh w=23.2u l=1.6u (SEL, A, Y);
    penh w=29.6u l=1.6u (SELN, A, Y);
}

network tg111 (terminal A, SEL, SELN, Y)
{
    nenh w=23.2u l=1.6u (SEL, A, Y);
    penh w=29.6u l=1.6u (SELN, A, Y);
}

network Laa10 (terminal S, R, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (S, QN, vss);
    penh w=29.6u l=1.6u (S, 5, vdd);
    nenh w=23.2u l=1.6u (Q, QN, vss);
    penh w=29.6u l=1.6u (Q, 5, QN);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, 18, vdd);
    nenh w=23.2u l=1.6u (R, Q, vss);
    penh w=29.6u l=1.6u (R, 18, Q);
}

network Mu110 (terminal A, B, G, S, Y, YN, vss, vdd)
{
    nenh w=23.2u l=1.6u (G, 3, vss);
    penh w=29.6u l=1.6u (G, 1, vdd);
    penh w=29.6u l=1.6u (3, 2, vdd);
    nenh w=23.2u l=1.6u (3, 2, vss);
    nenh w=23.2u l=1.6u (S, 3, vss);
    penh w=29.6u l=1.6u (S, 1, 3);
    penh w=29.6u l=1.6u (2, 6, vdd);
    nenh w=23.2u l=1.6u (2, 4, vss);
    nenh w=23.2u l=1.6u (3, 5, vss);
    penh w=29.6u l=1.6u (3, 8, vdd);
    penh w=29.6u l=1.6u (B, 6, vdd);
    nenh w=23.2u l=1.6u (B, 4, 6);
    nenh w=23.2u l=1.6u (A, 5, 8);
    penh w=29.6u l=1.6u (A, 8, vdd);
    penh w=29.6u l=1.6u (6, 14, vdd);
    nenh w=23.2u l=1.6u (6, 7, vss);
    nenh w=23.2u l=1.6u (G, 17, vss);
    penh w=29.6u l=1.6u (G, 17, vdd);
    penh w=29.6u l=1.6u (8, 14, vdd);
    nenh w=23.2u l=1.6u (8, 7, 14);
    nenh w=23.2u l=1.6u (14, 13, vss);
    penh w=29.6u l=1.6u (14, 13, vdd);
    nenh w=23.2u l=1.6u (13, 15, vss);
    penh w=29.6u l=1.6u (13, 20, vdd);
    penh w=29.6u l=1.6u (14, 18, vdd);
    nenh w=23.2u l=1.6u (14, 16, vss);
    nenh w=23.2u l=1.6u (17, 15, 20);
    penh w=29.6u l=1.6u (17, 20, vdd);
    penh w=29.6u l=1.6u (17, 18, vdd);
    nenh w=23.2u l=1.6u (17, 16, 18);
    nenh w=23.2u l=1.6u (Y, 19, vss);
    penh w=29.6u l=1.6u (Y, YN, vdd);
    penh w=29.6u l=1.6u (18, Y, vdd);
    nenh w=23.2u l=1.6u (18, 21, vss);
    nenh w=23.2u l=1.6u (20, 19, YN);
    penh w=29.6u l=1.6u (20, YN, vdd);
    penh w=29.6u l=1.6u (YN, Y, vdd);
    nenh w=23.2u l=1.6u (YN, 21, Y);
}

extern network lah10 (terminal D, LD, Q, QN)

network mu110 (terminal A, B, G, S, Y, YN)
{
    @ nor (G, S, SEL);
    @ and (A, SEL, Z1);
    @ invert (SEL, SELN);
    @ and (B, SELN, Z2);
    @ or (Z1, Z2, Z3);
    @ invert (G, GN);
    lah10 (Z3, GN, Y, YN);
}

network Mu210 (terminal S1, S2, A, B, C, D, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (S1, 4, vss);
    penh w=29.6u l=1.6u (S1, 4, vdd);
    penh w=29.6u l=1.6u (A, 1, vdd);
    nenh w=23.2u l=1.6u (A, 1, vss);
    nenh w=23.2u l=1.6u (D, 2, vss);
    penh w=29.6u l=1.6u (D, 2, vdd);
    penh w=29.6u l=1.6u (S1, 1, 6);
    nenh w=23.2u l=1.6u (4, 1, 6);
    nenh w=23.2u l=1.6u (S1, 2, 7);
    penh w=29.6u l=1.6u (4, 2, 7);
    penh w=29.6u l=1.6u (B, 3, vdd);
    nenh w=23.2u l=1.6u (B, 3, vss);
    nenh w=23.2u l=1.6u (C, 5, vss);
    penh w=29.6u l=1.6u (C, 5, vdd);
    penh w=29.6u l=1.6u (4, 3, 6);
    nenh w=23.2u l=1.6u (S1, 3, 6);
    nenh w=23.2u l=1.6u (4, 5, 7);
    penh w=29.6u l=1.6u (S1, 5, 7);
    penh w=29.6u l=1.6u (S2, 6, 8);
    nenh w=23.2u l=1.6u (9, 6, 8);
    nenh w=23.2u l=1.6u (S2, 7, 8);
    penh w=29.6u l=1.6u (9, 7, 8);
    penh w=29.6u l=1.6u (S2, 9, vdd);
    nenh w=23.2u l=1.6u (S2, 9, vss);
    nenh w=23.2u l=1.6u (8, Y, vss);
    penh w=29.6u l=1.6u (8, Y, vdd);
}

network mu210 (terminal S1, S2, A, B, C, D, Y)
{
    @ invert (A, an);
    @ invert (B, bn);
    @ invert (C, cn);
    @ invert (D, dn);
    @ invert (S1, S1n);
    @ invert (S2, S2n);
    @ invert (yn, Y);
    nenh (S1n, an, ab);
    penh (S1, an, ab);
    nenh (S1, bn, ab);
    penh (S1n, bn, ab);
    nenh (S1n, cn, cd);
    penh (S1, cn, cd);
    nenh (S1, dn, cd);
    penh (S1n, dn, cd);
    nenh (S2n, ab, yn);
    penh (S2, ab, yn);
    nenh (S2, cd, yn);
    penh (S2n, cd, yn);
}

network Rer10 (terminal D, INH, R, CK, Q, QN, vss, vdd)
{
    nenh w=23.2u l=1.6u (INH, 5, vss);
    penh w=29.6u l=1.6u (INH, 5, vdd);
    nenh w=23.2u l=1.6u (INH, 10, Q);
    penh w=29.6u l=1.6u (5, 10, Q);
    penh w=29.6u l=1.6u (INH, D, 10);
    nenh w=23.2u l=1.6u (5, D, 10);
    nenh w=23.2u l=1.6u (CK, 13, vss);
    penh w=29.6u l=1.6u (CK, 13, vdd);
    penh w=29.6u l=1.6u (7, 8, vdd);
    nenh w=23.2u l=1.6u (7, 6, vss);
    penh w=29.6u l=1.6u (10, 8, vdd);
    nenh w=23.2u l=1.6u (10, 6, 8);
    nenh w=23.2u l=1.6u (R, 7, vss);
    penh w=29.6u l=1.6u (R, 7, vdd);
    penh w=29.6u l=1.6u (7, 14, vdd);
    nenh w=23.2u l=1.6u (7, 9, vss);
    nenh w=23.2u l=1.6u (8, 11, vss);
    penh w=29.6u l=1.6u (8, 16, vdd);
    penh w=29.6u l=1.6u (10, 14, vdd);
    nenh w=23.2u l=1.6u (10, 9, 12);
    nenh w=23.2u l=1.6u (13, 11, 16);
    penh w=29.6u l=1.6u (13, 16, vdd);
    penh w=29.6u l=1.6u (13, 14, vdd);
    nenh w=23.2u l=1.6u (13, 12, 14);
    nenh w=23.2u l=1.6u (19, 15, vss);
    penh w=29.6u l=1.6u (19, 18, vdd);
    penh w=29.6u l=1.6u (14, 19, vdd);
    nenh w=23.2u l=1.6u (14, 17, vss);
    nenh w=23.2u l=1.6u (16, 15, 18);
    penh w=29.6u l=1.6u (16, 18, vdd);
    penh w=29.6u l=1.6u (18, 19, vdd);
    nenh w=23.2u l=1.6u (18, 17, 19);
    nenh w=23.2u l=1.6u (18, 20, vss);
    penh w=29.6u l=1.6u (18, 24, vdd);
    penh w=29.6u l=1.6u (19, 22, vdd);
    nenh w=23.2u l=1.6u (19, 21, vss);
    nenh w=23.2u l=1.6u (CK, 20, 24);
    penh w=29.6u l=1.6u (CK, 24, vdd);
    penh w=29.6u l=1.6u (CK, 22, vdd);
    nenh w=23.2u l=1.6u (CK, 21, 22);
    nenh w=23.2u l=1.6u (Q, 23, vss);
    penh w=29.6u l=1.6u (Q, QN, vdd);
    penh w=29.6u l=1.6u (22, Q, vdd);
    nenh w=23.2u l=1.6u (22, 25, vss);
    nenh w=23.2u l=1.6u (24, 23, QN);
    penh w=29.6u l=1.6u (24, QN, vdd);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    nenh w=23.2u l=1.6u (QN, 25, Q);
}

extern network dfr10 (terminal D, R, CK, Q, QN)

network rer10 (terminal D, INH, R, CK, Q, QN)
{
    net {Z1, Z2};
    net {Z1, Z3};
    net {Z1, Z4};
    net {Z1, Data};
    @ invert (INH, INHN);
    nenh (INHN, D, Z1);
    penh (INH, D, Z1);
    nenh (INH, Q, Z1);
    penh (INHN, Q, Z1);
    dfr10 (Z1, R, CK, Q, QN);
}

network Col10 (terminal A, B, AEBI, AGBI, ALB, AEB, AGB, vss, vdd)
{
    nenh w=23.2u l=1.6u (AEBI, 6, vss);
    penh w=29.6u l=1.6u (AEBI, 6, vdd);
    penh w=29.6u l=1.6u (A, 10, vdd);
    nenh w=23.2u l=1.6u (A, 10, vss);
    nenh w=23.2u l=1.6u (AGBI, 1, vss);
    penh w=29.6u l=1.6u (AGBI, 1, vdd);
    penh w=29.6u l=1.6u (B, 3, vdd);
    nenh w=23.2u l=1.6u (B, 3, vss);
    nenh w=23.2u l=1.6u (1, 7, AGB);
    penh w=29.6u l=1.6u (1, AGB, vdd);
    penh w=29.6u l=1.6u (A, 5, vdd);
    nenh w=23.2u l=1.6u (A, 2, vss);
    nenh w=23.2u l=1.6u (10, 7, vss);
    penh w=29.6u l=1.6u (10, 4, AGB);
    penh w=29.6u l=1.6u (3, 5, vdd);
    nenh w=23.2u l=1.6u (3, 2, AEB);
    nenh w=23.2u l=1.6u (6, 7, vss);
    penh w=29.6u l=1.6u (6, 4, 8);
    penh w=29.6u l=1.6u (6, 5, 12);
    nenh w=23.2u l=1.6u (6, AEB, vss);
    nenh w=23.2u l=1.6u (B, 7, vss);
    penh w=29.6u l=1.6u (B, 8, vdd);
    penh w=29.6u l=1.6u (B, 12, AEB);
    nenh w=23.2u l=1.6u (B, 9, vss);
    nenh w=23.2u l=1.6u (AGB, ALB, vss);
    penh w=29.6u l=1.6u (AGB, 11, vdd);
    penh w=29.6u l=1.6u (10, 12, AEB);
    nenh w=23.2u l=1.6u (10, 9, AEB);
    nenh w=23.2u l=1.6u (AEB, ALB, vss);
    penh w=29.6u l=1.6u (AEB, 11, ALB);
}

network col10_orig (terminal A, B, AGBI, AEBI, ALB, AEB, AGB, vss, vdd)
{
    penh (A, vdd, notA);
    penh (B, vdd, notB);
    penh (AEBI, vdd, notAEBI);
    penh (AGBI, vdd, notAGBI);
    penh (A, vdd, x);
    penh (B, x, y);
    penh (notB, vdd, x);
    penh (notA, x, y);
    penh (notAEBI, y, AEB);
    penh (B, vdd, z);
    penh (notA, z, u);
    penh (notAEBI, u, AGB);
    penh (notAGBI, vdd, AGB);
    penh (AEB, vdd, v);
    penh (AGB, v, ALB);
    nenh (A, notA, vss);
    nenh (B, notB, vss);
    nenh (AEBI, notAEBI, vss);
    nenh (AGBI, notAGBI, vss);
    nenh (A, k, vss);
    nenh (B, l, vss);
    nenh (notB, AEB, k);
    nenh (notA, AEB, l);
    nenh (notAEBI, AEB, vss);
    nenh (notA, m, vss);
    nenh (B, m, vss);
    nenh (notAEBI, m, vss);
    nenh (notAGBI, AGB, m);
    nenh (AEB, ALB, vss);
    nenh (AGB, ALB, vss);
}

network col10 (terminal A, B, AEBI, AGBI, ALB, AEB, AGB, vss, vdd)
{
    nenh w=23.2u l=1.6u (AEBI, 6, vss);
    penh w=29.6u l=1.6u (AEBI, 6, vdd);
    penh w=29.6u l=1.6u (A, 10, vdd);
    nenh w=23.2u l=1.6u (A, 10, vss);
    nenh w=23.2u l=1.6u (AGBI, 1, vss);
    penh w=29.6u l=1.6u (AGBI, 1, vdd);
    penh w=29.6u l=1.6u (B, 3, vdd);
    nenh w=23.2u l=1.6u (B, 3, vss);
    nenh w=23.2u l=1.6u (1, 7, AGB);
    penh w=29.6u l=1.6u (1, AGB, vdd);
    penh w=29.6u l=1.6u (A, 5, vdd);
    nenh w=23.2u l=1.6u (A, 2, vss);
    nenh w=23.2u l=1.6u (10, 7, vss);
    penh w=29.6u l=1.6u (10, 4, AGB);
    penh w=29.6u l=1.6u (3, 5, vdd);
    nenh w=23.2u l=1.6u (3, 2, AEB);
    nenh w=23.2u l=1.6u (6, 7, vss);
    penh w=29.6u l=1.6u (6, 4, 8);
    penh w=29.6u l=1.6u (6, 5, 12);
    nenh w=23.2u l=1.6u (6, AEB, vss);
    nenh w=23.2u l=1.6u (B, 7, vss);
    penh w=29.6u l=1.6u (B, 8, vdd);
    penh w=29.6u l=1.6u (B, 12, AEB);
    nenh w=23.2u l=1.6u (B, 9, vss);
    nenh w=23.2u l=1.6u (AGB, ALB, vss);
    penh w=29.6u l=1.6u (AGB, 11, vdd);
    penh w=29.6u l=1.6u (10, 12, AEB);
    nenh w=23.2u l=1.6u (10, 9, AEB);
    nenh w=23.2u l=1.6u (AEB, ALB, vss);
    penh w=29.6u l=1.6u (AEB, 11, ALB);
}

network ln3x3 (terminal g, d, s)
{
    nenh w=23.2u l=1.6u (g, 7, s);
    nenh w=23.2u l=1.6u (g, 7, 14);
    nenh w=23.2u l=1.6u (g, 14, d);
    nenh w=23.2u l=1.6u (g, 27, d);
    nenh w=23.2u l=1.6u (g, 27, 34);
    nenh w=23.2u l=1.6u (g, 34, s);
    nenh w=23.2u l=1.6u (g, s, 47);
    nenh w=23.2u l=1.6u (g, 47, 54);
    nenh w=23.2u l=1.6u (g, 54, d);
}

network lp3x3 (terminal g, d, s)
{
    penh w=29.6u l=1.6u (g, 9, d);
    penh w=29.6u l=1.6u (g, 9, 16);
    penh w=29.6u l=1.6u (g, 16, s);
    penh w=29.6u l=1.6u (g, 29, s);
    penh w=29.6u l=1.6u (g, 29, 36);
    penh w=29.6u l=1.6u (g, 36, d);
    penh w=29.6u l=1.6u (g, d, 49);
    penh w=29.6u l=1.6u (g, 49, 56);
    penh w=29.6u l=1.6u (g, 56, s);
}

network mir_nin (terminal in, g, vss, vdd)
{
    nenh w=23.2u l=1.6u (in, 7, in);
    nenh w=23.2u l=1.6u (in, 7, 14);
    nenh w=23.2u l=1.6u (in, 14, g);
    nenh w=23.2u l=1.6u (in, 27, g);
    nenh w=23.2u l=1.6u (in, 27, 34);
    nenh w=23.2u l=1.6u (in, 34, in);
    nenh w=23.2u l=1.6u (in, 47, in);
    nenh w=23.2u l=1.6u (in, 47, 54);
    nenh w=23.2u l=1.6u (in, 54, g);
    nenh w=23.2u l=1.6u (g, 67, g);
    nenh w=23.2u l=1.6u (g, 67, 74);
    nenh w=23.2u l=1.6u (g, 74, vss);
    nenh w=23.2u l=1.6u (g, 87, vss);
    nenh w=23.2u l=1.6u (g, 87, 94);
    nenh w=23.2u l=1.6u (g, 94, g);
    nenh w=23.2u l=1.6u (g, 107, g);
    nenh w=23.2u l=1.6u (g, 107, 114);
    nenh w=23.2u l=1.6u (g, 114, vss);
}

network mir_nout (terminal in, g, out, vss, vdd)
{
    nenh w=23.2u l=1.6u (in, 7, out);
    nenh w=23.2u l=1.6u (in, 7, 14);
    nenh w=23.2u l=1.6u (in, 14, 101);
    nenh w=23.2u l=1.6u (in, 27, 101);
    nenh w=23.2u l=1.6u (in, 27, 34);
    nenh w=23.2u l=1.6u (in, 34, out);
    nenh w=23.2u l=1.6u (in, out, 47);
    nenh w=23.2u l=1.6u (in, 47, 54);
    nenh w=23.2u l=1.6u (in, 54, 101);
    nenh w=23.2u l=1.6u (g, 67, 101);
    nenh w=23.2u l=1.6u (g, 67, 74);
    nenh w=23.2u l=1.6u (g, 74, vss);
    nenh w=23.2u l=1.6u (g, 87, vss);
    nenh w=23.2u l=1.6u (g, 87, 94);
    nenh w=23.2u l=1.6u (g, 94, 101);
    nenh w=23.2u l=1.6u (g, 101, 108);
    nenh w=23.2u l=1.6u (g, 108, 115);
    nenh w=23.2u l=1.6u (g, 115, vss);
}

network mir_pin (terminal in, g, vss, vdd)
{
    penh w=29.6u l=1.6u (in, 9, in);
    penh w=29.6u l=1.6u (in, 9, 16);
    penh w=29.6u l=1.6u (in, 16, g);
    penh w=29.6u l=1.6u (in, 29, g);
    penh w=29.6u l=1.6u (in, 29, 36);
    penh w=29.6u l=1.6u (in, 36, in);
    penh w=29.6u l=1.6u (in, 49, in);
    penh w=29.6u l=1.6u (in, 49, 56);
    penh w=29.6u l=1.6u (in, 56, g);
    penh w=29.6u l=1.6u (g, 69, g);
    penh w=29.6u l=1.6u (g, 69, 76);
    penh w=29.6u l=1.6u (g, 76, vdd);
    penh w=29.6u l=1.6u (g, 89, vdd);
    penh w=29.6u l=1.6u (g, 89, 96);
    penh w=29.6u l=1.6u (g, 96, g);
    penh w=29.6u l=1.6u (g, 109, g);
    penh w=29.6u l=1.6u (g, 109, 116);
    penh w=29.6u l=1.6u (g, 116, vdd);
}

network mir_pout (terminal in, g, out, vss, vdd)
{
    penh w=29.6u l=1.6u (in, 9, out);
    penh w=29.6u l=1.6u (in, 9, 16);
    penh w=29.6u l=1.6u (in, 16, 103);
    penh w=29.6u l=1.6u (in, 29, 103);
    penh w=29.6u l=1.6u (in, 29, 36);
    penh w=29.6u l=1.6u (in, 36, out);
    penh w=29.6u l=1.6u (in, out, 49);
    penh w=29.6u l=1.6u (in, 49, 56);
    penh w=29.6u l=1.6u (in, 56, 103);
    penh w=29.6u l=1.6u (g, 69, 103);
    penh w=29.6u l=1.6u (g, 69, 76);
    penh w=29.6u l=1.6u (g, 76, vdd);
    penh w=29.6u l=1.6u (g, 89, vdd);
    penh w=29.6u l=1.6u (g, 89, 96);
    penh w=29.6u l=1.6u (g, 96, 103);
    penh w=29.6u l=1.6u (g, 103, 110);
    penh w=29.6u l=1.6u (g, 110, 117);
    penh w=29.6u l=1.6u (g, 117, vdd);
}

network mu111 (terminal A, B, S, Y)
{
    @ invert (A, an);
    @ invert (B, bn);
    @ invert (S, sn);
    @ invert (yn, Y);
    nenh (sn, an, yn);
    penh (S, an, yn);
    nenh (S, bn, yn);
    penh (sn, bn, yn);
}

network Mu111 (terminal A, B, S, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (S, 14, vss);
    penh w=29.6u l=1.6u (S, 14, vdd);
    nenh w=23.2u l=1.6u (A, 9, vss);
    penh w=29.6u l=1.6u (A, 9, vdd);
    nenh w=23.2u l=1.6u (14, 9, 24);
    penh w=29.6u l=1.6u (S, 9, 24);
    nenh w=23.2u l=1.6u (S, 19, 24);
    penh w=29.6u l=1.6u (14, 19, 24);
    nenh w=23.2u l=1.6u (B, 19, vss);
    penh w=29.6u l=1.6u (B, 19, vdd);
    nenh w=23.2u l=1.6u (24, Y, vss);
    penh w=29.6u l=1.6u (24, Y, vdd);
}

network De211 (terminal A, B, Y0, Y1, Y2, Y3, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 6, vss);
    penh w=29.6u l=1.6u (A, 6, vdd);
    penh w=29.6u l=1.6u (B, 4, vdd);
    nenh w=23.2u l=1.6u (B, 4, vss);
    nenh w=23.2u l=1.6u (A, Y2, vss);
    penh w=29.6u l=1.6u (A, 1, vdd);
    penh w=29.6u l=1.6u (B, 2, vdd);
    nenh w=23.2u l=1.6u (B, Y0, vss);
    nenh w=23.2u l=1.6u (4, Y2, vss);
    penh w=29.6u l=1.6u (4, 1, Y2);
    penh w=29.6u l=1.6u (A, 2, Y0);
    nenh w=23.2u l=1.6u (A, Y0, vss);
    nenh w=23.2u l=1.6u (6, Y3, vss);
    penh w=29.6u l=1.6u (6, 3, vdd);
    penh w=29.6u l=1.6u (B, 5, vdd);
    nenh w=23.2u l=1.6u (B, Y1, vss);
    nenh w=23.2u l=1.6u (4, Y3, vss);
    penh w=29.6u l=1.6u (4, 3, Y3);
    penh w=29.6u l=1.6u (6, 5, Y1);
    nenh w=23.2u l=1.6u (6, Y1, vss);
}

network de211 (terminal A, B, Y0, Y1, Y2, Y3)
{
    @ nor (A, B, Y0);
    @ nor (an, B, Y1);
    @ nor (A, bn, Y2);
    @ nor (an, bn, Y3);
    @ invert (A, an);
    @ invert (B, bn);
}

network Buf20 (terminal A, Y, vss, vdd)
{
    nenh w=23.2u l=1.6u (A, 9, vss);
    penh w=29.6u l=1.6u (A, 9, vdd);
    nenh w=23.2u l=1.6u (9, Y, vss);
    penh w=29.6u l=1.6u (9, Y, vdd);
    nenh w=23.2u l=1.6u (9, Y, vss);
    penh w=29.6u l=1.6u (9, Y, vdd);
}

network buf20 (terminal A, Y)
{
    @ invert (A, an);
    @ invert (an, Y);
}
