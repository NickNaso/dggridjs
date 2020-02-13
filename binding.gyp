{
  "targets": [
    {
      "target_name": "dggridjs",
      "sources": [ "addon.cc" ,"<!@(node -p \"require('fs').readdirSync('./src').map(f=>'src/'+f).join(' ')\")"]
    }
  ]
}
