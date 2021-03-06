## recursive: Whether to show the details of all elements
## of the object x which are not directly associated with x
## showAttributes: whether to show the information of the attributes of x
isSharedSEXP <- function(x, showAttributes = FALSE, depth = 1){
    result <- FALSE
    if (is.altrep(x)) {
        result <- C_isShared(x)
    }
    attrs <- attributes(x)
    if(showAttributes&&!is.null(attrs)&& depth>0){
        attrsShared <- rep(list(NA),length(attrs))
        names(attrsShared) <- paste0(names(attrs), "Shared")
        for(i in seq_along(attrs)){
            attrsShared[[i]] <- is.shared(attrs[[i]], showAttributes = FALSE, depth = depth - 1)
        }
        attributes(result) <- attrsShared
    }
    result
}

isSharedANY <- function(x,...,depth,showAttributes){
    ## If the object is neither an S4 object or a list
    ## Just check if the SEXP is a shared altrep object
    result <- isSharedSEXP(x,showAttributes=showAttributes)
    result
}
isSharedList <- function(x,...,depth,showAttributes){
    result <- lapply(x, function(x,...)is.shared(x,...),
                     ...,depth=depth-1L, showAttributes=FALSE)
    if(depth<=0){
        result <- any(unlist(result))
    }
    attrs <- attributes(x)
    if(showAttributes&&!is.null(attrs)){
        attrsShared <- is.shared(attrs, showAttributes = FALSE, depth = 1)
        names(attrsShared) <- paste0(names(attrs), "Shared")
        if(depth>0){
            if("names"%in%names(attrs)){
                attrsShared["names"] <-attrs["names"]
            }
        }
        attributes(result) <- attrsShared
    }
    result
}

isSharedS4 <- function(x,...,depth,showAttributes){
    slots <- slotNames(x)
    result <- vector("list",length(slots))
    for(i in seq_along(slots)){
        result[[i]] <- is.shared(slot(x, slots[i]),...,depth=depth-1L, showAttributes = FALSE)
    }
    names(result) <- slots
    if(".Data" %in% names(result)){
        if(isSharableAtomic(x)){
            result[[".Data"]] <- isSharedSEXP(x,showAttributes = FALSE)
        }
        if(isSEXPList(x)&&is.list(result[[".Data"]])){
            names(result[[".Data"]]) <- names(x)
        }
    }
    ## remove the empty slot
    # result <- result[unlist(lapply(result, function(x) length(x) != 0))]
    if(depth<=0){
        result <- any(unlist(result))
    }
    result
}



#' @rdname is.shared
#' @export
setMethod("is.shared", "ANY", function(x,...,depth,showAttributes){
    if(isS4(x)){
        return(isSharedS4(x,...,depth=depth,showAttributes=showAttributes))
    }
    if(is.list(x)||is.environment(x)){
        return(isSharedList(x,...,depth=depth,showAttributes=showAttributes))
    }
    isSharedANY(x,...,depth=depth,showAttributes=showAttributes)
})





