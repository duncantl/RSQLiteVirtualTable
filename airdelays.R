d = read.csv("2008.csv", nrow = 1e5, stringsAsFactors = FALSE)
c = read.csv("carriers.csv", stringsAsFactors = FALSE)
a = read.csv("airports.csv", stringsAsFactors = FALSE)

library(RSQLite)
library(RSQLiteVirtualTable)

db = dbConnect(SQLite(), ":memory:")

rsqliteVTable(db, a, "airports")
rsqliteVTable(db, c, "carriers")
rsqliteVTable(db, d, "delays")

dbListTables(db)

dbGetQuery(db, "SELECT * FROM carriers WHERE Code = 'UA'")
dbGetQuery(db, "SELECT * FROM carriers WHERE Code In ('UA', 'AA', 'CO')")

dbGetQuery(db, "SELECT * FROM airports WHERE iata IN ('SFO', 'EWR', 'OAK', 'SMF', 'JFK', 'DFW')")

tm = system.time(ans <- dbGetQuery(db, "SELECT Year, Month, DayofMonth, DayOfWeek, airport, city, state, Description, UniqueCarrier FROM delays, airports, carriers  WHERE UniqueCarrier = Code AND Origin = iata AND ArrDelay > 0"))

tmr = system.time( {
                     delayed = subset(d, ArrDelay > 0)
                     delayed = cbind(delayed, Description = c[match(delayed$UniqueCarrier, c$Code), "Description"])
                     delayed = cbind(delayed, a[match(delayed$Origin, a$iata), c("airport", "city", "state")])
                     ansr = delayed[, c("Year", "Month", "DayofMonth", "DayOfWeek", "airport", "city", "state", "Description")]
                   })

tm = system.time(ans <- dbGetQuery(db, "SELECT Year, Month, DayofMonth, DayOfWeek FROM delays WHERE ArrDelay > 0"))
tmr = system.time(ansr <- subset(d, ArrDelay > 0))
tm/tmr  # 1.5

ans <- dbGetQuery(db, "SELECT Year, Month, DayofMonth, DayOfWeek, Description FROM delays, carriers WHERE delays.UniqueCarrier = carriers.Code  AND delays.ArrDelay > 0")
