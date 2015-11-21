\name{mkCreateTableString}
\alias{mkCreateTableString}
\title{Generate the SQL code for the CREATE TABLE command to represent
  an R data frame}
\description{
  This is a function that is called by the C code in this package when the
  virtual table for a data frame is being created.
  This maps the type of each column to its SQL equivalent.
}
\usage{
mkCreateTableString(df, tableName = "tt")
}
\arguments{
  \item{df}{the R data frame}
  \item{tableName}{the name of the SQL table.}
}
\value{
  A character string containing the SQL code for the
  \code{CREATE TABLE} command.
}
\references{
\url{https://www.sqlite.org/vtab.html}  
}
\author{
Duncan Temple Lang
}

\seealso{
\code{rsqliteVTable}
}
\examples{
data(mtcars)
mkCreateTableString(mtcars)
}
\keyword{IO}
