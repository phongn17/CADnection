function (doc) {
    var id = doc.getId();
    var regexExt = /\.json$/g;
    var found  = id.match(regexExt);
    
    if (!found) {
      return doc;
    }
    
    return null;
  }