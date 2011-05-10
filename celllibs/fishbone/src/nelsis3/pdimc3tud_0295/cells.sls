/******************************************************************************
 *
 * @(#) cells.sls 1.2	Thu Oct 13 1994
 *
 * This file is automatically created by ldm2sls from source file
 * 'cells.ldm 1.2 Oct. 13, 1994'
 * and contains the circuit description of the cells of library 'pdimc3tud'
 *
 ******************************************************************************/

network an02d1 (terminal A1, A2, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, 10);
    penh w=29.6u l=1.6u (A1, 10, vdd);
    cap 17.80156f (A1, vss);
    cap 33.62304f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, vss);
    penh w=29.6u l=1.6u (A2, 10, vdd);
    cap 17.25418f (A2, vss);
    nenh w=23.2u l=1.6u (10, Z, vss);
    penh w=29.6u l=1.6u (10, Z, vdd);
    cap 168.9605f (10, vss);
    cap 139.062f (Z, vss);
    cap 253.8144f (vdd, vss);
}

network an03d1 (terminal A1, A2, A3, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, 15);
    penh w=29.6u l=1.6u (A1, 15, vdd);
    cap 17.25418f (A1, vss);
    cap 33.62304f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, 10);
    penh w=29.6u l=1.6u (A2, 15, vdd);
    cap 18.32412f (A2, vss);
    cap 33.62304f (10, vss);
    nenh w=23.2u l=1.6u (A3, 10, vss);
    penh w=29.6u l=1.6u (A3, 15, vdd);
    cap 17.25418f (A3, vss);
    nenh w=23.2u l=1.6u (15, Z, vss);
    penh w=29.6u l=1.6u (15, Z, vdd);
    cap 297.6748f (15, vss);
    cap 136.7158f (Z, vss);
    cap 230.9048f (vdd, vss);
}

network an04d1 (terminal A1, A2, A3, A4, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, 20);
    penh w=29.6u l=1.6u (A1, 20, vdd);
    cap 17.80156f (A1, vss);
    cap 33.62304f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, 10);
    penh w=29.6u l=1.6u (A2, 20, vdd);
    cap 17.25418f (A2, vss);
    cap 33.62304f (10, vss);
    nenh w=23.2u l=1.6u (A3, 10, 15);
    penh w=29.6u l=1.6u (A3, 20, vdd);
    cap 18.32412f (A3, vss);
    cap 33.62304f (15, vss);
    nenh w=23.2u l=1.6u (A4, 15, vss);
    penh w=29.6u l=1.6u (A4, 20, vdd);
    cap 17.25418f (A4, vss);
    nenh w=23.2u l=1.6u (20, Z, vss);
    penh w=29.6u l=1.6u (20, Z, vdd);
    cap 271.2012f (20, vss);
    cap 133.7273f (Z, vss);
    cap 360.6999f (vdd, vss);
}

network ao01d1 (terminal A1, A2, B1, B2, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, ZN);
    penh w=29.6u l=1.6u (A1, 19, vdd);
    cap 15.69586f (A1, vss);
    cap 33.62304f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, vss);
    penh w=29.6u l=1.6u (A2, 19, vdd);
    cap 16.76581f (A2, vss);
    nenh w=23.2u l=1.6u (B2, 14, vss);
    penh w=29.6u l=1.6u (B2, ZN, 19);
    cap 16.76581f (B2, vss);
    cap 33.62304f (14, vss);
    nenh w=23.2u l=1.6u (B1, 14, ZN);
    penh w=29.6u l=1.6u (B1, ZN, 19);
    cap 16.21842f (B1, vss);
    cap 195.2252f (ZN, vss);
    cap 323.5078f (19, vss);
    cap 126.9326f (vdd, vss);
}

network ao04d1 (terminal A1, A2, B, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, vss);
    penh w=29.6u l=1.6u (A1, 10, vdd);
    cap 17.80156f (A1, vss);
    cap 32.1792f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, ZN);
    penh w=29.6u l=1.6u (A2, 10, vdd);
    cap 17.80156f (A2, vss);
    nenh w=23.2u l=1.6u (B, ZN, vss);
    cap 224.0925f (10, vss);
    penh w=29.6u l=1.6u (B, 10, ZN);
    cap 16.73162f (B, vss);
    cap 138.8017f (ZN, vss);
    cap 124.3985f (vdd, vss);
}

network ao05d1 (terminal A1, A2, B, C, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, vss);
    penh w=29.6u l=1.6u (A1, 10, vdd);
    cap 17.80156f (A1, vss);
    cap 32.1792f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, ZN);
    penh w=29.6u l=1.6u (A2, 10, vdd);
    cap 17.80156f (A2, vss);
    nenh w=23.2u l=1.6u (B, ZN, vss);
    cap 224.0925f (10, vss);
    penh w=29.6u l=1.6u (B, 10, 15);
    cap 16.73162f (B, vss);
    nenh w=23.2u l=1.6u (C, ZN, vss);
    cap 93.39552f (15, vss);
    penh w=29.6u l=1.6u (C, 15, ZN);
    cap 17.80156f (C, vss);
    cap 174.6004f (ZN, vss);
    cap 127.7841f (vdd, vss);
}

network dfctnb (terminal D, CP, CDN, QN, Q, vss, vdd)
{
    nenh w=23.2u l=1.6u (38, D, 23);
    cap 193.5663f (D, vss);
    penh w=29.6u l=1.6u (CP, D, 23);
    nenh w=23.2u l=1.6u (CP, 9, 23);
    penh w=29.6u l=1.6u (38, 10, 23);
    cap 37.68384f (9, vss);
    nenh w=23.2u l=1.6u (33, 9, vss);
    cap 99.22176f (10, vss);
    penh w=29.6u l=1.6u (33, 10, vdd);
    nenh w=23.2u l=1.6u (CP, 38, vss);
    penh w=29.6u l=1.6u (CP, 38, vdd);
    nenh w=23.2u l=1.6u (23, 28, vss);
    penh w=29.6u l=1.6u (23, 33, vdd);
    cap 184.6355f (23, vss);
    cap 35.9184f (28, vss);
    nenh w=23.2u l=1.6u (CDN, 28, 33);
    penh w=29.6u l=1.6u (CDN, 33, vdd);
    nenh w=23.2u l=1.6u (CP, 33, QN);
    cap 294.0624f (33, vss);
    penh w=29.6u l=1.6u (38, 33, QN);
    nenh w=23.2u l=1.6u (38, 43, QN);
    cap 209.9371f (38, vss);
    penh w=29.6u l=1.6u (CP, 53, QN);
    cap 104.436f (CP, vss);
    cap 33.62304f (43, vss);
    nenh w=23.2u l=1.6u (CDN, 43, 48);
    penh w=29.6u l=1.6u (CDN, 53, vdd);
    cap 43.81026f (CDN, vss);
    cap 37.68384f (48, vss);
    nenh w=23.2u l=1.6u (Q, 48, vss);
    penh w=29.6u l=1.6u (Q, 53, vdd);
    cap 196.5664f (53, vss);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    cap 167.0052f (QN, vss);
    cap 169.4581f (Q, vss);
    cap 480.481f (vdd, vss);
}

network dfntnb (terminal D, CP, QN, Q, vss, vdd)
{
    nenh w=23.2u l=1.6u (47, D, 15);
    cap 194.0162f (D, vss);
    penh w=29.6u l=1.6u (CP, D, 15);
    nenh w=23.2u l=1.6u (CP, 9, 15);
    penh w=29.6u l=1.6u (47, 10, 15);
    cap 37.41312f (9, vss);
    nenh w=23.2u l=1.6u (20, 9, vss);
    cap 98.04864f (10, vss);
    penh w=29.6u l=1.6u (20, 10, vdd);
    nenh w=23.2u l=1.6u (15, 20, vss);
    penh w=29.6u l=1.6u (15, 20, vdd);
    cap 171.4463f (15, vss);
    nenh w=23.2u l=1.6u (CP, 20, QN);
    cap 173.6405f (20, vss);
    penh w=29.6u l=1.6u (47, 20, QN);
    nenh w=23.2u l=1.6u (47, 29, QN);
    penh w=29.6u l=1.6u (CP, 30, QN);
    cap 36.24f (29, vss);
    nenh w=23.2u l=1.6u (Q, 29, vss);
    cap 97.72704f (30, vss);
    penh w=29.6u l=1.6u (Q, 30, vdd);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    cap 173.9784f (QN, vss);
    cap 164.3187f (Q, vss);
    nenh w=23.2u l=1.6u (CP, 47, vss);
    penh w=29.6u l=1.6u (CP, 47, vdd);
    cap 94.27154f (CP, vss);
    cap 236.5645f (47, vss);
    cap 364.8784f (vdd, vss);
}

network in01d1 (terminal I, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    cap 17.25418f (I, vss);
    cap 139.062f (ZN, vss);
    cap 145.3862f (vdd, vss);
}

network in01d2 (terminal I, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    cap 34.69761f (I, vss);
    cap 137.8991f (ZN, vss);
    cap 244.977f (vdd, vss);
}

network in01d3 (terminal I, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    cap 53.18617f (I, vss);
    cap 269.8614f (ZN, vss);
    cap 249.9566f (vdd, vss);
}

network in01d4 (terminal I, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    nenh w=23.2u l=1.6u (I, ZN, vss);
    penh w=29.6u l=1.6u (I, ZN, vdd);
    cap 69.53483f (I, vss);
    cap 274.0332f (ZN, vss);
    cap 350.3101f (vdd, vss);
}

network it01d1 (terminal I, OE, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (OE, 9, vss);
    penh w=29.6u l=1.6u (OE, 9, vdd);
    nenh w=23.2u l=1.6u (OE, 9, vss);
    penh w=29.6u l=1.6u (OE, 9, vdd);
    nenh w=23.2u l=1.6u (OE, 14, vss);
    cap 44.81811f (OE, vss);
    penh w=29.6u l=1.6u (9, 15, vdd);
    cap 154.1703f (9, vss);
    cap 32.1792f (14, vss);
    nenh w=23.2u l=1.6u (I, 14, ZN);
    cap 93.39552f (15, vss);
    penh w=29.6u l=1.6u (I, 15, ZN);
    cap 16.20906f (I, vss);
    cap 136.7158f (ZN, vss);
    cap 253.9803f (vdd, vss);
}

network it02d1 (terminal I, OEN, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (OEN, 9, vss);
    penh w=29.6u l=1.6u (OEN, 9, vdd);
    nenh w=23.2u l=1.6u (OEN, 9, vss);
    penh w=29.6u l=1.6u (OEN, 9, vdd);
    nenh w=23.2u l=1.6u (9, 14, vss);
    cap 154.1573f (9, vss);
    penh w=29.6u l=1.6u (OEN, 15, vdd);
    cap 44.83111f (OEN, vss);
    cap 32.1792f (14, vss);
    nenh w=23.2u l=1.6u (I, 14, ZN);
    cap 93.39552f (15, vss);
    penh w=29.6u l=1.6u (I, 15, ZN);
    cap 16.20906f (I, vss);
    cap 136.7158f (ZN, vss);
    cap 253.9803f (vdd, vss);
}

network lacfnb (terminal D, EN, CDN, QN, Q, vss, vdd)
{
    nenh w=23.2u l=1.6u (27, D, Q);
    cap 146.5622f (D, vss);
    penh w=29.6u l=1.6u (EN, D, Q);
    nenh w=23.2u l=1.6u (EN, 17, Q);
    penh w=29.6u l=1.6u (27, 18, Q);
    cap 37.68384f (17, vss);
    nenh w=23.2u l=1.6u (QN, 17, vss);
    cap 96.2832f (18, vss);
    penh w=29.6u l=1.6u (QN, 18, vdd);
    nenh w=23.2u l=1.6u (EN, 27, vss);
    penh w=29.6u l=1.6u (EN, 27, vdd);
    cap 56.80227f (EN, vss);
    cap 169.6116f (27, vss);
    nenh w=23.2u l=1.6u (Q, 36, QN);
    penh w=29.6u l=1.6u (Q, QN, vdd);
    cap 174.9516f (Q, vss);
    cap 36.51072f (36, vss);
    nenh w=23.2u l=1.6u (CDN, 36, vss);
    penh w=29.6u l=1.6u (CDN, QN, vdd);
    cap 17.25418f (CDN, vss);
    cap 178.0744f (QN, vss);
    cap 352.107f (vdd, vss);
}

network lactnb (terminal D, E, CDN, QN, Q, vss, vdd)
{
    nenh w=23.2u l=1.6u (E, D, 24);
    cap 192.6997f (D, vss);
    penh w=29.6u l=1.6u (19, D, 24);
    nenh w=23.2u l=1.6u (19, 9, 24);
    penh w=29.6u l=1.6u (E, 10, 24);
    cap 34.79616f (9, vss);
    nenh w=23.2u l=1.6u (QN, 9, vss);
    cap 96.2832f (10, vss);
    penh w=29.6u l=1.6u (QN, 10, vdd);
    nenh w=23.2u l=1.6u (E, 19, vss);
    penh w=29.6u l=1.6u (E, 19, vdd);
    cap 47.26079f (E, vss);
    cap 170.6052f (19, vss);
    nenh w=23.2u l=1.6u (24, 29, QN);
    penh w=29.6u l=1.6u (24, QN, vdd);
    cap 178.4726f (24, vss);
    cap 33.62304f (29, vss);
    nenh w=23.2u l=1.6u (CDN, 29, vss);
    penh w=29.6u l=1.6u (CDN, QN, vdd);
    cap 17.25418f (CDN, vss);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    cap 192.6849f (QN, vss);
    cap 139.062f (Q, vss);
    cap 461.9106f (vdd, vss);
}

network mfctnb (terminal DA, DB, CP, SA, CDN, QN, Q, vss, vdd)
{
    nenh w=23.2u l=1.6u (SA, 15, vss);
    penh w=29.6u l=1.6u (SA, 15, vdd);
    nenh w=23.2u l=1.6u (SA, 9, vss);
    penh w=29.6u l=1.6u (15, 10, vdd);
    cap 37.63312f (9, vss);
    nenh w=23.2u l=1.6u (DA, 9, 26);
    cap 99.00704f (10, vss);
    penh w=29.6u l=1.6u (DA, 10, 26);
    cap 37.62189f (DA, vss);
    nenh w=23.2u l=1.6u (15, 20, 26);
    cap 204.8707f (15, vss);
    penh w=29.6u l=1.6u (SA, 21, 26);
    cap 48.61679f (SA, vss);
    cap 33.62304f (20, vss);
    nenh w=23.2u l=1.6u (DB, 20, vss);
    cap 94.83936f (21, vss);
    penh w=29.6u l=1.6u (DB, 21, vdd);
    cap 17.25418f (DB, vss);
    nenh w=23.2u l=1.6u (26, 31, vss);
    penh w=29.6u l=1.6u (26, 31, vdd);
    cap 163.0685f (26, vss);
    nenh w=23.2u l=1.6u (69, 31, 54);
    cap 156.9033f (31, vss);
    penh w=29.6u l=1.6u (CP, 31, 54);
    nenh w=23.2u l=1.6u (CP, 40, 54);
    penh w=29.6u l=1.6u (69, 41, 54);
    cap 37.68384f (40, vss);
    nenh w=23.2u l=1.6u (64, 40, vss);
    cap 99.22176f (41, vss);
    penh w=29.6u l=1.6u (64, 41, vdd);
    nenh w=23.2u l=1.6u (CP, 69, vss);
    penh w=29.6u l=1.6u (CP, 69, vdd);
    nenh w=23.2u l=1.6u (54, 59, vss);
    penh w=29.6u l=1.6u (54, 64, vdd);
    cap 184.6355f (54, vss);
    cap 35.9184f (59, vss);
    nenh w=23.2u l=1.6u (CDN, 59, 64);
    penh w=29.6u l=1.6u (CDN, 64, vdd);
    nenh w=23.2u l=1.6u (CP, 64, QN);
    cap 294.0624f (64, vss);
    penh w=29.6u l=1.6u (69, 64, QN);
    nenh w=23.2u l=1.6u (69, 74, QN);
    cap 209.9371f (69, vss);
    penh w=29.6u l=1.6u (CP, 84, QN);
    cap 104.436f (CP, vss);
    cap 33.62304f (74, vss);
    nenh w=23.2u l=1.6u (CDN, 74, 79);
    penh w=29.6u l=1.6u (CDN, 84, vdd);
    cap 43.81026f (CDN, vss);
    cap 37.68384f (79, vss);
    nenh w=23.2u l=1.6u (Q, 79, vss);
    penh w=29.6u l=1.6u (Q, 84, vdd);
    cap 196.5664f (84, vss);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    cap 167.0052f (QN, vss);
    cap 168.5163f (Q, vss);
    cap 701.7084f (vdd, vss);
}

network mfntnb (terminal DA, DB, CP, SA, QN, Q, vss, vdd)
{
    nenh w=23.2u l=1.6u (SA, 15, vss);
    penh w=29.6u l=1.6u (SA, 15, vdd);
    nenh w=23.2u l=1.6u (SA, 9, vss);
    penh w=29.6u l=1.6u (15, 10, vdd);
    cap 37.63312f (9, vss);
    nenh w=23.2u l=1.6u (DA, 9, 26);
    cap 99.00704f (10, vss);
    penh w=29.6u l=1.6u (DA, 10, 26);
    cap 37.62189f (DA, vss);
    nenh w=23.2u l=1.6u (15, 20, 26);
    cap 204.8707f (15, vss);
    penh w=29.6u l=1.6u (SA, 21, 26);
    cap 48.61679f (SA, vss);
    cap 33.62304f (20, vss);
    nenh w=23.2u l=1.6u (DB, 20, vss);
    cap 94.83936f (21, vss);
    penh w=29.6u l=1.6u (DB, 21, vdd);
    cap 17.25418f (DB, vss);
    nenh w=23.2u l=1.6u (26, 43, vss);
    penh w=29.6u l=1.6u (26, 43, vdd);
    cap 160.4329f (26, vss);
    nenh w=23.2u l=1.6u (CP, 68, vss);
    penh w=29.6u l=1.6u (CP, 68, vdd);
    nenh w=23.2u l=1.6u (68, 43, 58);
    cap 300.7126f (43, vss);
    penh w=29.6u l=1.6u (CP, 43, 58);
    nenh w=23.2u l=1.6u (CP, 52, 58);
    penh w=29.6u l=1.6u (68, 53, 58);
    cap 36.24f (52, vss);
    nenh w=23.2u l=1.6u (63, 52, vss);
    cap 98.04864f (53, vss);
    penh w=29.6u l=1.6u (63, 53, vdd);
    nenh w=23.2u l=1.6u (58, 63, vss);
    penh w=29.6u l=1.6u (58, 63, vdd);
    cap 171.4084f (58, vss);
    nenh w=23.2u l=1.6u (CP, 63, QN);
    cap 173.1983f (63, vss);
    penh w=29.6u l=1.6u (68, 63, QN);
    nenh w=23.2u l=1.6u (68, 73, QN);
    cap 230.178f (68, vss);
    penh w=29.6u l=1.6u (CP, 74, QN);
    cap 84.62284f (CP, vss);
    cap 33.35232f (73, vss);
    nenh w=23.2u l=1.6u (Q, 73, vss);
    cap 97.72704f (74, vss);
    penh w=29.6u l=1.6u (Q, 74, vdd);
    nenh w=23.2u l=1.6u (QN, Q, vss);
    penh w=29.6u l=1.6u (QN, Q, vdd);
    cap 165.6224f (QN, vss);
    cap 160.3859f (Q, vss);
    cap 601.7047f (vdd, vss);
}

network mx21d1 (terminal I0, I1, S, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (S, 15, vss);
    penh w=29.6u l=1.6u (S, 15, vdd);
    nenh w=23.2u l=1.6u (S, 9, vss);
    penh w=29.6u l=1.6u (15, 10, vdd);
    cap 37.63312f (9, vss);
    nenh w=23.2u l=1.6u (I1, 9, 26);
    cap 99.00704f (10, vss);
    penh w=29.6u l=1.6u (I1, 10, 26);
    cap 37.62189f (I1, vss);
    nenh w=23.2u l=1.6u (15, 20, 26);
    cap 204.8707f (15, vss);
    penh w=29.6u l=1.6u (S, 21, 26);
    cap 48.61679f (S, vss);
    cap 33.62304f (20, vss);
    nenh w=23.2u l=1.6u (I0, 20, vss);
    cap 94.83936f (21, vss);
    penh w=29.6u l=1.6u (I0, 21, vdd);
    cap 17.25418f (I0, vss);
    nenh w=23.2u l=1.6u (26, Z, vss);
    penh w=29.6u l=1.6u (26, Z, vdd);
    cap 160.4329f (26, vss);
    cap 133.7273f (Z, vss);
    cap 236.0558f (vdd, vss);
}

network nd02d1 (terminal A1, A2, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, vss);
    penh w=29.6u l=1.6u (A1, ZN, vdd);
    cap 18.34895f (A1, vss);
    cap 32.1792f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, ZN);
    penh w=29.6u l=1.6u (A2, ZN, vdd);
    cap 16.73162f (A2, vss);
    cap 138.3606f (ZN, vss);
    cap 246.4209f (vdd, vss);
}

network nd03d1 (terminal A1, A2, A3, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, vss);
    penh w=29.6u l=1.6u (A1, ZN, vdd);
    cap 18.34895f (A1, vss);
    cap 32.1792f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, 10);
    penh w=29.6u l=1.6u (A2, ZN, vdd);
    cap 16.73162f (A2, vss);
    cap 32.1792f (10, vss);
    nenh w=23.2u l=1.6u (A3, 10, ZN);
    penh w=29.6u l=1.6u (A3, ZN, vdd);
    cap 17.80156f (A3, vss);
    cap 236.0687f (ZN, vss);
    cap 250.8081f (vdd, vss);
}

network nd04d1 (terminal A1, A2, A3, A4, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 5, vss);
    penh w=29.6u l=1.6u (A1, ZN, vdd);
    cap 18.34895f (A1, vss);
    cap 32.1792f (5, vss);
    nenh w=23.2u l=1.6u (A2, 5, 10);
    penh w=29.6u l=1.6u (A2, ZN, vdd);
    cap 16.73162f (A2, vss);
    cap 32.1792f (10, vss);
    nenh w=23.2u l=1.6u (A3, 10, 15);
    penh w=29.6u l=1.6u (A3, ZN, vdd);
    cap 17.80156f (A3, vss);
    cap 32.1792f (15, vss);
    nenh w=23.2u l=1.6u (A4, 15, ZN);
    penh w=29.6u l=1.6u (A4, ZN, vdd);
    cap 16.73162f (A4, vss);
    cap 237.7135f (ZN, vss);
    cap 352.6055f (vdd, vss);
}

network ni01d2 (terminal I, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (I, 9, vss);
    penh w=29.6u l=1.6u (I, 9, vdd);
    cap 17.25418f (I, vss);
    nenh w=23.2u l=1.6u (9, Z, vss);
    penh w=29.6u l=1.6u (9, Z, vdd);
    nenh w=23.2u l=1.6u (9, Z, vss);
    penh w=29.6u l=1.6u (9, Z, vdd);
    cap 212.4335f (9, vss);
    cap 137.8991f (Z, vss);
    cap 221.6881f (vdd, vss);
}

network ni01d3 (terminal I, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (I, 13, vss);
    penh w=29.6u l=1.6u (I, 13, vdd);
    cap 17.25418f (I, vss);
    nenh w=23.2u l=1.6u (13, Z, vss);
    penh w=29.6u l=1.6u (13, Z, vdd);
    nenh w=23.2u l=1.6u (13, Z, vss);
    penh w=29.6u l=1.6u (13, Z, vdd);
    nenh w=23.2u l=1.6u (13, Z, vss);
    penh w=29.6u l=1.6u (13, Z, vdd);
    cap 230.9221f (13, vss);
    cap 269.8614f (Z, vss);
    cap 226.6676f (vdd, vss);
}

network nr02d1 (terminal A1, A2, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, ZN, vss);
    penh w=29.6u l=1.6u (A1, 5, vdd);
    cap 18.34895f (A1, vss);
    nenh w=23.2u l=1.6u (A2, ZN, vss);
    cap 93.39552f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, ZN);
    cap 16.73162f (A2, vss);
    cap 138.8017f (ZN, vss);
    cap 148.0091f (vdd, vss);
}

network nr03d1 (terminal A1, A2, A3, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, ZN, vss);
    penh w=29.6u l=1.6u (A1, 5, vdd);
    cap 18.34895f (A1, vss);
    nenh w=23.2u l=1.6u (A2, ZN, vss);
    cap 93.39552f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, 10);
    cap 16.73162f (A2, vss);
    nenh w=23.2u l=1.6u (A3, ZN, vss);
    cap 93.39552f (10, vss);
    penh w=29.6u l=1.6u (A3, 10, ZN);
    cap 17.80156f (A3, vss);
    cap 174.6004f (ZN, vss);
    cap 151.3947f (vdd, vss);
}

network nr04d1 (terminal A1, A2, A3, A4, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, ZN, vss);
    penh w=29.6u l=1.6u (A1, 5, vdd);
    cap 18.34895f (A1, vss);
    nenh w=23.2u l=1.6u (A2, ZN, vss);
    cap 93.39552f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, 10);
    cap 16.73162f (A2, vss);
    nenh w=23.2u l=1.6u (A3, ZN, vss);
    cap 93.39552f (10, vss);
    penh w=29.6u l=1.6u (A3, 10, 15);
    cap 17.80156f (A3, vss);
    nenh w=23.2u l=1.6u (A4, ZN, vss);
    cap 93.39552f (15, vss);
    penh w=29.6u l=1.6u (A4, 15, ZN);
    cap 16.73162f (A4, vss);
    cap 176.6863f (ZN, vss);
    cap 154.7803f (vdd, vss);
}

network oa01d1 (terminal A1, A2, B1, B2, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 14, ZN);
    penh w=29.6u l=1.6u (A1, 5, ZN);
    cap 17.25418f (A1, vss);
    nenh w=23.2u l=1.6u (B1, 14, vss);
    cap 98.90016f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, vdd);
    nenh w=23.2u l=1.6u (B2, 14, vss);
    penh w=29.6u l=1.6u (B1, 15, vdd);
    cap 34.25853f (B1, vss);
    cap 79.8976f (14, vss);
    nenh w=23.2u l=1.6u (A2, 14, ZN);
    cap 96.2832f (15, vss);
    penh w=29.6u l=1.6u (B2, 15, ZN);
    cap 22.38093f (B2, vss);
    cap 51.05187f (A2, vss);
    cap 338.7383f (ZN, vss);
    cap 131.0149f (vdd, vss);
}

network oa04d1 (terminal A1, A2, B, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 10, vss);
    penh w=29.6u l=1.6u (A1, 5, vdd);
    cap 17.80156f (A1, vss);
    nenh w=23.2u l=1.6u (A2, 10, vss);
    cap 93.39552f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, ZN);
    cap 17.80156f (A2, vss);
    cap 83.93024f (10, vss);
    nenh w=23.2u l=1.6u (B, 10, ZN);
    penh w=29.6u l=1.6u (B, ZN, vdd);
    cap 16.73162f (B, vss);
    cap 138.3606f (ZN, vss);
    cap 249.8065f (vdd, vss);
}

network oa05d1 (terminal A1, A2, B, C, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 10, vss);
    penh w=29.6u l=1.6u (A1, 5, vdd);
    cap 17.80156f (A1, vss);
    nenh w=23.2u l=1.6u (A2, 10, vss);
    cap 93.39552f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, ZN);
    cap 17.80156f (A2, vss);
    cap 83.93024f (10, vss);
    nenh w=23.2u l=1.6u (B, 10, 15);
    penh w=29.6u l=1.6u (B, ZN, vdd);
    cap 16.73162f (B, vss);
    cap 32.1792f (15, vss);
    nenh w=23.2u l=1.6u (C, 15, ZN);
    penh w=29.6u l=1.6u (C, ZN, vdd);
    cap 17.80156f (C, vss);
    cap 236.0687f (ZN, vss);
    cap 254.1937f (vdd, vss);
}

network or02d1 (terminal A1, A2, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 10, vss);
    penh w=29.6u l=1.6u (A1, 5, 10);
    cap 17.80156f (A1, vss);
    nenh w=23.2u l=1.6u (A2, 10, vss);
    cap 94.83936f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, vdd);
    cap 17.25418f (A2, vss);
    nenh w=23.2u l=1.6u (10, Z, vss);
    penh w=29.6u l=1.6u (10, Z, vdd);
    cap 184.5936f (10, vss);
    cap 137.8889f (Z, vss);
    cap 125.4828f (vdd, vss);
}

network or03d1 (terminal A1, A2, A3, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 15, vss);
    penh w=29.6u l=1.6u (A1, 5, 15);
    cap 17.25418f (A1, vss);
    nenh w=23.2u l=1.6u (A2, 15, vss);
    cap 94.83936f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, 10);
    cap 18.32412f (A2, vss);
    nenh w=23.2u l=1.6u (A3, 15, vss);
    cap 94.83936f (10, vss);
    penh w=29.6u l=1.6u (A3, 10, vdd);
    cap 17.25418f (A3, vss);
    nenh w=23.2u l=1.6u (15, Z, vss);
    penh w=29.6u l=1.6u (15, Z, vdd);
    cap 236.2065f (15, vss);
    cap 136.7158f (Z, vss);
    cap 128.1057f (vdd, vss);
}

network or04d1 (terminal A1, A2, A3, A4, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (A1, 20, vss);
    penh w=29.6u l=1.6u (A1, 5, 20);
    cap 17.80156f (A1, vss);
    nenh w=23.2u l=1.6u (A2, 20, vss);
    cap 94.83936f (5, vss);
    penh w=29.6u l=1.6u (A2, 5, 10);
    cap 17.25418f (A2, vss);
    nenh w=23.2u l=1.6u (A3, 20, vss);
    cap 94.83936f (10, vss);
    penh w=29.6u l=1.6u (A3, 10, 15);
    cap 18.32412f (A3, vss);
    nenh w=23.2u l=1.6u (A4, 20, vss);
    cap 94.83936f (15, vss);
    penh w=29.6u l=1.6u (A4, 15, vdd);
    cap 17.25418f (A4, vss);
    nenh w=23.2u l=1.6u (20, Z, vss);
    penh w=29.6u l=1.6u (20, Z, vdd);
    cap 225.8637f (20, vss);
    cap 139.062f (Z, vss);
    cap 132.254f (vdd, vss);
}

network xn02d1 (terminal A1, A2, ZN, vss, vdd)
{
    nenh w=23.2u l=1.6u (A2, 15, vss);
    penh w=29.6u l=1.6u (A2, 15, vdd);
    nenh w=23.2u l=1.6u (A1, 9, vss);
    penh w=29.6u l=1.6u (A1, 10, vdd);
    cap 34.79616f (9, vss);
    nenh w=23.2u l=1.6u (A2, 9, 24);
    cap 96.6048f (10, vss);
    penh w=29.6u l=1.6u (15, 10, 24);
    nenh w=23.2u l=1.6u (15, A1, 24);
    cap 233.9221f (15, vss);
    penh w=29.6u l=1.6u (A2, A1, 24);
    cap 47.82381f (A2, vss);
    cap 161.0434f (A1, vss);
    nenh w=23.2u l=1.6u (24, ZN, vss);
    penh w=29.6u l=1.6u (24, ZN, vdd);
    cap 165.3655f (24, vss);
    cap 139.062f (ZN, vss);
    cap 245.8018f (vdd, vss);
}

network xo02d1 (terminal A1, A2, Z, vss, vdd)
{
    nenh w=23.2u l=1.6u (A2, 15, vss);
    penh w=29.6u l=1.6u (A2, 15, vdd);
    nenh w=23.2u l=1.6u (A1, 9, vss);
    penh w=29.6u l=1.6u (A1, 10, vdd);
    cap 36.24f (9, vss);
    nenh w=23.2u l=1.6u (15, 9, 24);
    cap 98.04864f (10, vss);
    penh w=29.6u l=1.6u (A2, 10, 24);
    nenh w=23.2u l=1.6u (A2, A1, 24);
    cap 50.03136f (A2, vss);
    penh w=29.6u l=1.6u (15, A1, 24);
    cap 232.1002f (15, vss);
    cap 162.1134f (A1, vss);
    nenh w=23.2u l=1.6u (24, Z, vss);
    penh w=29.6u l=1.6u (24, Z, vdd);
    cap 163.9216f (24, vss);
    cap 139.062f (Z, vss);
    cap 245.8018f (vdd, vss);
}

