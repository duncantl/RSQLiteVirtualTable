mkCreateTableString =
function(df, tableName = "tt")
{
  vars = mapply(colSpec, names(df), df)
  paste(sprintf("CREATE TABLE %s", tableName),
           "(",
             paste(vars, collapse = ",\n\t"),
          ");", sep = "\n")
 }

colSpec =
function(varName, vals)
{
   sprintf("%s %s", varName, getSQLType(vals))
}

getSQLType  =
function(obj)
{
  switch(typeof(obj),
          integer = "INTEGER",
          double =,
          numeric = "REAL",
          character = "VARCHAR()")
}
