\name{rsqliteVTable}
\alias{rsqliteVTable}
\title{Create an SQLite virutal table directly from an R Data Frame}
\description{
  This function treats a data frame in R as a virtual table in an
  SQLite database. This table can then be used in SQL queries,
  including connecting its rows to other tables in the database.
}
\usage{
rsqliteVTable(db, data, name = substitute(data), ..., .args = list(...),
              .createTable = TRUE)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{db}{the SQLite database connection, created with \code{dbConnect}.}
  \item{data}{the R data frame containing the data to use for the SQL table.}
  \item{name}{the name to use for the SQL table}
  \item{\dots}{additional arguments to be passed to the newly created
    virtual table model. These can, in principle, be used to customize
    its behavior}
  \item{.args}{an alternative way to specify additional arguments,
    rather than \dots}
  \item{.createTable}{logical value. If this is \code{TRUE}, we create
    the virtual table. Otherwise, we just define the module.}
}
\details{
  This defines an SQL module  specific to this data frame and then
  creates the virtual table within the database.
}
\value{
0 for success.
}
\references{
\url{https://www.sqlite.org/vtab.html}
}
\author{
Duncan Temple Lang
}

\note{
  The purpose of this is to facilitate experimentation to explore indexing
  of data frames within SQL.
}


\seealso{
The RSQLite package.
}
\examples{
data(mtcars)
library(RSQLite)
db = dbConnect(SQLite(), ":memory:")
rsqliteVTable(db, mtcars)
dbListTables(db)
dbReadTable(db, "mtcars")
dbGetQuery(db, "SELECT * FROM mtcars WHERE gear > 3")
}

\keyword{IO}

