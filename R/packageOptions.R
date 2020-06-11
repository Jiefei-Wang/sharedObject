sharedAtomicOptions = c("copyOnWrite", "sharedSubset", "sharedCopy")

globalSettings = new.env()
globalSettings$copyOnWrite = TRUE
globalSettings$sharedSubset = FALSE
globalSettings$sharedCopy = FALSE
globalSettings$mustWork = FALSE

#' Get or set the global options for the SharedObject package
#'
#' @param ...
#' `setSharedObjectOptions`: the options you want to set, it can be copyOnWrite,
#' sharedSubset and sharedCopy.
#'
#' `getSharedObjectOptions`: A character vector. If empty, all options will be returned.
#' @return
#' `setSharedObjectOptions`: No return value
#' `getSharedObjectOptions`: A list of the package options or a single value
#' @examples
#' getSharedObjectOptions()
#' setSharedObjectOptions(copyOnWrite = FALSE)
#' getSharedObjectOptions()
#' getSharedObjectOptions("copyOnWrite")
#'
#'
#' @rdname sharedObjectOptions
#' @export
setSharedObjectOptions <- function(...) {
    options = list(...)
    options = checkOptionExistance(options)
    for (i in seq_along(options)) {
        globalSettings[[names(options)[i]]] = options[[i]]
    }
}

#' @rdname sharedObjectOptions
#' @export
getSharedObjectOptions <- function(...) {
    options = c(...)
    if (length(options) == 0) {
        return(as.list(globalSettings))
    } else{
        if(length(options)==1){
            return(globalSettings[[options]])
        }else{
            return(as.list(globalSettings)[options])
        }
    }
}

## Check if options exist or not
## return the options that exist
checkOptionExistance <- function(options) {
    noneExistOptions = !names(options) %in% names(globalSettings)
    if (any(noneExistOptions)) {
        vapply(paste0(names(options)[noneExistOptions]), function(x)
            warning(paste0("The option `", x, "` does not exist")), character(1))
    }
    options = options[!noneExistOptions]
    options
}

## Fill the options with their default argument
## if not specified
completeOptions <- function(...) {
    options <- list(...)
    defaultOptions <- getSharedObjectOptions()
    ind <- !names(defaultOptions) %in% names(options)
    c(options,defaultOptions[ind])
}


