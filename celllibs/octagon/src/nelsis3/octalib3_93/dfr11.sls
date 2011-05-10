
network Dfr11 (terminal Q, QN, C, D, R, vss, vdd)
{
    nenh w=4.8u l=800n (81, QN, vss);
    nenh w=5.0912u l=848.53n (81, QN, vss);
    nenh w=5.0912u l=848.53n (C, 79, vss);
    nenh w=5.0912u l=848.53n (75, Q, vss);
    nenh w=4.8u l=800n (75, Q, vss);
    penh w=12u l=800n (81, QN, vdd);
    penh w=12.351u l=848.53n (81, QN, vdd);
    penh w=12.351u l=848.53n (C, 79, vdd);
    penh w=12.351u l=848.53n (75, Q, vdd);
    penh w=12u l=800n (75, Q, vdd);
    penh w=12u l=800n (78, 67, vdd);
    penh w=12u l=800n (79, 67, 80);
    penh w=12.351u l=848.53n (67, 74, vdd);
    penh w=12.351u l=848.53n (C, 75, 80);
    nenh w=4.8u l=800n (78, 67, vss);
    nenh w=4.8u l=800n (C, 67, 80);
    penh w=12.351u l=848.53n (79, 74, 78);
    penh w=12.351u l=848.53n (81, 75, vdd);
    nenh w=5.0912u l=848.53n (67, 74, vss);
    nenh w=5.0912u l=848.53n (79, 75, 80);
    penh w=12u l=800n (C, 78, 128);
    penh w=12u l=800n (80, 81, vdd);
    nenh w=5.0912u l=848.53n (C, 74, 78);
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
