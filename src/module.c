#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

#include <Rdefines.h>

const char * const
R_getCreateTableStr(SEXP df, const char * const tblName)
{
    SEXP e, ans;
    PROTECT(e = allocVector(LANGSXP, 3));
    SETCAR(e, Rf_install("mkCreateTableString"));
    SETCAR(CDR(e), df);
    SETCAR(CDR(CDR(e)), ScalarString(mkChar(tblName)));

    int err = 0;
    PROTECT(ans = R_tryEval(e, R_GlobalEnv, &err));
    if(err) {
	return NULL;
    }

    const char * const str = CHAR(STRING_ELT(ans, 0));
    UNPROTECT(2);
    return(strdup(str));
}


typedef struct {
    sqlite3_vtab base;
    SEXP df;
    int numRows;
    int numCols;
} rdfm_vtab;

typedef struct {
    sqlite3_vtab_cursor base;
    rdfm_vtab *vtab;
    int curRow;
} rdfm_cursor;

int rdfmCreate(sqlite3 *db,  void *pAux,  int argc, const char *const*argv,
	       sqlite3_vtab **ppVTab,  char **pzErr)
{
    int rc = SQLITE_OK;
    rdfm_vtab *pNew = 0;
    int nDbName;
    const char *zDbName = argv[1];
    nDbName = (int)strlen(zDbName);
    pNew = sqlite3_malloc( sizeof(*pNew) + nDbName + 1);

    if( pNew == 0 ){
	rc = SQLITE_NOMEM;
    } else {
	memset(pNew, 0, sizeof(*pNew));
	pNew->df = (SEXP) pAux;
	pNew->numCols = Rf_length(pNew->df);
        pNew->numRows = Rf_length(VECTOR_ELT(pNew->df, 0));

	const char * const createTableStr = R_getCreateTableStr((SEXP) pAux, argv[2]);
	sqlite3_declare_vtab(db, createTableStr);
    }

    if(rc == SQLITE_OK)
	*ppVTab = (sqlite3_vtab *) pNew;

    return rc;
}

/* We should probably not set this in the methods and so make the module and table eponymous.*/
int rdfmConnect(sqlite3* db, void *pAux, int argc, const char *const*argv,
                sqlite3_vtab **ppVTab, char**err)
{
  return(SQLITE_OK);
}

int rdfmBestIndex(sqlite3_vtab *pVTab, sqlite3_index_info* info)
{
//    printf("BestIndex: constraints %d\n", info->nConstraint);
    info->idxNum = 0;
    info->estimatedCost = 1e50;
    return(SQLITE_OK);
}

int rdfmDisconnect(sqlite3_vtab *pVTab)
{
  rdfm_vtab *p = (rdfm_vtab*)pVTab;
  sqlite3_free(p);
  return(SQLITE_OK);
}

int rdfmDestroy(sqlite3_vtab *pVTab)
{
//XXX 
// DROP TABLE IF EXISTS....
    rdfmDisconnect(pVTab);  // Call the method from the vtab routine pointer.
    return(SQLITE_OK);
}


int rdfmOpen(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor)
{
//printf("rdfmOpen\n");
  rdfm_cursor *pCur;
  pCur = sqlite3_malloc( sizeof(*pCur) );
  if( pCur==0 ) 
     return SQLITE_NOMEM;

  memset(pCur, 0, sizeof(*pCur));
  pCur->vtab = (rdfm_vtab *) pVTab;
  pCur->curRow = 0;
  *ppCursor = &pCur->base;
  return(SQLITE_OK);
}

int rdfmClose(sqlite3_vtab_cursor *cur)
{
    rdfm_cursor *pCur = (rdfm_cursor *) cur;
    sqlite3_free(pCur);
//printf("rdfmClose\n");
    return(SQLITE_OK);
}

int rdfmFilter(sqlite3_vtab_cursor *cur, int idxNum, const char *idxStr,
              int argc, sqlite3_value **argv)
{
    rdfm_cursor *pCur = (rdfm_cursor *)cur;
    pCur->curRow = 0;    
    return(SQLITE_OK);
}



int rdfmNext(sqlite3_vtab_cursor *cur)
{
  rdfm_cursor *pCur = (rdfm_cursor *)cur;
  pCur->curRow++;
// printf("next row %d\n", pCur->curRow);
  return(SQLITE_OK);
}

int rdfmEof(sqlite3_vtab_cursor *cur)
{
  rdfm_cursor *pCur = (rdfm_cursor *)cur;
  return pCur->curRow >= pCur->vtab->numRows;
}


/* Add support for NAs and map them to NULL via sqlite3_result_null().

   Also raw maps to blob. */
int rdfmColumn(sqlite3_vtab_cursor *cur, sqlite3_context* ctxt, int i)
{
  rdfm_cursor *pCur = (rdfm_cursor *)cur;

  if(i >= pCur->vtab->numCols) {
      sqlite3_result_null(ctxt);
  } else {
      SEXP col = VECTOR_ELT(pCur->vtab->df, i);
      switch(TYPEOF(col)) {
         case INTSXP:
	     sqlite3_result_int(ctxt, INTEGER(col)[pCur->curRow]);
	     break;
         case REALSXP:
	     sqlite3_result_double(ctxt, REAL(col)[pCur->curRow]);
	     break;
         case STRSXP: {
	     const char * const str = CHAR(STRING_ELT(col, pCur->curRow));
	     sqlite3_result_text(ctxt, str, -1, SQLITE_TRANSIENT);
	     break;
         default:
	     PROBLEM "unhandled type (%d) in rdfmColumn", TYPEOF(col)
    	     WARN;
	 }
      }
  }

  return(SQLITE_OK);
}

int rdfmRowid(sqlite3_vtab_cursor *cur, sqlite3_int64 *pRowid)
{
  rdfm_cursor *pCur = (rdfm_cursor *)cur;
  *pRowid = pCur->curRow;
  return(SQLITE_OK);
}

int rdfmUpdate(sqlite3_vtab *tab, int argc, sqlite3_value **argv, sqlite3_int64 *rowid)
{
//XXXX
  return(SQLITE_OK);
}



#if 0
struct sqlite3_module {
  int iVersion;
  int (*xCreate)(sqlite3*, void *pAux,
               int argc, const char *const*argv,
               sqlite3_vtab **ppVTab, char**);
  int (*xConnect)(sqlite3*, void *pAux,
               int argc, const char *const*argv,
               sqlite3_vtab **ppVTab, char**);
  int (*xBestIndex)(sqlite3_vtab *pVTab, sqlite3_index_info*);
  int (*xDisconnect)(sqlite3_vtab *pVTab);
  int (*xDestroy)(sqlite3_vtab *pVTab);
  int (*xOpen)(sqlite3_vtab *pVTab, sqlite3_vtab_cursor **ppCursor);
  int (*xClose)(sqlite3_vtab_cursor*);
  int (*xFilter)(sqlite3_vtab_cursor*, int idxNum, const char *idxStr,
                int argc, sqlite3_value **argv);
  int (*xNext)(sqlite3_vtab_cursor*);
  int (*xEof)(sqlite3_vtab_cursor*);
  int (*xColumn)(sqlite3_vtab_cursor*, sqlite3_context*, int);
  int (*xRowid)(sqlite3_vtab_cursor*, sqlite3_int64 *pRowid);
  int (*xUpdate)(sqlite3_vtab *, int, sqlite3_value **, sqlite3_int64 *);
  int (*xBegin)(sqlite3_vtab *pVTab);
  int (*xSync)(sqlite3_vtab *pVTab);
  int (*xCommit)(sqlite3_vtab *pVTab);
  int (*xRollback)(sqlite3_vtab *pVTab);
  int (*xFindFunction)(sqlite3_vtab *pVtab, int nArg, const char *zName,
                       void (**pxFunc)(sqlite3_context*,int,sqlite3_value**),
                       void **ppArg);
  int (*xRename)(sqlite3_vtab *pVtab, const char *zNew);
  /* The methods above are in version 1 of the sqlite_module object. Those 
  ** below are for version 2 and greater. */
  int (*xSavepoint)(sqlite3_vtab *pVTab, int);
  int (*xRelease)(sqlite3_vtab *pVTab, int);
  int (*xRollbackTo)(sqlite3_vtab *pVTab, int);
};
#endif

static sqlite3_module RdataFrameModuleMethods  = {
    0,
    rdfmCreate,
    NULL,//    rdfmConnect,
    rdfmBestIndex,
    rdfmDisconnect,
    rdfmDestroy,
    rdfmOpen,
    rdfmClose,
    rdfmFilter,
    rdfmNext,
    rdfmEof,
    rdfmColumn,
    rdfmRowid,
    rdfmUpdate,
    NULL, // begin 
    NULL, // sync
    NULL, // commit
    NULL, // rollback
    NULL, // findfunction
    NULL, // rdfmRename
    NULL, // savepoint
    NULL, // release
    NULL // rollbackto
};



// From RSQLite's rsqlite.h
typedef struct SQLiteConnection {
  sqlite3* drvConnection;  
  void  *resultSet;
    // RSQLiteException *exception;
} SQLiteConnection;

sqlite3 *
GET_SQLITE_DB(SEXP rdb)
{
    SQLiteConnection * con = (SQLiteConnection *) R_ExternalPtrAddr(rdb);
    return(con->drvConnection);
}



SEXP
R_create_df_module(SEXP r_db, SEXP r_name, SEXP data)
{
    R_PreserveObject(data);
    sqlite3 *db = GET_SQLITE_DB(r_db);
    int status = sqlite3_create_module_v2(db, CHAR(STRING_ELT(r_name, 0)), &RdataFrameModuleMethods, data, R_ReleaseObject);
    return(ScalarInteger(status));
}




int sqlite3_extension_init(
  sqlite3 *db, 
  char **pzErrMsg, 
  const sqlite3_api_routines *pApi
){
  SQLITE_EXTENSION_INIT2(pApi);
  return SQLITE_OK;
}
