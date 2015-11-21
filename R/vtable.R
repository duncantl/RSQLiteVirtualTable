
rsqliteVTable =
    #
    #  Create a new module and then the virtual table.
    #
    # We could allow the caller to specify the CREATE TABLE command at this point
    #  rather than asynchronously computing it when we create the table.
    #
    #
function(db, data, name = substitute(data), ..., .args = list(...),
         .createTable = TRUE)
{
    sqliteExtension(db, pkg = "RSQLiteVirtualTable")
    
    name = paste(as.character(name), collapse = "")
    ans = .Call("R_create_df_module", db@Id, name, data)
    if(ans == 0) {
        if(.createTable) {
            args = as.character(unlist(.args))
            args = if(length(args))
                      sprintf("(%s)", paste(args, collapse = ", "))
                   else
                      ""
            query = sprintf("CREATE VIRTUAL TABLE %s USING %s %s",
                              name, name, args)
                         
            ans = dbGetQuery(db, query)
            if(!is.null(ans))
               warning("problem creating the virtual table")
        }
    } else
      warning("problem creating the module for virtual table")

    ans
}
