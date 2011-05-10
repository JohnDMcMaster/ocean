
network buf20 (terminal A, Y, vss, vdd)
{
    nenh w=5.0912u l=848.53n (23, vss, Y);
    nenh w=5.0912u l=848.53n (A, vss, 23);
    penh w=12.351u l=848.53n (23, Y, vdd);
    penh w=12.351u l=848.53n (A, vdd, 23);
}

network iv110 (terminal A, Y, vss, vdd)
{
    nenh w=5.0912u l=848.53n (A, vss, Y);
    penh w=12.351u l=848.53n (A, vdd, Y);
}

network na210 (terminal A, B, Y, vss, vdd)
{
    nenh w=5.0912u l=848.53n (A, vss, 10);
    nenh w=4.8u l=800n (B, 10, Y);
    penh w=12.351u l=848.53n (A, vdd, Y);
    penh w=12u l=800n (B, vdd, Y);
}

network na310 (terminal A, B, C, Y, vss, vdd)
{
    nenh w=5.0912u l=848.53n (C, 7, Y);
    nenh w=5.0912u l=848.53n (A, 7, 9);
    nenh w=4.8u l=800n (B, vss, 9);
    penh w=12.351u l=848.53n (C, vdd, Y);
    penh w=12.351u l=848.53n (A, vdd, Y);
    penh w=12u l=800n (B, vdd, Y);
}

network no210 (terminal A, B, Y, vss, vdd)
{
    nenh w=5.0912u l=848.53n (A, vss, Y);
    nenh w=4.8u l=800n (B, vss, Y);
    penh w=12.351u l=848.53n (A, vdd, 23);
    penh w=12u l=800n (B, Y, 23);
}

network no310 (terminal A, B, C, Y, vss, vdd)
{
    nenh w=5.0912u l=848.53n (C, vss, Y);
    nenh w=5.0912u l=848.53n (A, vss, Y);
    nenh w=4.8u l=800n (B, vss, Y);
    penh w=12.351u l=848.53n (C, Y, 18);
    penh w=12.351u l=848.53n (A, 18, 20);
    penh w=12u l=800n (B, vdd, 20);
}

network dfr11 (terminal D, R, CK, Q, QN, vss, vdd)
{
    nenh w=4.8u l=800n (81, QN, vss);
    nenh w=5.0912u l=848.53n (81, QN, vss);
    nenh w=5.0912u l=848.53n (CK, 79, vss);
    nenh w=5.0912u l=848.53n (75, Q, vss);
    nenh w=4.8u l=800n (75, Q, vss);
    penh w=12u l=800n (81, QN, vdd);
    penh w=12.351u l=848.53n (81, QN, vdd);
    penh w=12.351u l=848.53n (CK, 79, vdd);
    penh w=12.351u l=848.53n (75, Q, vdd);
    penh w=12u l=800n (75, Q, vdd);
    penh w=12u l=800n (78, 67, vdd);
    penh w=12u l=800n (79, 67, 80);
    penh w=12.351u l=848.53n (67, 74, vdd);
    penh w=12.351u l=848.53n (CK, 75, 80);
    nenh w=4.8u l=800n (78, 67, vss);
    nenh w=4.8u l=800n (CK, 67, 80);
    penh w=12.351u l=848.53n (79, 74, 78);
    penh w=12.351u l=848.53n (81, 75, vdd);
    nenh w=5.0912u l=848.53n (67, 74, vss);
    nenh w=5.0912u l=848.53n (79, 75, 80);
    penh w=12u l=800n (CK, 78, 128);
    penh w=12u l=800n (80, 81, vdd);
    nenh w=5.0912u l=848.53n (CK, 74, 78);
    nenh w=5.0912u l=848.53n (81, 75, vss);
    nenh w=4.8u l=800n (79, 78, 128);
    nenh w=4.8u l=800n (80, 81, vss);
    nenh w=5.0912u l=848.53n (139, vss, 128);
    nenh w=5.0912u l=848.53n (R, vss, 138);
    nenh w=5.0912u l=848.53n (138, vss, 109);
    nenh w=4.8u l=800n (D, 109, 139);
    penh w=12.351u l=848.53n (139, 128, vdd);
    penh w=12.351u l=848.53n (R, 138, vdd);
    penh w=12.351u l=848.53n (138, 139, vdd);
    penh w=12u l=800n (D, 139, vdd);
}
