var addon = require('bindings')('dggridjs');

addon(function(msg){
  console.log(msg); // 'hello world'
});

console.log(addon.hello())