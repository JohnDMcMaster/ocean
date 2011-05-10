/*  This file was automatically created by blif2sls.

    Mon Apr  5 09:36:59 1993
*/

#include<oplib1_93.ext>
network h( terminal IN_0, IN_1, OUT_0, R, CK, vss, vdd)
{
 {inst0}    dfr11( n_2355_, R, CK, LatchOut_v2, , vss, vdd);
 {inst1}    dfr11( OUT_0, R, CK, LatchOut_v3, , vss, vdd);
 {inst2}    iv110( LatchOut_v3, n_2314_, vss, vdd);
 {inst3}    iv110( IN_0, n_2315_, vss, vdd);
 {inst4}    no210( IN_0, IN_1, n_18_, vss, vdd);
 {inst5}    mu111( LatchOut_v2, n_2314_, n_18_, n_2355_, vss, vdd);
 {inst6}    no210( IN_1, n_2315_, n_17_, vss, vdd);
 {inst7}    mu111( LatchOut_v3, LatchOut_v2, n_17_, OUT_0, vss, vdd);
}

