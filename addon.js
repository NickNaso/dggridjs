var addon = require('bindings')('dggridjs');
// const myaddon = require('./addon');
console.log(addon);
addon.DGGSConstruct();
addon.GeoToSeqnum(0,0,function(seqnumber){
  console.log(seqnumber[0]); // 'hello world'
});

