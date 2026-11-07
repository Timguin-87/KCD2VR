#include "core/page_memo_pf.h"
#include <cstdio>
#include <cstdlib>
int main(){using kcd2vr::page_memo_pf::Table;unsigned n=0;auto ok=[&](bool b){++n;if(!b){std::printf("FAIL %u\n",n);std::exit(1);}};
 Table t{};t.pageSize=4096;std::uintptr_t a=0,b=0;
 ok(!t.lookup(0x10010,8,100,150,a,b));                       // empty
 t.insert(0x10000,0x11000,100);
 ok(t.lookup(0x10010,8,100,150,a,b)&&a==0x10000&&b==0x11000); // inside one page
 ok(t.lookup(0x10010,8,250,150,a,b));                        // exactly at ttl
 ok(!t.lookup(0x10010,8,251,150,a,b));                       // expired = re-query (same window as V169)
 ok(!t.lookup(0x10ffc,8,100,150,a,b));                       // spans into an unproven page
 t.insert(0x11000,0x12000,100);
 ok(t.lookup(0x10ffc,8,100,150,a,b)&&a==0x10000&&b==0x12000); // two proven pages
 ok(!t.lookup(0x10000,0x3000,100,150,a,b));                  // wider than 2 pages never answered here
 t.insert(0x10000+512*4096,0x11000+512*4096,100);           // collides with slot of 0x10000
 ok(!t.lookup(0x10010,8,100,150,a,b));                       // evicted page is not trusted
 ok(t.lookup(0x10010+512*4096,8,100,150,a,b));
 t.insert(0x20000,0x24000,100);                             // >2 pages: ignored
 ok(!t.lookup(0x20000,8,100,150,a,b));
 t.insert(0x30010,0x31000,100);                             // unaligned: ignored
 ok(!t.lookup(0x30010,8,100,150,a,b));
 ok(!t.lookup(0x40000,8,50,150,a,b));                        // never inserted, clock before stamp
 t.insert(0x40000,0x41000,100);ok(!t.lookup(0x40000,8,99,150,a,b)); // now<stamp not trusted
 t.clear();ok(!t.lookup(0x11000,8,100,150,a,b));
 std::printf("PASS %u page_memo_pf\n",n);}
