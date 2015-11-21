library(RSQLiteVirtualTable)
library(RSQLite)
library(RSQLiteUDF)

db = dbConnect(SQLite(), ":memory:")

#sqliteExtension(db)
#sqliteExtension(db, pkg = "RSQLiteUDF")
sqliteExtension(db, pkg = "RSQLiteVirtualTable")

data(mtcars)

rsqliteVTable(db, mtcars)
dbListTables(db)
d = dbReadTable(db, "mtcars")
dim(d)
stopifnot(all(mapply(identical, mtcars, d)))

# The row names are different.

###########
test = data.frame(x = rnorm(10), y = rnorm(10, 30))
rsqliteVTable(db, test, "testData")
dbReadTable(db, "testData")


##################################
# A second table.
dd = mtcars
dd$model = rownames(dd)
rsqliteVTable(db, dd, "mtcarsExtended")
#ans = dbSendQuery(db, "CREATE VIRTUAL TABLE mtcarsExtended USING mtcarsExtended(a, b, c)")
dbListTables(db)
tmp = dbReadTable(db, "mtcarsExtended")
table(sapply(tmp, class))
