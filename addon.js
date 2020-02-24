var DGGSObject = require('bindings')('dggridjs');

console.log(DGGSObject);
DGGSObject.GeoToSeqnum(0,0,function(seqNumber){
  console.log(seqNumber); // must return 10
});

// DGGSObject.DGGSConstruct("ISEA",30,"HEXAGON",0,0,0,0);
