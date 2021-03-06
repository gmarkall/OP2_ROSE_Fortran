#include "real.h"

void
save_soln(REAL *q, REAL *qold);

void 
res_calc(REAL *x1,  REAL *x2,  REAL *q1,  REAL *q2,
                     REAL *adt1,REAL *adt2,REAL *res1,REAL *res2);

void 
bres_calc(REAL *x1,  REAL *x2,  REAL *q1,
                      REAL *adt1,REAL *res1,int *bound);


void
adt_calc1(REAL *x1,REAL *x2,REAL *x3,REAL *x4,REAL *q,REAL *adt);

void
adt_calc2(REAL *x1,REAL *x2,REAL *x3,REAL *x4,REAL *q,REAL *adt);

void
update(REAL *qold, REAL *q, REAL *res, REAL *adt, REAL *rms);
