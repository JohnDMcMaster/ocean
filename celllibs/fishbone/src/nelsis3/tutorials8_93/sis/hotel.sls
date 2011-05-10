/*  This file was automatically created by blif2sls.

    Wed Jun 30 16:46:42 1993
*/

#include<oplib.ext>

/*     Latch order:                                          */
/*                          LatchOut_v2 ,                    */
/*                          LatchOut_v3 ,                    */

/*     Code Assignment: off1  10            */
/*     Code Assignment: off0  00            */
/*     Code Assignment: on1  01            */
/*     Code Assignment: on0  11            */

network hotel( terminal IN_0, IN_1, OUT_0, R, CK, vss, vdd)
{
 {inst0}    dfr11( n_2356_, R, CK, LatchOut_v2, vss, vdd);
 {inst1}    dfr11( n_2357_, R, CK, LatchOut_v3, vss, vdd);
 {inst2}    iv110( LatchOut_v2, n_2317_, vss, vdd);
 {inst3}    iv110( IN_0, n_2315_, vss, vdd);
 {inst4}    no210( IN_0, IN_1, n_17_, vss, vdd);
 {inst5}    mu111( LatchOut_v2, LatchOut_v3, n_17_, n_2356_, vss, vdd);
 {inst6}    no210( IN_1, n_2315_, n_18_, vss, vdd);
 {inst7}    mu111( LatchOut_v3, n_2317_, n_18_, n_2357_, vss, vdd);
            net{LatchOut_v3,OUT_0};
}

