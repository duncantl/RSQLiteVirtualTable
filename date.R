# http://ironbark.xtelco.com.au/subjects/DB/2011s2/resources/dateSPJ.html
P = read.csv("P", stringsAsFactors = FALSE, header = FALSE)
S = read.csv("S", stringsAsFactors = FALSE, header = FALSE)
SP = read.csv("SP", stringsAsFactors = FALSE, header = FALSE)

names(P) = c("PID", "Name", "Color", "Weight", "City")
names(S) = c("SID", "Name", "Status", "City")
names(SP) = c("SID", "PID", "Qty")

library(RSQLite)
library(RSQLiteVirtualTable)

db = dbConnect(SQLite(), ":memory:")

rsqliteVTable(db, P)
rsqliteVTable(db, S)
rsqliteVTable(db, SP)

dbListTables(db)

dbReadTable(db, "P")
d = dbGetQuery(db, "SELECT * FROM P, S")



