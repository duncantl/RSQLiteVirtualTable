mkCreateTableString =
    #
    # Generate the SQL command as a string to create an SQL table
    # corresponding to the R data frame.
    #
    # We'll extend this to handle Date, etc. and also allow specififying 
    # keys and indices.
    #
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
  if(class(obj) == "Date")
     return("DATE")

  if(inherits(obj, "POSIXct"))
     return("DATETIME")  
       
  switch(typeof(obj),
          logical =,
          integer = "INTEGER",
          double =,
          numeric = "REAL",
          character = sprintf("VARCHAR(%d)", max(nchar(obj))))
}
