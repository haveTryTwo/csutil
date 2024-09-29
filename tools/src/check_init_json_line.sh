#!/bin/sh
#

tail -n 1 ../data/src_req_line.json | awk -F' ' '{
  src = $1;
  print src;
  cmd="./tools -s \"" $1 "\" -k appid,mch_id,name,age 72";
  dst_json = "";
  while ((cmd | getline line) > 0) {
    dst_json = dst_json line;
  }
  close(cmd);
  print dst_json
}'
