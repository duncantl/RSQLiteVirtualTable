# Overview
This is a simple implementation and experiment for using R objects (specifically data frames for now)
as virtual tables in SQLite. This uses SQLite's virtual table API and allows us to use the in-memory
R object as if it were a data frame.

# Interface
The idea here is that we create an SQLite module for each data frame we want to use as a table within a given database.
We create the module and then instantiate a table with the same name as that module. (This name can be different
from the name of the data frame in R (which may have no name, of course).)

We could use a single module, and then use it to create different tables each with a different data frame.
To specify the data frame for such a table, we have to pass the name of the R object and then retrieve it.
We can do this via the "command line" arguments we pass to the module in
```
 CREATE VIRTUAL TABLE tblName USING  moduleName(arg1, arg2, ...)
```

However, this involves using a convention that identifies where the R object is to be found, e.g.,
the global environment.

By creating the module with the data frame, and then instantiating the table from that module which
has direct access to the data frame when the table is created, we avoid global lookups and using
global variables!


# Purpose
We might use this explore different indexing.
We might for example split() the data frame first and use the different sub-data frames
as a conceptual large data frame but with sub-groups already indexed into their own data frames.

Currently, we do not do anything special about indexing. We just iterate over the rows
of the data frame as they were given to us.
We can experiment with implementing different ideas for the the BestIndex and Filter routines in the API

[The SQLite API] https://www.sqlite.org/vtab.html



